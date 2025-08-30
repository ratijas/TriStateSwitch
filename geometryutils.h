#ifndef GEOMETRYUTILS_H
#define GEOMETRYUTILS_H

#include <QObject>
#include <QQmlEngine>
#include <QPoint>

class GeometryUtils : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit GeometryUtils(QObject *parent = nullptr);

    Q_INVOKABLE static qreal lerp(qreal from, qreal to, qreal t);

    // Calculate a transition between two given points, mapping position onto a nearest perpendicular point on a line between start and end points.
    // Returns a number clamped into [0; 1] range.
    Q_INVOKABLE static qreal linearPosition(QPointF start, QPointF end, QPointF position);

    // Calculate a transition between three given points in normalized coordinates.
    // Returns a Point where both coordinates are clamped into [0; 1] range:
    //  - x corresponds to the position between the start and end points,
    //  - y corresponds to the position on a perpendicular between the third (zero) point and a line between start and end points.
    Q_INVOKABLE static QPointF planarPosition(QPointF start, QPointF end, QPointF zero, QPointF position);
};

#endif // GEOMETRYUTILS_H
