#include "geometryutils.h"

GeometryUtils::GeometryUtils(QObject *parent)
    : QObject{parent}
{
}

qreal GeometryUtils::lerp(qreal from, qreal to, qreal t)
{
    return from + (to - from) * t;
}

static qreal dot(const QPointF a, const QPointF b)
{
    return a.x() * b.x() + a.y() * b.y();
}

static qreal magnitude2(const QPointF point)
{
    return point.x() * point.x() + point.y() * point.y();
}

// find a projection of `point` onto a line between points `a` and `b`.
static QPointF projection(QPointF a, QPointF b, QPointF point)
{
    // step 1: translate into {0,0} origin
    b -= a;
    point -= a;

    // step 2: find a projection (but don't multiply it by the `end` vector)
    QPointF proj = dot(point, b) / magnitude2(b) * b;

    // step 3: restore translation
    return proj + a;
}

qreal GeometryUtils::linearPosition(QPointF start, QPointF end, QPointF position)
{
    // step 1: translate into {0,0} origin
    end -= start;
    position -= start;
    start = {0.0, 0.0};

    // step 2: find a projection (but don't multiply it by the `end` vector)
    return std::clamp(dot(position, end) / magnitude2(end), qreal(0.0), qreal(1.0));
}

QPointF GeometryUtils::planarPosition(QPointF start, QPointF end, QPointF zero, QPointF position)
{
    const qreal directPosition = linearPosition(start, end, position);
    const qreal directPositionClamped = std::clamp(directPosition, 0.0, 1.0);

    const QPointF zeroProj = projection(start, end, zero);
    const qreal perpendicularPosition = linearPosition(zero, zeroProj, position);
    const qreal perpendicularPositionClamped = std::clamp(perpendicularPosition, 0.0, 1.0);

    return QPointF(directPositionClamped, perpendicularPositionClamped);
}

#include "moc_geometryutils.cpp"
