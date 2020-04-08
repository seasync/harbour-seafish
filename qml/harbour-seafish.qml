import QtQuick 2.0
import Sailfish.Silica 1.0

ApplicationWindow
{
    property bool loggedIn: false
    initialPage: Component {url: loggedIn ? "pages/MainPage.qml" : "pages/SetupPage.qml" }

    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
