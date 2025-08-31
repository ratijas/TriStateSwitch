#include "geometryutils.h"

#include <optional>

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

namespace
{

struct SnapCandidate
{
    QVector2D target;
    float distance;

    void min(const SnapCandidate newCandidate)
    {
        if (newCandidate.distance < distance) {
            *this = newCandidate;
        }
    }

    void min(const std::optional<SnapCandidate> maybeNewCandidate)
    {
        if (maybeNewCandidate.has_value() && maybeNewCandidate->distance < distance) {
            *this = *maybeNewCandidate;
        }
    }
};

SnapCandidate makeVertexCandidate(const QVector2D vertex, const QVector2D position)
{
    return SnapCandidate {
        .target = vertex,
        .distance = (vertex - position).length(),
    };
}

std::optional<SnapCandidate> makeProjectionCandidate(const QVector2D base, const QVector2D position)
{
    const QVector2D proj = QVector2D::dotProduct(position, base) / base.lengthSquared() * base;
    const bool sameDirectionX = base.x() < 0.0 == proj.x() < 0.0;
    const bool sameDirectionY = base.y() < 0.0 == proj.y() < 0.0;
    // qDebug() << "PROJ" << sameDirectionX << sameDirectionY << "BASE" << base << position << "PROJ" << proj;
    if (sameDirectionX && sameDirectionY && proj.lengthSquared() <= base.lengthSquared()) {
        return std::make_optional(SnapCandidate {
            .target = proj,
            .distance = (proj - position).length(),
        });
    }
    return {};
}

std::optional<SnapCandidate> makeProjectionCandidate(const QVector2D lineA, const QVector2D lineB, const QVector2D position)
{
    std::optional<SnapCandidate> maybeCandidate = makeProjectionCandidate(lineB - lineA, position - lineA);
    if (maybeCandidate.has_value()) {
        const auto c = *maybeCandidate;
        qDebug() << qSetRealNumberPrecision(2) << qSetFieldWidth(2) << "PROJ" << lineA << lineB << position << "RESULT" << c.target << c.distance;
    } else {
        qDebug() << qSetRealNumberPrecision(2) << qSetFieldWidth(2) << "PROJ" << lineA << lineB << position << "NONE";
    }
    if (maybeCandidate.has_value()) {
        maybeCandidate->target += lineA;
    }
    return maybeCandidate;
}

float sign(QVector2D position, QVector2D lineA, QVector2D lineB)
{
    return (position.x() - lineB.x()) * (lineA.y() - lineB.y()) - (lineA.x() - lineB.x()) * (position.y() - lineB.y());
}

bool isPositionInsideTriangle(QVector2D vertexA, QVector2D vertexB, QVector2D vertexC, QVector2D position)
{
    const float sign1 = sign(position, vertexA, vertexB);
    const float sign2 = sign(position, vertexB, vertexC);
    const float sign3 = sign(position, vertexC, vertexA);

    const bool allNeg = (sign1 < 0.0) || (sign2 < 0.0) || (sign3 < 0.0);
    const bool allPos = (sign1 > 0.0) || (sign2 > 0.0) || (sign3 > 0.0);

    // qDebug() << "ALL NEG:" << allNeg << "ALL POS:" << allPos;

    return !(allNeg && allPos);
}

};

QPointF GeometryUtils::snapPointToTriangle(QPointF vertexA, QPointF vertexB, QPointF vertexC, QPointF position)
{
    return snapVectorToTriangle(QVector2D(vertexA), QVector2D(vertexB), QVector2D(vertexC), QVector2D(position)).toPointF();
}

QVector2D GeometryUtils::snapVectorToTriangle(QVector2D vertexA, QVector2D vertexB, QVector2D vertexC, QVector2D position)
{
    if (isPositionInsideTriangle(vertexA, vertexB, vertexC, position)) {
        // the point is inside, no further actions needed
        return position;
    }

    SnapCandidate bestSnapCandidate = makeVertexCandidate(vertexA, position);
    bestSnapCandidate.min(makeVertexCandidate(vertexB, position));
    bestSnapCandidate.min(makeVertexCandidate(vertexC, position));
    bestSnapCandidate.min(makeProjectionCandidate(vertexA, vertexB, position));
    bestSnapCandidate.min(makeProjectionCandidate(vertexB, vertexC, position));
    bestSnapCandidate.min(makeProjectionCandidate(vertexC, vertexA, position));
    return bestSnapCandidate.target;
}

#include "moc_geometryutils.cpp"
