import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height + Theme.paddingLarge

        VerticalScrollDecorator {}

        Column {
            id: column
            anchors { left: parent.left; right: parent.right }
            spacing: Theme.paddingLarge

            PageHeader { title: "Login into Seafile" }

            TextField {
                id: sfinstance
                anchors { left: parent.left; right: parent.right }
                focus: true; label: "Seafile Instance"; placeholderText: label
                EnterKey.enabled: url || inputMethodComposing
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: email.focus = true
            }

            TextField {
                id: email
                anchors { left: parent.left; right: parent.right }
                label: "Email address"; placeholderText: label
                EnterKey.enabled: text || inputMethodComposing
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: password.focus = true
            }

            TextField {
                id: password
                anchors { left: parent.left; right: parent.right }
                echoMode: TextInput.Password
                label: "Password"; placeholderText: label
                EnterKey.enabled: text || inputMethodComposing
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: clientname.focus = true
            }

            TextSwitch {
                id: autlogin
                text: "Automatic login on startup"
            }

            TextField {
                id: clientname
                anchors { left: parent.left; right: parent.right }
                label: "Client name"; placeholderText: label
                EnterKey.enabled: text || inputMethodComposing
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: pageStack.animatorPush(login_tryPage)
            }
        }
    }

    Component {
        id: login_tryPage

        Connections {
            target: login_tryPage
            onToken_needed:
                PageStack.push(twoFAPage)

        }

        Page {
            backNavigation: false

            Column {
                anchors { left: parent.left; right: parent.right }
                spacing: Theme.paddingLarge

                PageHeader { title: "Account Created" }

                Label {
                    anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    text: "User Information"
                }

                Label {
                    anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    text: sfinstance.text
                }

                Label {
                    anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    text: email.text
                }

                Label {
                    anchors { left: parent.left; right: parent.right }
                    horizontalAlignment: Text.AlignHCenter
                    text: clientname.text
                }

                BusyIndicator {
                         size: BusyIndicatorSize.small
                         anchors.centerIn: parent
                         running: true
                }

                login_tryPage.onStatusChanged:  {
                         if (status === PageStatus.Activating) {
                         console.log("LoginRequest from qml")
                         LoginDialog.loginCall(sfinstance.url, email.text, password.text, clientname.text, autlogin.checked)
                         }
                }

            }
        }
    }

    Component {
        id:twoFA_dialog

        Dialog {
             property string name

             Column {
                 width: parent.width

                 DialogHeader { title: "Second factor autenthication"}

                 TextField {
                     id: tokenField
                     width: parent.width
                     placeholderText: "2FA token string"
                     label: "2FA Token input"
                 }
             }

             onDone: {
                 if (result == DialogResult.Accepted) {
                     TwoFactorDialog.twoFAToken(tokenField.text)
                     PageStack.pop()
                 }
             }
         }
    }
}
