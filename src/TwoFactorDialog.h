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
    void TokenNeeded();

public slots:
    void twoFAEntered(QString token, bool remember_device);

private:
    QString token_;
    bool remember_device_;
};

#endif // TWOFACTORDIALOG_H
