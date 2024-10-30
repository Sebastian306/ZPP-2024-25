#include "terrainChecker.h"
#include "../common.h"
#include <iostream>

#define TESTS_FOLDER "testPoints/"
#define ON_LAND_FILE "onLand.in"
#define ON_OCEAN_FILE "onOcean.in"

#define CHECK_ERROR_ON_FILE(file) \
if (!file) { \
    std::cerr << "Error: opening file for reading!" << std::endl; \
    file.close(); \
    return; \
} \
if (!file.is_open()) { \
    std::cerr << "Error: opening file for reading: " << filename << std::endl; \
    file.close(); \
    return; \
} 

void test_points(bool expected, std::string filename) {
    TerrainChecker tc;
    std::ifstream file(filename);
    CHECK_ERROR_ON_FILE(file);
    int n;
    std::cout << "Reading from file: " << filename << std::endl;
    file >> n;
    if (n == 0) {
        std::cerr << "Error: number of points is 0" << std::endl;
        return;
    }
    std::cout << "Number of points in test: " << n << std::endl;
    for (int i = 0; i < n; ++i) {
        double lat, lon;
        file >> lat >> lon;
        CHECK_ERROR_ON_FILE(file);
        GeoPoint point(lat, lon);
        if (tc.isOnLand(point) != expected) {
            if (expected) {
                std::cout << "Error: point (" << lat << ", " << lon << ") is on water" << std::endl;
            } else {
                std::cout << "Error: point (" << lat << ", " << lon << ") is on land" << std::endl;
            }
        }
    }
    file.close();
}

void test_input(){
    TerrainChecker tc;
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        double lat, lon;
        std::cin >> lat >> lon;
        GeoPoint point(lat, lon);
        if (tc.isOnLand(point)) {
            std::cout << "Land" << std::endl;
        } else {
            std::cout << "Water" << std::endl;
        }
    }
}

int main() {
    test_points(true, std::string(TERRAIN_TYPE_FOLDER_PATH) + std::string(TESTS_FOLDER) + std::string(ON_LAND_FILE));
    test_points(false, std::string(TERRAIN_TYPE_FOLDER_PATH) + std::string(TESTS_FOLDER) + std::string(ON_OCEAN_FILE));
}