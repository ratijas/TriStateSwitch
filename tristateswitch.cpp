#include "tristateswitch.h"

#include <QtGui/qstylehints.h>
#include <QtGui/qguiapplication.h>
#include <QtQuick/private/qquickwindow_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuickTemplates2/private/qquickabstractbutton_p_p.h>

#include <cmath>
#include <qnumeric.h>

class TriStateSwitchPrivate : public QQuickAbstractButtonPrivate
{
    Q_DECLARE_PUBLIC(TriStateSwitch)

public:
    QPointF positionAt(const QPointF &point) const;

    bool canDrag(const QPointF &movePoint) const;
    bool handleMove(const QPointF &point, ulong timestamp) override;
    bool handleRelease(const QPointF &point, ulong timestamp) override;

    QPalette defaultPalette() const override { return QQuickTheme::palette(QQuickTheme::Switch); }

    QPointF position{0.0, 0.0};

    Qt::CheckState checkState = Qt::Unchecked;
    QJSValue nextCheckState;
};

QPointF TriStateSwitchPrivate::positionAt(const QPointF &point) const
{
    Q_Q(const TriStateSwitch);
    QPointF pos = {0.0, 0.0};
    if (indicator) {
        QPointF mapped = indicator->mapFromItem(q, point);
        pos = { mapped.x() / indicator->width(), mapped.y() / indicator->height() };
    }
    if (q->isMirrored()) {
        return { 1.0 - pos.x(), pos.y() };
    }
    return pos;
}

bool TriStateSwitchPrivate::canDrag(const QPointF &movePoint) const
{
    // don't start dragging the handle unless the initial press was at the indicator,
    // or the drag has reached the indicator area. this prevents unnatural jumps when
    // dragging far outside the indicator.
    const QPointF pressPos = positionAt(pressPoint);
    const QPointF movePos = positionAt(movePoint);
    return (pressPos.x() >= 0.0 && pressPos.x() <= 1.0 && pressPos.y() >= 0.0 && pressPos.y() <= 1.0)
        || (movePos.x() >= 0.0 && movePos.x() <= 1.0 && movePos.y() >= 0.0 && movePos.y() <= 1.0);
}

bool TriStateSwitchPrivate::handleMove(const QPointF &point, ulong timestamp)
{
    Q_Q(TriStateSwitch);
    QQuickAbstractButtonPrivate::handleMove(point, timestamp);
    if (q->keepMouseGrab() || q->keepTouchGrab()) {
        q->setPosition(positionAt(point));
    }
    return true;
}

bool TriStateSwitchPrivate::handleRelease(const QPointF &point, ulong timestamp)
{
    Q_Q(TriStateSwitch);
    QQuickAbstractButtonPrivate::handleRelease(point, timestamp);
    q->setKeepMouseGrab(false);
    q->setKeepTouchGrab(false);
    return true;
}

TriStateSwitch::TriStateSwitch(QQuickItem *parent)
    : QQuickAbstractButton(*(new TriStateSwitchPrivate), parent)
{
    Q_D(TriStateSwitch);
    d->keepPressed = true;
    setCheckable(true);
}

QPointF TriStateSwitch::position() const
{
    Q_D(const TriStateSwitch);
    return d->position;
}

void TriStateSwitch::setPosition(QPointF position)
{
    Q_D(TriStateSwitch);

    if (position.y() > position.x()) {
        const qreal delta = position.y() - position.x();
        if (qFuzzyIsNull(delta)) {
            position.setY(position.x());
        } else {
            qreal diagonal = (position.x() + position.y()) / qreal(2.0);
            position = {diagonal, diagonal};
        }
    }

    position = { std::clamp(position.x(), qreal(0.0), qreal(1.0)), std::clamp(position.y(), qreal(0.0), qreal(1.0)) };
    if (qFuzzyCompare(d->position, position)) {
        return;
    }

    d->position = position;
    Q_EMIT positionChanged();
    Q_EMIT visualPositionChanged();
}

QPointF TriStateSwitch::visualPosition() const
{
    Q_D(const TriStateSwitch);
    if (isMirrored()) {
        return { 1.0 - d->position.x(), d->position.y() };
    }
    return d->position;
}

void TriStateSwitch::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(TriStateSwitch);
    if (!keepMouseGrab()) {
        const QPointF movePoint = event->position();
        if (d->canDrag(movePoint)) {
            setKeepMouseGrab(QQuickDeliveryAgentPrivate::dragOverThreshold(movePoint.x() - d->pressPoint.x(), Qt::XAxis, event)
                || QQuickDeliveryAgentPrivate::dragOverThreshold(movePoint.y() - d->pressPoint.y(), Qt::YAxis, event));
        }
    }
    QQuickAbstractButton::mouseMoveEvent(event);
}

#if QT_CONFIG(quicktemplates2_multitouch)
void TriStateSwitch::touchEvent(QTouchEvent *event)
{
    Q_D(TriStateSwitch);
    if (!keepTouchGrab() && event->type() == QEvent::TouchUpdate) {
        for (const QTouchEvent::TouchPoint &point : event->points()) {
            if (point.id() != d->touchId || point.state() != QEventPoint::Updated) {
                continue;
            }
            if (d->canDrag(point.position())) {
                setKeepTouchGrab(QQuickDeliveryAgentPrivate::dragOverThreshold(point.position().x() - d->pressPoint.x(), Qt::XAxis, point)
                    || QQuickDeliveryAgentPrivate::dragOverThreshold(point.position().y() - d->pressPoint.y(), Qt::YAxis, point));
            }
        }
    }
    QQuickAbstractButton::touchEvent(event);
}
#endif

void TriStateSwitch::mirrorChange()
{
    QQuickAbstractButton::mirrorChange();
    Q_EMIT visualPositionChanged();
}

static QPointF checkStateToPosition(Qt::CheckState checkState)
{
    switch (checkState) {
    case Qt::CheckState::Unchecked:
    default:
        return { 0.0, 0.0 };
    case Qt::CheckState::PartiallyChecked:
        return { 1.0, 0.0 };
    case Qt::CheckState::Checked:
        return { 1.0, 1.0 };
    }
}

static constexpr const QPointF POSITION_UNCHECKED{0.0, 0.0};
static constexpr const QPointF POSITION_PARTIALLY_CHECKED{1.0, 0.0};
static constexpr const QPointF POSITION_CHECKED{1.0, 1.0};

static qreal vectorLength(QPointF vector)
{
    return std::sqrt(vector.x() * vector.x() + vector.y() * vector.y());
}

static std::tuple<Qt::CheckState, QPointF> positionToCheckState(QPointF position)
{
    const qreal distanceUnchecked = vectorLength(position - POSITION_UNCHECKED);
    const qreal distancePartiallyChecked = vectorLength(position - POSITION_PARTIALLY_CHECKED);
    const qreal distanceChecked = vectorLength(position - POSITION_CHECKED);

    if (distanceUnchecked <= distancePartiallyChecked && distanceUnchecked <= distanceChecked) {
        return {Qt::CheckState::Unchecked, POSITION_UNCHECKED};
    } else if (distancePartiallyChecked <= distanceChecked) {
        return {Qt::CheckState::PartiallyChecked, POSITION_PARTIALLY_CHECKED};
    } else {
        return {Qt::CheckState::Checked, POSITION_CHECKED};
    }
}

Qt::CheckState TriStateSwitch::checkState() const
{
    Q_D(const TriStateSwitch);
    return d->checkState;
}

void TriStateSwitch::setCheckState(Qt::CheckState state)
{
    Q_D(TriStateSwitch);
    if (d->checkState == state) {
        return;
    }

    bool wasChecked = isChecked();
    d->checked = state == Qt::Checked;
    d->checkState = state;
    Q_EMIT checkStateChanged();
    if (d->checked != wasChecked) {
        Q_EMIT checkedChanged();
    }
    // the checked state might not change => force a position update to
    // avoid that the handle is left somewhere in the middle (QTBUG-57944)
    setPosition(checkStateToPosition(d->checkState));
}

QJSValue TriStateSwitch::getNextCheckState() const
{
    Q_D(const TriStateSwitch);
    return d->nextCheckState;
}

void TriStateSwitch::setNextCheckState(const QJSValue &callback)
{
    Q_D(TriStateSwitch);
    d->nextCheckState = callback;
    Q_EMIT nextCheckStateChanged();
}

void TriStateSwitch::nextCheckState()
{
    Q_D(TriStateSwitch);

    if (keepMouseGrab() || keepTouchGrab()) {
        const auto [ checkState, position ] = positionToCheckState(d->position);
        setCheckState(checkState);
        // the checked state might not change => force a position update to
        // avoid that the handle is left somewhere in the middle (QTBUG-57944)
        setPosition(position);
    } else if (d->nextCheckState.isCallable()) {
        setCheckState(static_cast<Qt::CheckState>(d->nextCheckState.call().toInt()));
    } else {
        setCheckState(static_cast<Qt::CheckState>((d->checkState + 1) % 3));
    }
}

void TriStateSwitch::buttonChange(ButtonChange change)
{
    Q_D(TriStateSwitch);
    if (change == ButtonCheckedChange) {
        setCheckState(isChecked() ? Qt::Checked : Qt::Unchecked);
    } else {
        QQuickAbstractButton::buttonChange(change);
    }
}

QFont TriStateSwitch::defaultFont() const
{
    return QQuickTheme::font(QQuickTheme::Switch);
}

#include "moc_tristateswitch.cpp"
