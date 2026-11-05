from typing import Optional
from fastapi import APIRouter, Header
from app.core.security import verify_dedicated_server_signature
from app.services.game_service import GameBackendService
from app.repositories.store import repo
from app.schemas.payloads import (
    AuthLoginRequest, AuthLoginResponse, PlayerProfileResponse,
    ServerGrantRewardRequest, ServerCombatResultRequest,
    VehiclePurchaseRequest, PartyCreateRequest, MatchmakingEnqueueRequest
)

router = APIRouter(prefix="/api/v1")

@router.post("/auth/login", response_model=AuthLoginResponse)
def login(req: AuthLoginRequest) -> AuthLoginResponse:
    return GameBackendService.authenticate_player(req)

@router.get("/profiles/{account_id}", response_model=PlayerProfileResponse)
def get_profile(account_id: str) -> PlayerProfileResponse:
    return GameBackendService.get_player_profile(account_id)

@router.post("/missions/complete", response_model=PlayerProfileResponse)
def complete_mission_authoritative(
    req: ServerGrantRewardRequest,
    x_iu_server_signature: Optional[str] = Header(default=None)
) -> PlayerProfileResponse:
    canonical = f"{req.account_id}:{req.mission_id}:{req.currency_delta_inr}:{req.xp_delta}"
    verify_dedicated_server_signature("/api/v1/missions/complete", canonical, x_iu_server_signature or "")
    return GameBackendService.server_grant_mission_reward(req)

@router.post("/combat/record-kill")
def record_combat_result_authoritative(
    req: ServerCombatResultRequest,
    x_iu_server_signature: Optional[str] = Header(default=None)
):
    canonical = f"{req.match_id}:{req.killer_account_id}:{req.victim_account_id}:{int(req.is_match_victory)}"
    verify_dedicated_server_signature("/api/v1/combat/record-kill", canonical, x_iu_server_signature or "")
    return GameBackendService.server_record_combat_result(req)

@router.post("/transactions/purchase-vehicle", response_model=PlayerProfileResponse)
def purchase_vehicle(req: VehiclePurchaseRequest) -> PlayerProfileResponse:
    return GameBackendService.purchase_vehicle(req)

@router.post("/parties/create")
def create_party(req: PartyCreateRequest):
    return GameBackendService.create_party(req)

@router.post("/matchmaking/enqueue")
def enqueue_matchmaking(req: MatchmakingEnqueueRequest):
    return GameBackendService.enqueue_matchmaking(req)

@router.get("/leaderboards/global")
def get_global_leaderboard():
    records = repo.get_leaderboard(10)
    return {
        "entries": [
            {
                "account_id": r.account_id,
                "display_name": r.display_name,
                "level": r.level,
                "experience_xp": r.experience_xp,
                "kills": r.kills,
                "br_victories": r.br_victories
            }
            for r in records
        ]
    }
