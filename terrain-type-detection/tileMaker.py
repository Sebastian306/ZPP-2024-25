import geopandas as gpd
import matplotlib.pyplot as plt
import numpy as np
import os
import struct

import geopandas as gpd
from shapely.geometry import Polygon
from shapely.affinity import translate


DATA_PATH = './land-polygons-split-4326/land_polygons.shp'
TILES_FOLDER = './tiles/'
TILES_X_SIZE = 3
TILES_Y_SIZE = 3
MIN_POLYGON_AREA = 0.001

class PolygonMap:
    def __init__(self, gdf, tileSizeX, tileSizeY):
        self.gdf = gdf
        self.tileSizeX = tileSizeX
        self.tileSizeY = tileSizeY
        self.bounds = [-90, -180, 90, 180]

        # Przesunięcie współrzędnych, aby nie było wartości ujemnych
        self.minx, self.miny, self.maxx, self.maxy = self.bounds
        self.mapShiftX = -self.minx  # Przesunięcie na osi X
        self.mapShiftY = -self.miny  # Przesunięcie na osi Y

        self.mapLenX = self.maxx + self.mapShiftX
        self.mapLenY = self.maxy + self.mapShiftY

        self.polygons = []

        # Obliczamy ilość kafelków na osi X i Y
        self.tilesCountX = int((self.maxx + self.mapShiftX) // tileSizeX + 1)
        self.tilesCountY = int((self.maxy + self.mapShiftY) // tileSizeY + 1)

        # Tworzymy dwuwymiarową listę kafelków
        self.tiles = []
        for i in range(self.tilesCountX):
            self.tiles.append([])
            for j in range(self.tilesCountY):
                self.tiles[i].append([])

        # Iterujemy przez każdy poligon
        for polygon in gdf['geometry']:
            if polygon.is_valid:
                # Przesuwamy współrzędne każdego poligonu za pomocą translate
                shifted_polygon = translate(polygon, xoff=self.mapShiftX, yoff=self.mapShiftY)
                self.polygons.append(shifted_polygon)
                index = len(self.polygons) - 1

                # Pobieramy współrzędne po przesunięciu
                min_x = shifted_polygon.bounds[0]
                min_y = shifted_polygon.bounds[1]
                max_x = shifted_polygon.bounds[2]
                max_y = shifted_polygon.bounds[3]

                # Przypisanie poligonów do odpowiednich kafelków
                for i in range(int(min_x // tileSizeX), int(max_x // tileSizeX + 1)):
                    for j in range(int(min_y // tileSizeY), int(max_y // tileSizeY + 1)):
                        self.tiles[i][j].append(index)

    def getLens(self):
        lens = []
        for i in range(self.tilesCountX):
            for j in range(self.tilesCountY):
                lens.append(len(self.tiles[i][j]))
        return lens


def polygonArea(vertices):
    # Zamiana listy wierzchołków na tablicę numpy
    vertices = np.array(vertices)
    
    # Rozdzielenie współrzędnych na x i y
    x = vertices[:, 0]
    y = vertices[:, 1]
    
    # Obliczanie pola według wzoru Shoelaeckera
    area = 0.5 * np.abs(np.dot(x, np.roll(y, 1)) - np.dot(y, np.roll(x, 1)))
    
    return area

def saveTile(polMap, i, j):
    # Zapis kafelka z poligonami do pliku w formie tekstowej
    tile = polMap.tiles[i][j]
    with open(f'tiles/tile_{i}_{j}.txt', 'wb') as f:
        f.write(struct.pack('i', len(tile)))
        for index in tile:
            polygon = polMap.polygons[index]
            f.write(struct.pack('i', len(polygon.exterior.coords)))
            for coord in polygon.exterior.coords:
                f.write(struct.pack('dd', coord[0], coord[1]))
                
def swap_lon_lat(geometry):
    if geometry.geom_type == 'Polygon':
        return Polygon([(y, x) for x, y in geometry.exterior.coords])
    return geometry

if __name__ == '__main__':
    # Wczytaj plik Shapefile
    shapefile_path = DATA_PATH
    print(f'Loading shapefile from {shapefile_path}')
    gdf = gpd.read_file(shapefile_path)
    print("Swapping lon and lat")
    gdf['geometry'] = gdf['geometry'].apply(swap_lon_lat)

    minPolygonArea = MIN_POLYGON_AREA
    print(f'Reducing polygons with area less than {minPolygonArea} surface units')
    gdfReduced = gdf[gdf['geometry'].apply(lambda x: polygonArea(x.exterior.coords) > minPolygonArea)]
    print(f'Number of polygons before reduction: {len(gdf)}')
    print(f'Number of polygons after reduction: {len(gdfReduced)}')

    print('Creating PolygonMap')
    polM = PolygonMap(gdfReduced, TILES_X_SIZE, TILES_Y_SIZE)
    # wypisanie ile jest niepustych kaflów
    print(len([1 for l in polM.getLens() if l > 0]), ' tiles are not empty')

    # Sprawdzanie czy istnieje folder o nazwie 'tiles', jeśli nie to go tworzymy
    if not os.path.exists('tiles'):
        print('Creating folder tiles')
        os.makedirs('tiles')
    else:
        print('Clearing folder tiles')
        files = os.listdir('tiles')
        for file in files:
            os.remove(f'tiles/{file}')


    # Zapisanie danych do pliku tiles.properties
    print('Saving tiles.properties')
    with open('tiles/tiles.properties', 'w') as f:
        f.write(f'tilesCountX={polM.tilesCountX}\n')
        f.write(f'tilesCountY={polM.tilesCountY}\n')
        f.write(f'tileSizeX={TILES_X_SIZE}\n')
        f.write(f'tileSizeY={TILES_Y_SIZE}\n')
        f.write(f'minPolygonArea={MIN_POLYGON_AREA}\n')
        f.write(f'shiftX={polM.mapShiftX}\n')
        f.write(f'shiftY={polM.mapShiftY}\n')

    print('Saving tiles')
    for i in range(polM.tilesCountX):
        for j in range(polM.tilesCountY):
            if len(polM.tiles[i][j]) > 0:
                saveTile(polM, i, j)
    
    
                


