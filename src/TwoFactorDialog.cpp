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
    return remember_device_;
}

void TwoFactorDialog::twoFAEntered(QString token, bool remember_device) {
    token_ = token;
    remember_device_ = rember_device;
}
