#ifndef TRISTATESWITCH_H
#define TRISTATESWITCH_H

#include <QObject>
#include <QQuickItem>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuickTemplates2/private/qquickabstractbutton_p.h>
#include <QtQuickTemplates2/private/qquickswitch_p.h>

class TriStateSwitchPrivate;

class TriStateSwitch : public QQuickAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged FINAL)
    Q_PROPERTY(QPointF visualPosition READ visualPosition NOTIFY visualPositionChanged FINAL)
    Q_PROPERTY(Qt::CheckState checkState READ checkState WRITE setCheckState NOTIFY checkStateChanged FINAL)
    QML_NAMED_ELEMENT(TriStateSwitch)

public:
    explicit TriStateSwitch(QQuickItem *parent = nullptr);

    QPointF position() const;
    void setPosition(QPointF position);

    QPointF visualPosition() const;

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

Q_SIGNALS:
    void positionChanged();
    void visualPositionChanged();
    void checkStateChanged();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
#if QT_CONFIG(quicktemplates2_multitouch)
    void touchEvent(QTouchEvent *event) override;
#endif

    void mirrorChange() override;

    void nextCheckState() override;
    void buttonChange(ButtonChange change) override;

    QFont defaultFont() const override;

private:
    Q_DISABLE_COPY(TriStateSwitch)
    Q_DECLARE_PRIVATE(TriStateSwitch)
};

#endif // TRISTATESWITCH_H
