#ifndef terrainChecker_H
#define terrainChecker_H

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <format>
#include "../common.h"
#include "terrainPolygon.h"

class TerrainChecker
{
    private:
        std::map<std::tuple<int, int>, std::vector<TerrainPolygon>> terrainMap;
        long double tileSizeX;
        long double tileSizeY;
        int tilesCountX;
        int tilesCountY;
        long double shiftX;
        long double shiftY;

        void readTile(int x, int y);
    public:
        TerrainChecker();
        std::string getTileFileName(int x, int y);
        std::pair<int, int> getTileForPoint(GeoPoint point) const;
        std::vector<TerrainPolygon> getPolygonsForTile(int x, int y);
        std::vector<TerrainPolygon> getPolygonsForPoint(GeoPoint point);
        bool isOnLand(GeoPoint point);
};

#endif // terrainChecker_H