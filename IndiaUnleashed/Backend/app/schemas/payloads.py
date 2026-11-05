from typing import List, Optional
from pydantic import BaseModel, Field

class AuthLoginRequest(BaseModel):
    account_id: str = Field(..., min_length=3)
    display_name: str = Field(..., min_length=2)
    platform: str = Field(default="WindowsPC")

class AuthLoginResponse(BaseModel):
    account_id: str
    session_token: str
    profile_level: int
    currency_inr: int

class PlayerProfileResponse(BaseModel):
    account_id: str
    display_name: str
    currency_inr: int
    experience_xp: int
    level: int
    kills: int
    deaths: int
    br_victories: int
    current_district: str
    inventory_items: List[str]
    owned_vehicles: List[str]
    completed_missions: List[str]

class ServerGrantRewardRequest(BaseModel):
    account_id: str
    mission_id: str
    currency_delta_inr: int = Field(..., ge=0, le=500000)
    xp_delta: int = Field(..., ge=0, le=100000)
    reward_item_id: Optional[str] = None

class ServerCombatResultRequest(BaseModel):
    match_id: str
    killer_account_id: str
    victim_account_id: str
    is_match_victory: bool = False

class VehiclePurchaseRequest(BaseModel):
    account_id: str
    vehicle_id: str
    price_inr: int = Field(..., gt=0)

class PartyCreateRequest(BaseModel):
    leader_account_id: str
    max_members: int = Field(default=4, ge=2, le=4)

class MatchmakingEnqueueRequest(BaseModel):
    account_id: str
    mode: str = Field(default="OpenWorld_Coop") # OpenWorld_Coop, MultiplayerCity, BR_Solo, BR_Duo, BR_Squad
    region: str = Field(default="ap-south-hyderabad")
