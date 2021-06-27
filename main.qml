import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Protify")
    color: "#000000"
    Material.theme: Material.Dark
    Material.accent: Material.Teal

    TextField {
        id: searchBar
        width: root.width*0.8
        height: 60
        placeholderText: qsTr("Search a processus")
        x: root.width/2-width/2
        y: root.height*0.1
        onTextChanged: {
            backend.updateSearchResults(text)
        }
    }

    ListView {
        id: searchResults
        width: root.width*0.8
        height: root.height*0.8-searchBar.height
        x: root.width/2-width/2
        anchors.top: searchBar.bottom
        anchors.topMargin: root.height*0.01
        model: backend.searchResults
        clip: true
        delegate: Button {
            id: controlSearched
            text: name
            width: searchResults.width
            highlighted: followed
            contentItem: Text {
                text: controlSearched.text
                font.capitalization: Font.MixedCase
                color: "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            onClicked: {
                console.log("process with pid "+pid+" changed")
                if (!followed) {
                    followed = true
                    highlighted = true
                    backend.addFollowedItem(pid)
                } else {
                    followed = false
                    highlighted = false
                    backend.removeFollowedItem(pid)
                }
            }
        }
    }

    Drawer {
        id: sideDrawer
        readonly property bool inPortrait: true
        y: 0
        width: root.width / 2
        height: root.height
        edge: Qt.RightEdge
        modal: inPortrait
        interactive: inPortrait
        position: inPortrait ? 0 : 1
        visible: !inPortrait
        Rectangle {
            anchors.fill: parent
            color: "#333333"
        }
        ToolButton {
            id: backDrawerButton
            width: root.width*0.1
            height: root.height*0.1
            x: parent.x
            y: parent.y
            icon.source: "qrc:/icons/right-chevron.svg"
            onClicked: {
                sideDrawer.close()
            }
        }
        ToolBar {
            id: drawerToolBarHeader
            z: 1
            property var calculatedWidth: parent.width-backDrawerButton.width
            width: calculatedWidth
            anchors.left: backDrawerButton.right
            Label {
                anchors.centerIn: parent
                text: qsTr("Followed Processes")
            }
        }
        ToolBar {
            id: configToolBarFooter
            z: 1
            property var calculatedWidth: parent.width-backDrawerButton.width
            width: calculatedWidth
            anchors.left: backDrawerButton.right
            anchors.top: followedItems.bottom
            Label {
                anchors.centerIn: parent
                text: qsTr("Hostname")
            }
        }
        TextField {
            id: hostnameField
            width: parent.width*0.8
            height: 60
            placeholderText: qsTr("Hostname")
            x: parent.width/2-width/2
            y: configToolBarFooter.y+configToolBarFooter.height+
               (parent.height-configToolBarFooter.y-
                configToolBarFooter.height)/2-height/2
            Component.onCompleted: {
                text = backend.hostname
            }

            onTextChanged: {
                backend.hostname = text
            }
        }
        ListView {
            id: followedItems
            width: parent.width
            height: parent.height-drawerToolBarHeader.height
                    -configToolBarFooter.height-hostnameField.height
                    -parent.height*0.1
            anchors.top: drawerToolBarHeader.bottom
            model: backend.followedItems
            clip: true
            delegate: Row {
                id: followedItemLayout
                states: [
                    State {
                        name: "finished"; when: !running
                        PropertyChanges {
                            target: controlFollowed
                            enabled: false
                        }
                        PropertyChanges {
                            target: deleteFollowedItemButton
                            visible: true
                        }
                        PropertyChanges {
                            target: finishedImage
                            x: rectangleFollowedItem.width*0.9-width
                        }
                    }
                ]
                transitions: [
                    Transition {
                        from: ""
                        to: "finished"
                        NumberAnimation {
                            properties: "x"
                            duration: 700
                            easing.type: Easing.OutBounce
                        }
                    }
                ]
                ToolButton {
                    id: deleteFollowedItemButton
                    width: backDrawerButton.width
                    icon.source: "qrc:/icons/delete-icon.svg"
                    visible: true
                    onClicked: {
                        backend.removeFollowedItem(pid)
                    }
                }
                Rectangle {
                    id: rectangleFollowedItem
                    x: backDrawerButton.width
                    onXChanged: {
                        x = backDrawerButton.width
                    }
                    width: sideDrawer.width-deleteFollowedItemButton.width-sideDrawer.width*0.01
                    height: 50
                    color: "#333333"
                    Button {
                        id: controlFollowed
                        text: name
                        highlighted: true
                        anchors.fill: parent
                        contentItem: Text {
                            text: controlFollowed.text
                            font.capitalization: Font.MixedCase
                            color: "#FFFFFF"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                            width: parent.width*0.9
                        }
                        onClicked: {
                            highlighted = !highlighted
                            deleteFollowedItemButton.visible = !deleteFollowedItemButton.visible
                        }
                    }
                    Image {
                        id: finishedImage
                        source: "qrc:/icons/scalable/Protify.svg"
                        sourceSize.height: parent.height*0.5
                        sourceSize.width: parent.height*0.5
                        x: parent.width
                        opacity: 0.9
                    }
                }
            }
        }
    }
    ToolButton {
        width: root.width*0.1
        height: root.height*0.1
        x: root.width-width
        y: 0
        icon.source: "qrc:/icons/open_drawer_icon.svg"
        onClicked: {
            sideDrawer.open()
        }
    }
    ToolButton {
        x: 0
        y: 0
        width: root.width*0.1
        height: root.height*0.1
        icon.source: "qrc:/icons/sync.svg"
        onClicked: {
            backend.updateSearchResults()
        }
    }
}
