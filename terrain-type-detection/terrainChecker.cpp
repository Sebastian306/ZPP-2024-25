#include "terrainChecker.h"

TerrainChecker::TerrainChecker(std::string tilesFolderPath, std::string tilesPropertiesFileName, std::string tileFileNameTemplate) {
    this->tilesFolderPath = tilesFolderPath;
    this->tilesPropertiesFileName = tilesPropertiesFileName;
    this->tileFileNameTemplate = tileFileNameTemplate;
}

