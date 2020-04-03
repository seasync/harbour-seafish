import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // Tell SilicaFlickable the height of its content.
        contentHeight: unlogged_column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: unlogged_column

            width: page.width
            spacing: Theme.paddingLarge

            Label {
                x: Theme.horizontalPageMargin
                text: qsTr("Log into Seafile")
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeExtraLarge
            }

            Button {
                x: Theme.horizontalPageMargin * 4
                y: page.verticalCenter
                text: "Login"
                onClicked: pageStack.push(Qt.resolvedUrl("LoginDialog.qml"))
            }

        }
    }
}
