#include "LoginDialog.h"
#include "TwoFactorDialog.h"

#include <sailfishapp.h>

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
}

void LoginDialog::doLogin() {
    if (!validateInputs()) {
        return;
    }
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
