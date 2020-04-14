#include "TwoFactorDialog.h"

TwoFactorDialog::TwoFactorDialog(QObject *parent) : QObject(parent) {}

bool TwoFactorDialog::finished() {
    return !token_.isEmpty();
}

QString TwoFactorDialog::getText() {
    return token_;
}
