#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QObject>
#include <QNetworkReply>
#include <QUrl>

class LoginRequest;
class FetchAccountInfoRequest;
class ApiError;
class AccountInfo;

class LoginDialog : public QObject
{
    Q_OBJECT
public:
    explicit LoginDialog(QObject *parent = nullptr);
    ~LoginDialog() noexcept;

signals:
    void loginSuccess();
    void Warning(const QString& msg);

public slots:
    void loginCall(const QUrl sfinstance, const QString username,
                      const QString password, const QString clientname);

private slots:
    void doLogin();
    void loginSuccess(const QString& token, const QString& s2fa_token);
    void loginFailed(const ApiError& error);
    void onFetchAccountInfoFailed(const ApiError& error);
    void onFetchAccountInfoSuccess(const AccountInfo& info);

private:

    void onNetworkError(const QNetworkReply::NetworkError& error, const QString& error_string);
    void onSslErrors(QNetworkReply *reply, const QList<QSslError>& errors);
    void onHttpError(int code);

    QUrl url_;
    QString username_;
    QString password_;
    QString computer_name_;
    bool is_remember_device_;
    LoginRequest *request_;
    FetchAccountInfoRequest *account_info_req_;

    QString two_factor_auth_token_;

};

#endif // LOGINDIALOG_H
