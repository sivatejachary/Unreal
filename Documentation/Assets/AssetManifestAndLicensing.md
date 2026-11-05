# Asset Manifest & Licensing Registry (Alpha 0.1)

Strict adherence to Section 2 & Rule 12: Zero proprietary GTA or PUBG characters, maps, missions, branding, or code are used. All landmark geometry in Alpha 0.1 is original procedural C++ blockout geometry inspired by public-domain historical proportions of Hyderabad monuments.

| Asset ID | Category | Path / Source | License / Provenance | Target Platform LODs |
| :--- | :--- | :--- | :--- | :--- |
| `CHARMINAR_BLOCKOUT` | Landmark Actor | `Source/IUWorld/Private/IUCharminarBlockoutActor.cpp` | 100% Original C++ Procedural Box/Arch Blockout | PC: Nanite / Mobile: 3-LOD Static Mesh |
| `M_Charminar_LimeStucco` | Material | `/Game/Environment/Materials/M_Charminar_LimeStucco` | Original PBR Material Graph | PC: 4K PBR / Android: 1K ASTC Orm-packed |
| `M_Hyderabad_DeccanGranite` | Material | `/Game/Environment/Materials/M_Hyderabad_DeccanGranite` | Original PBR Material Graph | PC: 4K PBR / Android: 1K ASTC |
| `VH_HYD_AUTORICKSHAW_01` | Vehicle Preset | `Source/IUVehicles/Private/IUVehicleBase.cpp` | Original 3-Wheeler Chaos Vehicle Config | LOD0: 28k tris, LOD1: 12k, LOD2 (Mobile): 4.5k |
| `VH_DECCAN_SEDAN_GT` | Vehicle Preset | `Source/IUVehicles/Private/IUVehicleBase.cpp` | Original 4-Door Sedan Chaos Vehicle Config | LOD0: 42k tris, LOD1: 18k, LOD2 (Mobile): 6.5k |
| `VH_TSRTC_METRO_EXPRESS` | Vehicle Preset | `Source/IUVehicles/Private/IUVehicleBase.cpp` | Original City Bus Chaos Vehicle Config | LOD0: 48k tris, LOD1: 20k, LOD2 (Mobile): 7.5k |
| `WPN_AR_INSAS_M1` | Weapon Preset | `Source/IUCombat/Private/IUWeaponBase.cpp` | Original Fictional `Vajra-556` Rifle Config | LOD0: 16k tris, Mobile LOD1: 5k tris |
