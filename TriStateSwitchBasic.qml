import QtQuick
import QtQuick.Templates as T
import TriStateSwitchQt

TriStateSwitch {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding,
                            implicitIndicatorWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    baselineOffset: contentItem.y + contentItem.baselineOffset
    hoverEnabled: true

    spacing: 16

    indicator: TriStateSwitchIndicatorBasic {
        x: (control.text || control.icon.name || control.icon.source)
            ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding)
            : control.leftPadding + Math.round((control.availableWidth - width) / 2)
        y: control.topPadding + Math.round((control.availableHeight - height) / 2)

        control: control
    }

    contentItem: Text {
        id: label

        readonly property int effectiveIndicatorWidth: control.indicator && control.indicator.visible && control.indicator.width > 0
            ? control.indicator.width + control.spacing : 0

        leftPadding: !control.mirrored ? effectiveIndicatorWidth : 0
        rightPadding: control.mirrored ? effectiveIndicatorWidth : 0

        verticalAlignment: Text.AlignVCenter
        text: control.text
        visible: text.length > 0 && control.display !== T.AbstractButton.IconOnly
        font: control.font
        color: control.palette.text
        linkColor: control.palette.link
        elide: Text.ElideRight
    }
}
