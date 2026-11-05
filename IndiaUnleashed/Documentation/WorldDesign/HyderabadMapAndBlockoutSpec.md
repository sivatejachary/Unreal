# Hyderabad Open-World Map & Charminar Alpha 0.1 Blockout Specification

## 1. World Partition & Coordinate Architecture
- **Map Asset**: `/Game/Maps/Hyderabad/L_Hyderabad_Prototype` (descriptor at `Content/Maps/Hyderabad/L_Hyderabad_Prototype.umap.json`, generated in-Editor via `Scripts/BuildHyderabadPrototypeInEditor.py`).
- **Origin `(0, 0, 0)`**: Exact center of the **Charminar Monument Plaza** in Hyderabad's Old City.
- **World Partition Grid**: `128m x 128m` (`12,800 cm`) cell size with a `384m` (`38,400 cm`) streaming load range on PC (`256m` on Android).
- **Data Layers**:
  - `DL_OldCity_Core` (Enabled in Alpha 0.1)
  - `DL_TankBund` (Enabled in Alpha 0.1 for Mission M01 delivery)
  - `DL_CyberCity`, `DL_Golconda`, `DL_Airport`, `DL_OuterHyderabad` (Registered in `UIUHyderabadWorldSubsystem`, streamed in subsequent phases)

## 2. Charminar Monument Blockout (`AIUCharminarBlockoutActor`)
Implemented in `Source/IUWorld/Private/IUCharminarBlockoutActor.cpp` using 18 collision-enabled architectural primitives matching historical proportions:
- **Granite Plaza Plinth**: `36m x 36m x 0.9m` (`M_Hyderabad_DeccanGranite`).
- **Main Square Base Structure**: `20m x 20m` footprint, `20m` (`2,000 cm`) high (`M_Charminar_LimeStucco`).
- **Four Grand Arches (North, South, East, West)**: `11m` (`1,100 cm`) clear opening width, crowned by `5.5m` arch spans up to `Z = 2,000 cm`.
- **Four Minarets (NW, NE, SW, SE)**: Rising from the corner pillars to `56m` (`5,600 cm`) total elevation, topped with ornate dome crowns (`M_Charminar_OrnateStucco`).
- **Upper Mosque Gallery Slab**: Spanning `20m x 20m` at `Z = 2,100 cm`.
- **Four Radiating Bazaar Road Spline Anchors**:
  1. **West Avenue (`-120m, 0, 0`)**: Laad Bazaar (Bangle Market) pedestrian/rickshaw street.
  2. **East Avenue (`+120m, 0, 0`)**: Sardar Mahal / Chowmahalla route.
  3. **North Avenue (`0, -150m, 0`)**: Char Kaman archway leading north toward Musi River and Tank Bund.
  4. **South Avenue (`0, +100m, 0`)**: Mecca Masjid courtyard esplanade.

## 3. Six Hyderabad Districts & 18 Registered POIs (`UIUHyderabadWorldSubsystem`)
1. **Old City (`EIUHyderabadDistrict::OldCity`)**: Charminar Monument Plaza (`POI_OLD_01`), Laad Bazaar Bangle Market (`POI_OLD_02`), Char Kaman Archway (`POI_OLD_03`), Mecca Masjid Courtyard (`POI_OLD_04`).
2. **Tank Bund District (`EIUHyderabadDistrict::TankBund`)**: Hussain Sagar Lake & Buddha Statue Island (`POI_TB_01`), Tank Bund Embankment Promenade (`POI_TB_02`), Necklace Road Street Racing Circuit (`POI_TB_03`).
3. **Cyber City (`EIUHyderabadDistrict::CyberCity`)**: Cyber Towers Quadrant (`POI_CYB_01`), Durgam Cheruvu Cable Bridge (`POI_CYB_02`), Gachibowli Financial Skyscraper Hub (`POI_CYB_03`).
4. **Golconda District (`EIUHyderabadDistrict::Golconda`)**: Golconda Fort Citadel & Bala Hisar (`POI_GOL_01`), Deccan Granite Boulder Ridge (`POI_GOL_02`), Qutb Shahi Heritage Gardens (`POI_GOL_03`).
5. **Airport District (`EIUHyderabadDistrict::AirportDistrict`)**: Rajiv Gandhi Inspired International Terminal (`POI_AIR_01`), PVNR Elevated Expressway Interchange (`POI_AIR_02`).
6. **Outer Hyderabad (`EIUHyderabadDistrict::OuterHyderabad`)**: Pochampally-Inspired Handloom Village (`POI_OUT_01`), Ananthagiri Deccan Forest Reserve (`POI_OUT_02`), Outer Ring Road Toll Plaza (`POI_OUT_03`).
