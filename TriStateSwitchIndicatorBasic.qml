import QtQuick
import QtQuick.Effects
import QtQuick.Shapes
import TriStateSwitchQt

Item {
    id: root

    required property TriStateSwitch control

    property real positionX: control.position.x
    property real positionY: control.position.y
    readonly property point position: Qt.point(positionX, positionY)

    readonly property point positionUnchecked: Qt.point(0, 0)
    readonly property point positionPartiallyChecked: Qt.point(1, 0)
    readonly property point positionChecked: Qt.point(1, 1)

    // transitions in normalized coordinates:
    // x corresponds to the position between the specified states,
    // y corresponds to the perpendicular position between the third state and a line between these two.
    readonly property point positionUncheckedToChecked: GeometryUtils.planarPosition(positionUnchecked, positionChecked, positionPartiallyChecked, position)
    readonly property point positionUncheckedToPartiallyChecked: GeometryUtils.planarPosition(positionUnchecked, positionPartiallyChecked, positionChecked, position)
    readonly property point positionPartiallyCheckedToChecked: GeometryUtils.planarPosition(positionPartiallyChecked, positionChecked, positionUnchecked, position)

    readonly property real positionOfChecked: 1 - positionUncheckedToPartiallyChecked.y
    readonly property real positionOfPartiallyChecked: 1 - positionUncheckedToChecked.y
    readonly property real positionOfUnchecked: 1 - positionPartiallyCheckedToChecked.y

    readonly property real knobSize: 28

    implicitWidth: 70
    implicitHeight: 50

    // outline
    Shape {
        id: outline

        x: root.knobSize / 2
        y: root.knobSize / 2
        width: parent.width - root.knobSize
        height: parent.height - root.knobSize

        ShapePath {
            strokeColor: "#e8e8e8"
            strokeWidth: root.knobSize
            // fillColor: "#e8e8e8"
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin

            startX: root.positionUnchecked.x * (root.width - root.knobSize)
            startY: root.positionUnchecked.y * (root.height - root.knobSize)

            PathSvg {
                path: `L ${root.positionPartiallyChecked.x * (root.width - root.knobSize)}
                      ${root.positionPartiallyChecked.y * (root.height - root.knobSize)}
                    L ${root.positionChecked.x * (root.width - root.knobSize)}
                      ${root.positionChecked.y * (root.height - root.knobSize)}
                    z
                `
                // unbreak indentation in QtCreator
                objectName: ""
            }
        }

        layer.enabled: true
        layer.samples: 2
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowBlur: 0.5
            shadowColor: "black"
        }
    }

    // Text {
    //     z: 999
    //     y: 0
    //     x: 200
    //     // anchors.centerIn: parent
    //     text: `checked: ${root.positionOfChecked}\npartial:${root.positionOfPartiallyChecked}\nuncheck:${positionOfUnchecked}`
    //     color: "#833"
    //     font.pointSize: 14
    //     font.family: "monospace"
    // }

    NumberBehavior on positionX {}
    NumberBehavior on positionY {}

    // knob
    Item {
        width: root.knobSize
        height: root.knobSize

        x: root.control.visualPosition.x * (root.width - root.knobSize)
        y: root.control.visualPosition.y * (root.height - root.knobSize)

        NumberBehavior on x {}
        NumberBehavior on y {}

        Rectangle {
            anchors.fill: parent
            anchors.margins: 2
            // that makes it 24x24

            color: "#EEE"
            border.color: control.activeFocus ? root.control.palette.highlight : "#33333380"
            border.width: 1
            radius: width / 2

            // icon canvas
            Item {
                // that makes it 16x16
                anchors.fill: parent
                anchors.margins: 4

                // horizontal line for Unchecked minus sign & Checked plus sign,
                // collapses into a middle dot for PartiallyChecked (...)
                Line {
                    anchors.centerIn: parent
                    anchors.alignWhenCentered: false
                    width: GeometryUtils.lerp(parent.width, implicitWidth, root.positionOfPartiallyChecked)
                }

                // vertical line for Checked plus sign
                // collapses into a middle dot for Unchecked & PartiallyChecked (...)
                Line {
                    anchors.centerIn: parent
                    anchors.alignWhenCentered: false
                    height: GeometryUtils.lerp(implicitHeight, parent.height, root.positionOfChecked)
                }

                // left dot for PartiallyChecked (...)
                Line {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: root.positionOfPartiallyChecked
                }

                // right dot for PartiallyChecked (...)
                Line {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: root.positionOfPartiallyChecked
                }
            }
        }
    }

    component Line : Rectangle {
        implicitWidth: 4
        implicitHeight: 4
        color: "#666"
        radius: Math.min(width, height)
    }

    component NumberBehavior : Behavior {
        enabled: !root.control.pressed
        NumberAnimation {
            duration: 500
            easing.type: Easing.OutCubic
        }
    }
}
