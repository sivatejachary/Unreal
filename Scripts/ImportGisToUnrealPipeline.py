# Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
# ImportGisToUnrealPipeline.py
# Automated GIS-to-Unreal Import & 3D Reconstruction Pipeline.
# Converts WGS84 GeoJSON datasets (Roads, Buildings, Water, POIs)
# into Unreal Engine 5 Landscape, Spline Roads, Procedural Meshes, and Level Actors.

import json
import math
import os
import sys

# Geospatial Cesium Origin Configuration (Central Hyderabad)
CENTRAL_LAT = 17.385044
CENTRAL_LON = 78.486671
CENTRAL_ALT = 505.0

WGS84_A = 6378137.0
WGS84_E2 = 0.0066943799901413165
DEG_TO_RAD = math.pi / 180.0

def geodetic_to_ecef(lon, lat, h):
    phi = lat * DEG_TO_RAD
    lam = lon * DEG_TO_RAD
    sin_phi = math.sin(phi)
    cos_phi = math.cos(phi)
    sin_lam = math.sin(lam)
    cos_lam = math.cos(lam)
    n = WGS84_A / math.sqrt(1.0 - WGS84_E2 * sin_phi * sin_phi)
    x = (n + h) * cos_phi * cos_lam
    y = (n + h) * cos_phi * sin_lam
    z = (n * (1.0 - WGS84_E2) + h) * sin_phi
    return x, y, z

ORIGIN_X, ORIGIN_Y, ORIGIN_Z = geodetic_to_ecef(CENTRAL_LON, CENTRAL_LAT, CENTRAL_ALT)

def wgs84_to_unreal_cm(lon, lat, alt_m=505.0):
    tx, ty, tz = geodetic_to_ecef(lon, lat, alt_m)
    dx = tx - ORIGIN_X
    dy = ty - ORIGIN_Y
    dz = tz - ORIGIN_Z

    phi0 = CENTRAL_LAT * DEG_TO_RAD
    lam0 = CENTRAL_LON * DEG_TO_RAD
    sin_phi0 = math.sin(phi0)
    cos_phi0 = math.cos(phi0)
    sin_lam0 = math.sin(lam0)
    cos_lam0 = math.cos(lam0)

    east_m = -sin_lam0 * dx + cos_lam0 * dy
    north_m = -sin_phi0 * cos_lam0 * dx - sin_phi0 * sin_lam0 * dy + cos_phi0 * dz
    up_m = cos_phi0 * cos_lam0 * dx + cos_phi0 * sin_lam0 * dy + sin_phi0 * dz

    # Unreal centimeters: X = North, Y = East, Z = Up
    return round(north_m * 100.0, 1), round(east_m * 100.0, 1), round(up_m * 100.0, 1)

class HyderabadGisPipeline:
    def __init__(self, project_root):
        self.root = project_root
        self.gis_dir = os.path.join(project_root, "Content", "INDIA_UNLEASHED", "GIS")
        self.roads_file = os.path.join(self.gis_dir, "Hyderabad_Road_Network.geojson")
        self.bldgs_file = os.path.join(self.gis_dir, "Hyderabad_Building_Footprints.geojson")
        self.water_file = os.path.join(self.gis_dir, "Hyderabad_Water_Bodies.geojson")
        self.geo_db_file = os.path.join(self.gis_dir, "Hyderabad_Geographical_Database.json")

    def run_pipeline(self):
        print("================================================================")
        print(" INDIA UNLEASHED - AUTOMATED GIS-TO-UNREAL RECONSTRUCTION PIPELINE")
        print("================================================================")
        print(f"Cesium Georeference Origin: Lat {CENTRAL_LAT} N, Lon {CENTRAL_LON} E, Height {CENTRAL_ALT}m")

        roads = self.process_roads()
        bldgs = self.process_buildings()
        water = self.process_water_bodies()
        pois = self.process_pois()

        level_spec = self.generate_ue_level_spec(roads, bldgs, water, pois)
        self.generate_attribution_manifest()

        print("=== PIPELINE EXECUTION COMPLETE ===")
        print(f"Processed: {len(roads)} Road Networks | {len(bldgs)} 3D Building Footprints | {len(water)} Water Bodies | {len(pois)} POI Anchors")
        return level_spec

    def process_roads(self):
        with open(self.roads_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        processed = []
        for feat in data["features"]:
            props = feat["properties"]
            coords = feat["geometry"]["coordinates"]
            unreal_spline_points = []
            for pt in coords:
                lon, lat = pt[0], pt[1]
                ux, uy, uz = wgs84_to_unreal_cm(lon, lat)
                unreal_spline_points.append({"x": ux, "y": uy, "z": uz})

            processed.append({
                "road_id": props["road_id"],
                "name": props["name"],
                "road_type": props["road_type"],
                "lanes": props["lanes"],
                "width_m": props["width_m"],
                "speed_limit_kmh": props["speed_limit_kmh"],
                "spline_points_cm": unreal_spline_points,
                "has_divider": props.get("has_divider", False),
                "streetlights": props.get("streetlights", "Standard_LED")
            })
        return processed

    def process_buildings(self):
        with open(self.bldgs_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        processed = []
        for feat in data["features"]:
            props = feat["properties"]
            poly = feat["geometry"]["coordinates"][0]
            unreal_poly_points = []
            center_x, center_y = 0.0, 0.0
            for pt in poly:
                ux, uy, uz = wgs84_to_unreal_cm(pt[0], pt[1])
                unreal_poly_points.append({"x": ux, "y": uy, "z": uz})
                center_x += ux
                center_y += uy

            n = len(poly)
            center_x /= n
            center_y /= n

            processed.append({
                "bldg_id": props["bldg_id"],
                "name": props["name"],
                "height_m": props["height_m"],
                "floors": props["floors"],
                "category": props["category"],
                "material": props["material"],
                "center_cm": {"x": round(center_x, 1), "y": round(center_y, 1), "z": 0.0},
                "footprint_points_cm": unreal_poly_points
            })
        return processed

    def process_water_bodies(self):
        with open(self.water_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        processed = []
        for feat in data["features"]:
            props = feat["properties"]
            geom_type = feat["geometry"]["type"]
            coords = feat["geometry"]["coordinates"]

            unreal_coords = []
            if geom_type == "Polygon":
                for pt in coords[0]:
                    ux, uy, uz = wgs84_to_unreal_cm(pt[0], pt[1], props["elevation_m"])
                    unreal_coords.append({"x": ux, "y": uy, "z": uz})
            elif geom_type == "LineString":
                for pt in coords:
                    ux, uy, uz = wgs84_to_unreal_cm(pt[0], pt[1], props["elevation_m"])
                    unreal_coords.append({"x": ux, "y": uy, "z": uz})

            processed.append({
                "name": props["name"],
                "water_type": props["water_type"],
                "elevation_m": props["elevation_m"],
                "shoreline_material": props["shoreline_material"],
                "geometry_type": geom_type,
                "unreal_points_cm": unreal_coords
            })
        return processed

    def process_pois(self):
        with open(self.geo_db_file, "r", encoding="utf-8") as f:
            data = json.load(f)
        return data["locations"]

    def generate_ue_level_spec(self, roads, bldgs, water, pois):
        level_actors = []

        # 1. Cesium Georeference Actor
        level_actors.append({
            "ActorClass": "/Script/CesiumRuntime.CesiumGeoreference",
            "ActorLabel": "CesiumGeoreference_CentralHyderabad",
            "OriginPlacement": "CartographicOrigin",
            "OriginLatitude": CENTRAL_LAT,
            "OriginLongitude": CENTRAL_LON,
            "OriginHeight": CENTRAL_ALT,
            "Scale": 1.0,
            "LocationCm": [0.0, 0.0, 0.0]
        })

        # 2. Cesium SunSky
        level_actors.append({
            "ActorClass": "/Script/CesiumRuntime.CesiumSunSky",
            "ActorLabel": "CesiumSunSky_Hyderabad",
            "SolarTimeHours": 14.5,
            "TimeZoneOffsetHours": 5.5,
            "DayOfYear": 180,
            "LocationCm": [0.0, 0.0, 50000.0]
        })

        # 3. Roads Spline Actors
        for r in roads:
            level_actors.append({
                "ActorClass": "/Script/IUWorld.IURoadSplineActor",
                "ActorLabel": f"Road_{r['road_id']}",
                "RoadName": r["name"],
                "RoadType": r["road_type"],
                "Lanes": r["lanes"],
                "WidthMeters": r["width_m"],
                "SpeedLimitKmh": r["speed_limit_kmh"],
                "SplinePointsCount": len(r["spline_points_cm"]),
                "LocationCm": [r["spline_points_cm"][0]["x"], r["spline_points_cm"][0]["y"], r["spline_points_cm"][0]["z"]]
            })

        # 4. 3D Buildings
        for b in bldgs:
            level_actors.append({
                "ActorClass": "/Script/IUWorld.IUProceduralBuildingActor",
                "ActorLabel": f"Building_{b['bldg_id']}",
                "BuildingName": b["name"],
                "HeightMeters": b["height_m"],
                "Floors": b["floors"],
                "MaterialPreset": b["material"],
                "LocationCm": [b["center_cm"]["x"], b["center_cm"]["y"], b["center_cm"]["z"]]
            })

        # 5. Water Bodies
        for w in water:
            level_actors.append({
                "ActorClass": "/Script/Water.WaterBodyOcean" if w["geometry_type"] == "Polygon" else "/Script/Water.WaterBodyRiver",
                "ActorLabel": f"Water_{w['name'].replace(' ', '_').replace('(', '').replace(')', '')}",
                "WaterType": w["water_type"],
                "ElevationMeters": w["elevation_m"],
                "LocationCm": [w["unreal_points_cm"][0]["x"], w["unreal_points_cm"][0]["y"], w["unreal_points_cm"][0]["z"]]
            })

        # 6. POI Anchors & Player Starts
        for p in pois:
            c = p["unreal_coordinates_cm"]
            level_actors.append({
                "ActorClass": "/Script/IUWorld.IUPOIAnchorActor",
                "ActorLabel": f"POI_{p['location_id']}",
                "LocationName": p["name"],
                "District": p["district"],
                "Latitude": p["latitude"],
                "Longitude": p["longitude"],
                "ElevationMeters": p["elevation_m"],
                "LocationCm": [c["x_north_cm"], c["y_east_cm"], c["z_up_cm"]]
            })

        level_spec = {
            "MapPackage": "/Game/Maps/Hyderabad/L_Hyderabad_World",
            "EngineVersion": "5.4",
            "WorldPartitionEnabled": True,
            "GridCellSizeCm": 25600,
            "LoadingRangeCm": 76800,
            "TotalActorCount": len(level_actors),
            "PlacedActors": level_actors
        }

        out_path = os.path.join(self.root, "Content", "Maps", "Hyderabad", "L_Hyderabad_World.umap.json")
        with open(out_path, "w", encoding="utf-8") as f:
            json.dump(level_spec, f, indent=2)

        print(f"Generated complete Level Specification at {out_path} ({len(level_actors)} total placed actors)")
        return level_spec

    def generate_attribution_manifest(self):
        manifest = {
            "project": "INDIA UNLEASHED - Hyderabad Open World",
            "pipeline": "Automated GIS-to-Unreal 3D Reconstruction Pipeline",
            "coordinate_system": "WGS84 EPSG:4326 to Unreal Engine 5 LHS (Centimeters)",
            "georeference_origin": {
                "latitude": CENTRAL_LAT,
                "longitude": CENTRAL_LON,
                "elevation_m": CENTRAL_ALT
            },
            "data_sources_and_licensing": [
                {
                    "source": "OpenStreetMap (OSM)",
                    "data_types": ["Road Networks", "Building Footprints", "Water Bodies", "Land Use"],
                    "license": "Open Data Commons Open Database License (ODbL) 1.0",
                    "attribution": "© OpenStreetMap contributors",
                    "url": "https://www.openstreetmap.org/copyright"
                },
                {
                    "source": "SRTM (Shuttle Radar Topography Mission) & Bhuvan ISRO",
                    "data_types": ["Deccan Plateau 30m Digital Elevation Model (DEM)"],
                    "license": "Public Domain / Open Data Policy (ISRO National Remote Sensing Centre)",
                    "attribution": "NASA JPL / ISRO NRSC Bhuvan Portal",
                    "url": "https://bhuvan.nrsc.gov.in"
                },
                {
                    "source": "Greater Hyderabad Municipal Corporation (GHMC) & HMDA",
                    "data_types": ["Master Plan 2031 Zonal Classifications, Ward Boundaries, Flyover Interchanges"],
                    "license": "Government Open Data License - India (GODL)",
                    "attribution": "Government of Telangana - Metropolitan Development Authority",
                    "url": "https://data.telangana.gov.in"
                },
                {
                    "source": "Cesium for Unreal Engine",
                    "data_types": ["Georeference Runtime, Ellipsoidal Coordinate Math, 3D Tiles Streaming"],
                    "license": "Apache License 2.0",
                    "attribution": "Cesium GS, Inc.",
                    "url": "https://github.com/CesiumGS/cesium-unreal"
                }
            ]
        }
        out_doc = os.path.join(self.root, "Content", "INDIA_UNLEASHED", "GIS", "GIS_DATA_SOURCES_AND_LICENSING.json")
        with open(out_doc, "w", encoding="utf-8") as f:
            json.dump(manifest, f, indent=2)
        print(f"Saved Data Sources and Licensing Manifest: {out_doc}")

if __name__ == "__main__":
    proj_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    pipeline = HyderabadGisPipeline(proj_dir)
    pipeline.run_pipeline()
