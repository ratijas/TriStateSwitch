import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Basic

Window {
    id: root

    width: 400
    height: 300
    visible: true
    title: qsTr("Tri State Switch")

    function prevCheckState(checkState: int): int {
        switch (checkState) {
        default:
        case Qt.Unchecked:
            return Qt.Checked;
        case Qt.PartiallyChecked:
            return Qt.Unchecked;
        case Qt.Checked:
            return Qt.PartiallyChecked;
        }
    }

    CheckBox {
        x: 30
        y: 20
        text: "Toggle the switch"
        tristate: true
        checkState: triState.checkState
        onCheckStateChanged: triState.checked = checked
        nextCheckState: () => checkState === Qt.Checked ? Qt.Unchecked : Qt.Checked
    }

    Button {
        id: randomizeButton
        x: 270
        y: 20
        text: "Randomize"
        onClicked: {
            for (const triStateSwitch of [triState, triState1, triState2]) {
                triStateSwitch.corners = GeometryUtils.randomUnitTriangle();
            }
        }
    }

    TriStateSwitchBasic {
        id: triState
        x: 30
        y: 80
        checkState: {
            if (triState1.checkState === Qt.Checked && triState2.checkState === Qt.Checked) {
                return Qt.Checked;
            } else if (triState1.checkState === Qt.Unchecked && triState2.checkState === Qt.Unchecked) {
                return Qt.Unchecked;
            } else {
                return Qt.PartiallyChecked;
            }
        }

        onCheckStateChanged: {
            switch (checkState) {
            case Qt.Checked:
                triState1.checkState = Qt.Checked;
                triState2.checkState = Qt.Checked;
                break;
            case Qt.Unchecked:
                triState1.checkState = Qt.Unchecked;
                triState2.checkState = Qt.Unchecked;
                break;
            default:
                break;
            }
        }

        text: "Tri State Switch!"
        corners: [Qt.point(0, 0.5), Qt.point(1, 0), Qt.point(1, 1)]
        // corners: [Qt.point(0, 0.0), Qt.point(1, 0), Qt.point(1, 1)]
    }

    TriStateSwitchBasic {
        id: triState1
        x: 100
        y: 155
        checkState: Qt.Checked
        text: "Child Tri State Switch 1"
        corners: [Qt.point(0, 1), Qt.point(0.7, 0), Qt.point(1, 1)]
    }

    TriStateSwitchBasic {
        id: triState2
        x: 100
        y: 230
        checkState: Qt.Checked
        nextCheckState: () => root.prevCheckState(checkState)
        text: "Child Tri State Switch 2\nnextCheckState: prevCheckState"
        corners: [Qt.point(0, 0), Qt.point(1, 0.3), Qt.point(0.6, 1)]
    }

    Line {
        id: line

        anchors.top: triState.bottom
        anchors.left: triState.left
        anchors.bottom: triState2.top
        anchors.topMargin: -8
        anchors.leftMargin: 16
        anchors.bottomMargin: -16
        z: -1
    }

    Line {
        anchors {
            top: triState1.top
            topMargin: 16
            left: line.left
        }
        width: 48
        z: -1
    }

    Line {
        anchors {
            bottom: line.bottom
            left: line.left
        }
        width: 48
        z: -1
    }

    component Line : Rectangle {
        implicitWidth: 2
        implicitHeight: 2
        color: "#666"
        radius: Math.min(width, height) / 2
    }

    // about
    Label {
        anchors {
            horizontalCenter: randomizeButton.horizontalCenter
            top: randomizeButton.bottom
            topMargin: 24
        }
        horizontalAlignment: Text.AlignHCenter
        textFormat: Text.StyledText
        text: '<a href="https://github.com/ratijas/TriStateSwitch">GitHub</a><br/>© Ratijas'
        onLinkActivated: link => Qt.openUrlExternally(link)

        ToolTip.visible: ToolTip.text !== ""
        ToolTip.text: hoveredLink
    }
}
