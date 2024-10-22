import requests
from PIL import Image, ImageDraw
from random import randint
import json

def get_water_bodies():
    # Definicja zapytania do Overpass API dla obszaru Wrocławia
    overpass_url = "http://overpass-api.de/api/interpreter"
    #geolocation = "51.075, 16.965, 51.14436, 17.16957"  # Bounding box for Wrocław
    #geolocation = "13.205, 119.688, 14.910, 122.945"  # Bounding box for Main Crater Lake
    #geolocation = "14.2031, 120.6508, 14.4160, 121.0580"  # Bounding box for rivers above Main Crater Lake
    overpass_query = """
    [out:json];
    (
      way["natural"="water"](""" + geolocation + """);
      relation["natural"="water"](""" + geolocation + """);
    );
    out body;
    >;
    out skel qt;
    """

    # Wysłanie zapytania do Overpass API
    response = requests.post(overpass_url, data={'data': overpass_query})
    if response.status_code == 200:
        data = response.json()
        #save response to the file
        with open("output.json", "w", encoding='utf-8') as file:
            json.dump(data, file)
        water_bodies = data['elements']
        return water_bodies
    else:
        print("Błąd w zapytaniu: ", response.status_code)
        return []

def get_data_from_json(filename):
    with open(filename, "r", encoding="utf-8") as file:
        data = json.load(file)
    return data['elements']

def get_node_positions(node_ids):
    if len(node_ids) == 0:
        return {}
    overpass_url = "http://overpass-api.de/api/interpreter"
    nodes_query = """
    [out:json];
    node(id:{});
    out body;
    """.format(','.join(map(str, node_ids)))

    response = requests.post(overpass_url, data={'data': nodes_query})

    if response.status_code == 200:
        print("ok")
        data = response.json()
        node_positions = {node['id']: (node['lat'], node['lon']) for node in data['elements']}
        return node_positions
    else:
        print("Błąd w zapytaniu o węzły: ", response.status_code)
        return {}

def draw_water_body_on_image(polygons_lists, nodes_lists, image_size=(3600, 3600), margin=50):

    # Extract all latitudes and longitudes from node positions
    latitudes = [pos[0] for pos in nodes_lists.values()]
    longitudes = [pos[1] for pos in nodes_lists.values()]

    # Determine the bounding box for scaling
    min_lat, max_lat = min(latitudes), max(latitudes)
    min_lon, max_lon = min(longitudes), max(longitudes)
    
    # Create a blank image
    img = Image.new('RGB', image_size, (255, 255, 255))
    draw = ImageDraw.Draw(img)

    # Function to scale lat/lon to image coordinates
    def scale_coords(lat, lon):
        x = margin + (lon - min_lon) / (max_lon - min_lon) * (image_size[0] - 2 * margin)
        y = margin + (max_lat - lat) / (max_lat - min_lat) * (image_size[1] - 2 * margin)  # Inverted Y-axis
        return (int(x), int(y))

    for data in polygons_lists:
        node_ids = data["nodes"]
        node_positions = [nodes_lists[node_id] for node_id in node_ids]
        # Draw the water body path
        points = [scale_coords(lat, lon) for lat, lon in node_positions]
        random_color = (randint(0, 255), randint(0, 255), randint(0, 255))
        infill = (0,0,255)

        if data.get('tags', None) is not None:
            if data['tags'].get('role', None) == 'inner':
                infill = (0, 255, 0)
            elif data['tags'].get('role', None) == 'outer':
                infill = (255, 0, 0)
            
        #infill = random_color

        draw.line(points, fill=infill, width=3)


    # Save or show the image
    img.show()
    return img

def is_polygon(way):
    return way['nodes'][0] == way['nodes'][-1]

def connect_no_polygon_ways(no_polygon_ways, nodes_lists, polygons_lists):
    """
    @param no_polygon_ways: dict, key: first node id, value: way
    @param nodes_lists: dict, key: node_id, value: (lat, lon)
    @param polygons_lists: list of ways
    """
    while len(no_polygon_ways.keys()) > 0:
        first_node_id = list(no_polygon_ways.keys())[0]
        way = no_polygon_ways[first_node_id]
        no_polygon_ways.pop(first_node_id)
        last_node_id = way['nodes'][-1]
        while not is_polygon(way) and no_polygon_ways.get(last_node_id, None) is not None:
            next_way = no_polygon_ways[last_node_id]
            no_polygon_ways.pop(last_node_id)
            way['nodes'].extend(next_way['nodes'][1:])
            last_node_id = way['nodes'][-1]
        if is_polygon(way):
            polygons_lists.append(way)
        else:
            print("Nie udało się połączyć wszystkich odcinków")

def prepare_data(data):
    rel_map = {}
    way_map = {}
    node_map = {}
    added_ways = set() # ways already added by relations
    polygons_lists = []

    for body in data:
        if body['type'] == 'way':
            way_map[body['id']] = body
        elif body['type'] == 'relation':
            rel_map[body['id']] = body
        elif body['type'] == 'node':
            node_map[body['id']] = (body['lat'], body['lon'])

    for rel in rel_map.values():
        if rel['tags'].get('type', None) == 'multipolygon':
            no_polygon_ways = {} # key: first node id, value: way
            for member in rel['members']:
                if member['type'] == 'way':
                    way = way_map[member['ref']]
                    if way.get('tags', None) is None:
                        way['tags'] = {}

                    if member['role'] == 'outer':
                        way['tags']['role'] = 'outer'
                    elif member['role'] == 'inner':
                        way['tags']['role'] = 'inner'

                    if is_polygon(way):
                        polygons_lists.append(way)
                        added_ways.add(way['id'])
                    else:
                        no_polygon_ways[way['nodes'][0]] = way
            connect_no_polygon_ways(no_polygon_ways, node_map, polygons_lists)
    
    for way in way_map.values():
        if way['id'] not in added_ways and is_polygon(way):
            polygons_lists.append(way)

    return polygons_lists, node_map


            
    



# Wywołanie funkcji i rysowanie zbiornika wodnego
#water_bodies = get_water_bodies()
water_bodies = get_data_from_json("wroclaw.json")

polygons_lists, nodes_lists = prepare_data(water_bodies)

# Draw the way (water body) on the image
img = draw_water_body_on_image(polygons_lists, nodes_lists)
img.save("water_body_map.png")
