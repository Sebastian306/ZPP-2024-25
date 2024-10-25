#ifndef terrainChecker_H
#define terrainChecker_H

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include "../common.h"
#include "terrainPolygon.h"

class TerrainChecker
{
    private:
        std::map<std::tuple<int, int>, std::vector<TerrainPolygon>> terrainMap;
        std::string tilesFolderPath;
        std::string tilesPropertiesFileName;
        std::string tileFileNameTemplate;
        long double tileSizeX;
        long double tileSizeY;
        int tilesCountX;
        int tilesCountY;
        long double shiftX;
        long double shiftY;
    public:
        TerrainChecker(
            std::string tilesFolderPath = TILES_FOLDER_PATH, 
            std::string tilesPropertiesFileName = TILES_PROPERTIES_FILE_NAME,
            std::string tileFileNameTemplate = TILES_FILE_NAME_TEMPLATE
            );
        std::vector<TerrainPolygon> getPolygonsForTile(int x, int y) const;
        std::vector<TerrainPolygon> getPolygonsForPoint(GeoPoint point) const;
        bool isOnLand(GeoPoint point) const;
};

#endif // terrainChecker_H