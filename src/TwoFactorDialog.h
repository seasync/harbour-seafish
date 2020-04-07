#ifndef TWOFACTORDIALOG_H
#define TWOFACTORDIALOG_H

#include <QObject>

class TwoFactorDialog : public QObject
{
    Q_OBJECT
public:
    explicit TwoFactorDialog(QObject *parent = nullptr);

signals:
    void Token_needed();

public slots:
    void twoFAToken(QString token);
};

#endif // TWOFACTORDIALOG_H
