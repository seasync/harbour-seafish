# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-seafish

CONFIG += sailfishapp
CONFIG += c++14

SOURCES += src/harbour-seafish.cpp \
    src/LoginDialog.cpp \
    src/TwoFactorDialog.cpp

DISTFILES += qml/harbour-seafish.qml \
    qml/cover/CoverPage.qml \
    qml/pages/LoginDialog.qml \
    qml/pages/SetupPage.qml \
    rpm/harbour-seafish.changes.in \
    rpm/harbour-seafish.changes.run.in \
    rpm/harbour-seafish.spec \
    rpm/harbour-seafish.yaml \
    translations/*.ts \
    harbour-seafish.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-seafish-de.ts

HEADERS += \
    src/LoginDialog.h \
    src/TwoFactorDialog.h
