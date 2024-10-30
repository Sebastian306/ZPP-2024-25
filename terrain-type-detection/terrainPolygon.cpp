#include "terrainPolygon.h"
#include <iostream>

TerrainPolygon::TerrainPolygon(std::vector<GeoPoint> points) {
    this->points = points;
}
TerrainPolygon::TerrainPolygon(std::vector<std::pair<double, double>> points) {
    this->points.clear();
    for (auto point : points) {
        this->points.push_back(GeoPoint(point.first, point.second));
    }
}
TerrainPolygon::TerrainPolygon() {
    points.clear();
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

bool TerrainPolygon::isInside(double lat, double lon) const {
    bool inside = false;
    int n = points.size();
    
    // Loop over each edge in the polygon
    for (int i = 0, j = n - 1; i < n; j = i++) {
        double xi = points[i].getLon(), yi = points[i].getLat(); // longitude, latitude for point i
        double xj = points[j].getLon(), yj = points[j].getLat(); // longitude, latitude for point j
        
        // Check if the edge intersects the horizontal line at "lon"
        bool intersect = ((yi > lat) != (yj > lat)) &&
                         (lon < (xj - xi) * (lat - yi) / (yj - yi) + xi);
        
        // Toggle the "inside" state if there's an intersection
        if (intersect) {
            inside = !inside;
        }
    }
    return inside;
}


bool TerrainPolygon::isInside(GeoPoint point) const {
    return isInside(point.getLat(), point.getLon());
}

std::string TerrainPolygon::toString() const {
    std::string result = "Polygon[";
    for (auto point : points) {
        result += "(" + std::to_string(point.getLat()) + ", " + std::to_string(point.getLon()) + "), ";
    }
    result += "]";
    return result;
}