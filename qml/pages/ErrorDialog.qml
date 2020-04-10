import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    backNavigation: false

    property string errorType
    property string errorMessage


    DialogHeader { title: "ERROR" }

    Label {
        text: errorType
        color: "red"
        anchors.horizontalCenter: parent.horizontalCenter
        font: Theme.fontFamilyHeading
    }

    TextField {
        text: errorMessage
        color: Theme.highlightColor
        font.family: Theme.fontFamily
    }
}
