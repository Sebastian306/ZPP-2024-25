#ifndef COMMON_H
#define COMMON_H

#define TERRAIN_TYPE_FOLDER_PATH "terrain-type-detection/"
#define TILES_FOLDER_PATH "terrain-type-detection/tiles/"
#define TILES_PROPERTIES_FILE_NAME "tiles.properties"
#define TILES_FILE_NAME_TEMPLATE "tile_{:d}_{:d}.txt" // {} will be replaced by x and y


class GeoPoint {
    private:
        double lat;
        double lon;
    public:
        GeoPoint(double latitude, double longitude);
        double getLat() const;
        double getLon() const;
        void setLat(double latitude);
        void setLon(double longitude);
        bool operator==(const GeoPoint &other) const;
};

#endif // COMMON_H