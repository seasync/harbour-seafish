#include "TwoFactorDialog.h"
#include "seafile/seafile-applet.h"

TwoFactorDialog::TwoFactorDialog(QObject *parent) : QObject(parent) {}

bool TwoFactorDialog::finished() {
    return !token_.isEmpty();
}

QString TwoFactorDialog::getText() {
    return token_;
}

bool TwoFactorDialog::rememberDeviceChecked() {
    return mRememberDevice->isChecked();
}

void TwoFactorDialog::twoFAToken(QString token) {
    token_ = token;
}
