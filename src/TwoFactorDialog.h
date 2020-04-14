#ifndef TWOFACTORDIALOG_H
#define TWOFACTORDIALOG_H

#include <QObject>

class TwoFactorDialog : public QObject
{
    Q_OBJECT
public:
    explicit TwoFactorDialog(QObject *parent = nullptr);
    bool finished();
    QString getText();
    bool rememberDeviceChecked();

signals:
    void Token_needed();

public slots:
    void twoFAToken(QString token);

private:
    QString token_;
};

#endif // TWOFACTORDIALOG_H
