#include "terrainChecker.h"

TerrainChecker::TerrainChecker() {
    tileSizeX = 3;
    tileSizeY = 3;
    tilesCountX = 61;
    tilesCountY = 121;
    shiftX = 90;
    shiftY = 180;
}

std::pair<int, int> TerrainChecker::getTileForPoint(GeoPoint point) const {
    int x = (point.getLat() + shiftX) / tileSizeX;
    int y = (point.getLon() + shiftY) / tileSizeY;
    return std::make_pair(x, y);
}

std::string TerrainChecker::getTileFileName(int x, int y) {
    // using "tileFileNameTemplate = tile_{}_{}"
    return TILES_FOLDER_PATH + std::format(TILES_FILE_NAME_TEMPLATE, x, y);
}

void TerrainChecker::readTile(int x, int y)  {
    std::string filename = getTileFileName(x, y);
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        terrainMap[std::make_tuple(x, y)] = std::vector<TerrainPolygon>();
        return;
    }
    

    int polygonsCount;
    inFile.read(reinterpret_cast<char*>(&polygonsCount), sizeof(polygonsCount));  // Odczyt liczby wierzchołków
    std::vector<TerrainPolygon> polygons;
    for (int i = 0; i < polygonsCount; ++i) {
        int verticesCount;
        inFile.read(reinterpret_cast<char*>(&verticesCount), sizeof(verticesCount));  // Odczyt liczby wierzchołków
        std::vector<std::pair<double, double>> coords;
        for (int j = 0; j < verticesCount; ++j) {
            double lat, lon;
            inFile.read(reinterpret_cast<char*>(&lat), sizeof(lat));  // Odczyt lat
            inFile.read(reinterpret_cast<char*>(&lon), sizeof(lon));  // Odczyt lon
            coords.push_back(std::make_pair(lat, lon));
        }

        polygons.push_back(TerrainPolygon(coords));
    }
    inFile.close();
    
    terrainMap[std::make_tuple(x, y)] = polygons;
}

std::vector<TerrainPolygon> TerrainChecker::getPolygonsForTile(int x, int y){
    if(terrainMap.find(std::make_tuple(x, y)) == terrainMap.end()) {
        readTile(x, y);
        if(terrainMap.find(std::make_tuple(x, y)) == terrainMap.end()) {
            return std::vector<TerrainPolygon>();
        }
    }
    return terrainMap[std::make_tuple(x, y)];
};

std::vector<TerrainPolygon> TerrainChecker::getPolygonsForPoint(GeoPoint point){
    std::pair<int, int> tile = getTileForPoint(point);
    return getPolygonsForTile(tile.first, tile.second);
}
bool TerrainChecker::isOnLand(GeoPoint point){
    std::vector<TerrainPolygon> polygons = getPolygonsForPoint(point);
    // std::cout<<"Point mapped to tile: "<<this->getTileForPoint(point).first<<" "<<this->getTileForPoint(point).second<<std::endl;
    // std::cout<<"Checking if point is on land: "<<point.getLat() + shiftX<<" "<<point.getLon() + shiftY<<std::endl;
    for (auto polygon : polygons) {
        if (polygon.isInside(point.getLat() + shiftX, point.getLon() + shiftY)) {
            // std::cout<<polygon.toString()<<std::endl;
            return true;
        }
    }
    return false;
}
