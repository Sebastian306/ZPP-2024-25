#ifndef TERRAINPOLYGON_H
#define TERRAINPOLYGON_H

#include <vector>
#include <cmath>
#include "../common.h"

class TerrainPolygon
{
    private:
        std::vector<GeoPoint> points;
    public:
        std::vector<GeoPoint> getPoints() const;
        void setPoints(std::vector<GeoPoint> newPoints);
        TerrainPolygon(std::vector<GeoPoint> points);
        TerrainPolygon();
        long double getArea() const;
        bool isInside(GeoPoint point) const;
};

#endif // TERRAINPOLYGON_H