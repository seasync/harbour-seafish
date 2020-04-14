#include "TwoFactorDialog.h"

TwoFactorDialog::TwoFactorDialog(QObject *parent) : QObject(parent) {}

bool TwoFactorDialog::finished() {
    return !token_.isEmpty();
}
