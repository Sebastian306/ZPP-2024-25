#include "common.h"

GeoPoint::GeoPoint(double latitude, double longitude) {
    lat = latitude;
    lon = longitude;
}
double GeoPoint::getLat() const {
    return lat;
}
double GeoPoint::getLon() const {
    return lon;
}
void GeoPoint::setLat(double latitude) {
    lat = latitude;
}
void GeoPoint::setLon(double longitude) {
    lon = longitude;
}
bool GeoPoint::operator==(const GeoPoint &other) const {
    return lat == other.getLat() && lon == other.getLon();
}