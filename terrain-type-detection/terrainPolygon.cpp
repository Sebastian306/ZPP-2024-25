#include "terrainPolygon.h"

TerrainPolygon::TerrainPolygon(std::vector<GeoPoint> points) {
    this->points = points;
}
TerrainPolygon::TerrainPolygon() {
    this->points = std::vector<GeoPoint>();
}
std::vector<GeoPoint> TerrainPolygon::getPoints() const {
    return points;
}
void TerrainPolygon::setPoints(std::vector<GeoPoint> newPoints) {
    points = newPoints;
}
long double TerrainPolygon::getArea() const {
    // Shoelace Formula for Area of a Polygon
    long double area = 0;
    int n = points.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += points[i].getLon() * points[j].getLat();
        area -= points[j].getLon() * points[i].getLat();
    }
    area = std::abs(area) / 2.0;
    return area;
}
bool TerrainPolygon::isInside(GeoPoint point) const {
    int n = points.size();
    double x = point.getLon(), y = point.getLat();
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        double xi = points[i].getLon(), yi = points[i].getLat();
        double xj = points[j].getLon(), yj = points[j].getLat();
        bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);
        if (intersect) {
            inside = !inside;
        }
    }
    return inside;
}