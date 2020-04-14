#include "LoginDialog.h"
#include "TwoFactorDialog.h"

#include <sailfishapp.h>
#include <QtNetwork>
#include <QStringList>
#include <QSettings>

#include "seafile/settings-mgr.h"
#include "seafile/account-mgr.h"
#include "seafile/seafile-applet.h"
#include "api/api-error.h"
#include "api/requests.h"
#include "utils/utils.h"

namespace {

const char *kUsedServerAddresses = "UsedServerAddresses";
const char *const kPreconfigureServerAddr = "PreconfigureServerAddr";
const char *const kPreconfigureServerAddrOnly = "PreconfigureServerAddrOnly";
const char *const kPreconfigureShibbolethLoginUrl = "PreconfigureShibbolethLoginUrl";

// 1. Returned by the server "X-Seafile-OTP: required" when login (if the user has 2FA enabled)
// 2. The client would send this header, e.g. "X-Seafile-OTP: 123456" when login again
const char *const kSeafileOTPHeader = "X-SEAFILE-OTP";
// This header tells sever to remember this device and do not ask this
// device for 2fa token in the next 90 days. The server would return a
// "S2FA" token which should be saved by the device.
const char *const kRememberDeviceHeader = "X-SEAFILE-2FA-TRUST-DEVICE";
// 1. The "S2FA" token returned by the server when "X-SEAFILE-2FA-TRUST-DEVICE: 1" is used to login
// 2. The client should send this device when login again so the server won't ask it for the 2FA token.
const char *const kTwofactorHeader = "X-SEAFILE-S2FA";

const char *const kSchemeHTTPS = "https";

QStringList getUsedServerAddresses()
{
    QSettings settings;
    settings.beginGroup(kUsedServerAddresses);
    QStringList retval = settings.value("main").toStringList();
    settings.endGroup();
    QString preconfigure_addr = seafApplet->readPreconfigureExpandedString(kPreconfigureServerAddr);
    if (!preconfigure_addr.isEmpty() && !retval.contains(preconfigure_addr)) {
        retval.push_back(preconfigure_addr);
    }
    return retval;
}

void saveUsedServerAddresses(const QString &new_address)
{
    QSettings settings;
    settings.beginGroup(kUsedServerAddresses);
    QStringList list = settings.value("main").toStringList();
    // put the last used address to the front
    list.removeAll(new_address);
    list.insert(0, new_address);
    settings.setValue("main", list);
    settings.endGroup();
}

} // namespace

LoginDialog::LoginDialog(QObject *parent)
    : QObject(parent) {

}

LoginDialog::~LoginDialog() {}

void LoginDialog::loginCall(const QUrl sfinstance, const QString username,
                               const QString password, const QString clientname)  {
    url_ = sfinstance;
    username_ = username;
    password_ = password;
    computer_name_ = clientname;
    doLogin();
}

void LoginDialog::doLogin() {
    saveUsedServerAddresses(url_.toString());

    if (request_) {
        request_->deleteLater();
    }

    request_ = new LoginRequest(url_, username_, password_, computer_name_);

    if (!two_factor_auth_token_.isEmpty()) {
        request_->setHeader(kSeafileOTPHeader, two_factor_auth_token_);
    }

    if (is_remember_device_) {
        request_->setHeader(kRememberDeviceHeader, "1");
    }

    Account account =  seafApplet->accountManager()->getAccountByHostAndUsername(url_.host(), username_);
    if (account.hasS2FAToken()) {
        request_->setHeader(kTwofactorHeader, account.s2fa_token);
    }

    connect(request_, SIGNAL(success(const QString&, const QString&)),
            this, SLOT(loginSuccess(const QString&, const QString&)));

    connect(request_, SIGNAL(failed(const ApiError&)),
            this, SLOT(loginFailed(const ApiError&)));

    request_->send();
}

void LoginDialog::onNetworkError(const QNetworkReply::NetworkError& error, const QString& error_string) {
    showWarning(tr("Network Error:\n %1").arg(error_string));
}

void LoginDialog::onSslErrors(QNetworkReply* reply, const QList<QSslError>& errors) {
    const QSslCertificate &cert = reply->sslConfiguration().peerCertificate();
    qDebug() << "\n= SslErrors =\n" << dumpSslErrors(errors);
    qDebug() << "\n= Certificate =\n" << dumpCertificate(cert);
    showWarning("SSL ERROR");
}

void LoginDialog::onHttpError(int code) {
    const QNetworkReply* reply = request_->reply();
    if (reply->hasRawHeader(kSeafileOTPHeader) &&
        QString(reply->rawHeader(kSeafileOTPHeader)) == "required") {
        TwoFactorDialog two_factor_dialog;
        if (two_factor_dialog.exec() == QDialog::Accepted) {
            two_factor_auth_token_ = two_factor_dialog.getText();
            is_remember_device_ = two_factor_dialog.rememberDeviceChecked();
        }

        if (!two_factor_auth_token_.isEmpty()) {
            doLogin();
            return;
        }
    } else {
        QString err_msg, reason;
        if (code == 400) {
            reason = tr("Incorrect email or password");
        } else if (code == 429) {
            reason = tr("Logging in too frequently, please wait a minute");
        } else if (code == 500) {
            reason = tr("Internal Server Error");
        }

        if (reason.length() > 0) {
            err_msg = tr("Failed to login: %1").arg(reason);
        } else {
            err_msg = tr("Failed to login");
        }

        showWarning(err_msg);
    }
}

void LoginDialog::loginSuccess(const QString& token, const QString& s2fa_token)
{
    // Some server configures mandatory http -> https redirect. In
    // such cases, we must update the server url to use https,
    // otherwise libcurl (used by the daemon) would be have trouble
    // dealing with it.
    if (url_.scheme() != kSchemeHTTPS && request_->reply()->url().scheme() == kSchemeHTTPS) {
        qWarning("Detected server %s redirects to https", toCStr(url_.toString()));
        url_.setScheme(kSchemeHTTPS);
    }
    if (account_info_req_) {
        account_info_req_->deleteLater();
    }
    account_info_req_ =
        new FetchAccountInfoRequest(Account(url_, username_, token, 0, false, true, s2fa_token));
    connect(account_info_req_, SIGNAL(success(const AccountInfo&)), this,
            SLOT(onFetchAccountInfoSuccess(const AccountInfo&)));
    connect(account_info_req_, SIGNAL(failed(const ApiError&)), this,
            SLOT(onFetchAccountInfoFailed(const ApiError&)));
    account_info_req_->send();
}

void LoginDialog::onFetchAccountInfoFailed(const ApiError& error)
{
    loginFailed(error);
}

void LoginDialog::loginFailed(const ApiError& error)
{
    switch (error.type()) {
    case ApiError::SSL_ERROR:
        onSslErrors(error.sslReply(), error.sslErrors());
        break;
    case ApiError::NETWORK_ERROR:
        onNetworkError(error.networkError(), error.networkErrorString());
        break;
    case ApiError::HTTP_ERROR:
        onHttpError(error.httpErrorCode());
    default:
        // impossible
        break;
    }
}

void LoginDialog::onFetchAccountInfoSuccess(const AccountInfo& info)
{
    Account account = account_info_req_->account();
    // The user may use the username to login, but we need to store the email
    // to account database
    account.username = info.email;
    account.isAutomaticLogin =
        mAutomaticLogin->checkState() == Qt::Checked;
    seafApplet->accountManager()->setCurrentAccount(account);
    seafApplet->accountManager()->updateAccountInfo(account, info);
    done(QDialog::Accepted);
}
