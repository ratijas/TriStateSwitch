import QtQuick
import QtQuick.Effects
import QtQuick.Shapes
import TriStateSwitchQt

Item {
    id: root

    required property TriStateSwitch control

    readonly property point position: Qt.point(positionX, positionY)
    property real positionX: control.position.x
    property real positionY: control.position.y

    NumberBehavior on positionX {}
    NumberBehavior on positionY {}

    readonly property point positionUnchecked: root.control.corners[0]
    readonly property point positionPartiallyChecked: root.control.corners[1]
    readonly property point positionChecked: root.control.corners[2]

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
        id: shape

        x: root.knobSize / 2
        y: root.knobSize / 2
        width: root.width - root.knobSize
        height: root.height - root.knobSize

        ShapePath {
            strokeColor: "#e8e8e8"
            strokeWidth: root.knobSize
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin

            startX: 0
            startY: 0

            PathSvg {
                path: `
                    M ${root.positionUnchecked.x * shape.width}
                      ${root.positionUnchecked.y * shape.height}
                    L ${root.positionPartiallyChecked.x * shape.width}
                      ${root.positionPartiallyChecked.y * shape.height}
                    L ${root.positionChecked.x * shape.width}
                      ${root.positionChecked.y * shape.height}
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

    // knob
    Item {
        width: root.knobSize
        height: root.knobSize

        // These expressions don't really make the knob 'stick' to the cursor :(
        // But rewriting it in the way that makes it 'stick' breaks
        // - either dragging from the edges outside indicator,
        // - or snapping around the vertices because of the changes to the scale of the triangle.
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
