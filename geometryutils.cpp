#include "geometryutils.h"

#include <algorithm>
#include <optional>
#include <random>

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

QList<QPointF> GeometryUtils::randomUnitTriangle()
{
    constexpr const unsigned int NUM_VERTICES = 3;

    std::uniform_real_distribution<qreal> unitDist(0.0, 1.0);
    std::uniform_int_distribution boolDist{0, 1};
    std::uniform_int_distribution<unsigned int> edgeDist{0, NUM_VERTICES - 1};
    std::uniform_int_distribution<unsigned int> rotationDist{0, 3};
    std::random_device rd;
    std::default_random_engine rng(rd());

    auto getUnit = [&]() -> qreal { return unitDist(rng); };
    auto getBool = [&]() -> bool { return boolDist(rng) != 0; };
    auto getEdge = [&]() -> unsigned int { return edgeDist(rng); };
    auto get2Edges = [&]() -> std::array<unsigned int, 2> {
        // pick any two out of three == pick one to discard
        std::array<unsigned int, 2> edges;
        const auto ignore = getEdge();
        for (int e = 0, v = 0; e < 2 && v < NUM_VERTICES; v += 1) {
            if (v != ignore) {
                edges[e] = v;
                e += 1;
            }
        }
        if (getBool()) {
            std::swap(edges[0], edges[1]);
        }
        return edges;
    };
    auto getRotations = [&]() -> unsigned int { return rotationDist(rng); };
    auto rotate90 = [](QPointF point) -> QPointF {
        // step 1: translate unit point to origin 0.0 (with extents -0.5..0.5)
        point -= QPointF(0.5, 0.5);
        // step 2: rotate 90deg CV
        point = {-point.y(), point.x()};
        // step 3: translate back to unit square
        point += QPointF(0.5, 0.5);
        return point;
    };

    QList<QPointF> vertices{NUM_VERTICES};

    // Random shape:
    // 0. Two on the same edge, third needs to be on the opposite edge.
    //    Then pick any two, and make sure they are on the opposite perpendicular edges too.
    // 1. All vertices are on three random but different edges.
    //    Then pick one of the edges perpendicular to the empty one, and move its point toward the empty edge.

    const auto shapeStrategy = getBool();
    if (shapeStrategy) {
        // two on the same (left) edge, i.e. x=0, y=rand
        vertices[0] = {0.0, getUnit()};
        vertices[1] = {0.0, getUnit()};
        // third is on the right edge
        vertices[2] = {1.0, getUnit()};

        // split vertically
        const auto [edgeTop, edgeBottom] = get2Edges();
        vertices[edgeTop].setY(0.0);
        vertices[edgeBottom].setY(1.0);
    } else {
        // three different edges
        vertices[0] = {0.0, getUnit()}; // left
        vertices[1] = {getUnit(), 0.0}; // top
        vertices[2] = {1.0, getUnit()}; // right
        // pick either left or right
        const auto vertex = getBool() ? 0 : 2;
        // move it toward bottom edge
        vertices[vertex].setY(1.0);
    }

    // random rotation: number of times vertices need to be rotates 90 degrees
    const auto rotations = getRotations();
    for (int r = 0; r < rotations; r++) {
        for (QPointF &point : vertices) {
            point = rotate90(point);
        }
    }

    // random swap vertices
    std::shuffle(vertices.begin(), vertices.end(), rng);
    return vertices;
}

#include "moc_geometryutils.cpp"
