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
        x: 50
        y: 20
        text: "Toggle the switch"
        tristate: true
        checkState: triState.checkState
        onCheckStateChanged: triState.checked = checked
        nextCheckState: () => checkState === Qt.Checked ? Qt.Unchecked : Qt.Checked
    }

    TriStateSwitchBasic {
        id: triState
        x: 50
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
    }

    TriStateSwitchBasic {
        id: triState1
        x: 125
        y: 155
        checkState: Qt.Checked
        text: "Child Tri State Switch 1"
    }

    TriStateSwitchBasic {
        id: triState2
        x: 125
        y: 230
        checkState: Qt.Checked
        nextCheckState: () => root.prevCheckState(checkState)
        text: "Child Tri State Switch 2\nnextCheckState: prevCheckState"
    }

    Line {
        id: line

        anchors.top: triState.bottom
        anchors.left: triState.left
        anchors.bottom: triState2.top
        anchors.topMargin: -8
        anchors.leftMargin: 16
        anchors.bottomMargin: -16
    }

    Line {
        anchors {
            top: triState1.top
            topMargin: 16
            left: line.left
        }
        width: 48
    }

    Line {
        anchors {
            top: triState2.top
            topMargin: 16
            left: line.left
        }
        width: 48
    }

    component Line : Rectangle {
        implicitWidth: 2
        implicitHeight: 2
        color: "#666"
    }
}
