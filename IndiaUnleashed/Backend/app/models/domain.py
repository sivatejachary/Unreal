from dataclasses import dataclass, field
from typing import List, Dict

@dataclass
class PlayerProfileRecord:
    account_id: str
    display_name: str
    currency_inr: int = 5000
    experience_xp: int = 0
    level: int = 1
    kills: int = 0
    deaths: int = 0
    br_victories: int = 0
    current_district: str = "OldCity_Charminar"
    inventory_items: List[str] = field(default_factory=lambda: ["WPN_PISTOL_9MM"])
    owned_vehicles: List[str] = field(default_factory=lambda: ["VH_HYD_AUTORICKSHAW_01"])
    completed_missions: List[str] = field(default_factory=list)
