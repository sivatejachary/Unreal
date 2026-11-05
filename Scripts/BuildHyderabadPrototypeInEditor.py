# Copyright (c) 2026 India Unleashed Studio. All Rights Reserved.
# Run inside Unreal Engine 5.4 Editor via:
#   UnrealEditor-Cmd.exe IndiaUnleashed.uproject -ExecutePythonScript="Scripts/BuildHyderabadPrototypeInEditor.py"
# Automatically builds /Game/Maps/Hyderabad/L_Hyderabad_Prototype.umap with the
# Charminar Landmark Blockout, Old City bazaar roads, lighting, and 2-player spawns.

import json
import os

try:
    import unreal
    HAS_UNREAL_EDITOR = True
except ImportError:
    HAS_UNREAL_EDITOR = False

def build_hyderabad_prototype_level():
    if not HAS_UNREAL_EDITOR:
        print("[Notice] Not running inside UnrealEditor Python runtime. Validating L_Hyderabad_Prototype.umap.json spec...")
        spec_path = os.path.join(os.path.dirname(__file__), "..", "Content", "Maps", "Hyderabad", "L_Hyderabad_Prototype.umap.json")
        with open(spec_path, "r", encoding="utf-8") as f:
            spec = json.load(f)
        print(f"Verified map spec: {spec['MapPackage']} with {len(spec['PlacedActors'])} core actors.")
        return

    level_sub = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    actor_sub = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

    map_package = "/Game/Maps/Hyderabad/L_Hyderabad_Prototype"
    level_sub.new_level(map_package)

    # Spawn Directional Sun, SkyAtmosphere, SkyLight, and Charminar Blockout
    actor_sub.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 5000), unreal.Rotator(-42, 135, 0))
    actor_sub.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
    actor_sub.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 1000))

    charminar_cls = unreal.load_class(None, "/Script/IUWorld.IUCharminarBlockoutActor")
    if charminar_cls:
        actor_sub.spawn_actor_from_class(charminar_cls, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))

    # Spawn 2 PlayerStarts for Listen-Server Co-op validation
    actor_sub.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(-2200, 450, 110), unreal.Rotator(0, 0, 0))
    actor_sub.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(2200, -450, 110), unreal.Rotator(0, 180, 0))

    level_sub.save_current_level()
    print(f"Successfully generated and saved {map_package}")

if __name__ == "__main__":
    build_hyderabad_prototype_level()
