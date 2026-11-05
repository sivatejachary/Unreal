import hashlib
from typing import Dict, List
from fastapi import HTTPException, status
from app.models.domain import PlayerProfileRecord
from app.repositories.store import repo
from app.schemas.payloads import (
    AuthLoginRequest, AuthLoginResponse, PlayerProfileResponse,
    ServerGrantRewardRequest, ServerCombatResultRequest,
    VehiclePurchaseRequest, PartyCreateRequest, MatchmakingEnqueueRequest
)

class GameBackendService:
    @staticmethod
    def authenticate_player(req: AuthLoginRequest) -> AuthLoginResponse:
        prof = repo.get_profile(req.account_id)
        if not prof:
            prof = PlayerProfileRecord(account_id=req.account_id, display_name=req.display_name)
            repo.upsert_profile(prof)
        token = hashlib.sha256(f"{req.account_id}:{req.platform}:iu_session".encode("utf-8")).hexdigest()
        return AuthLoginResponse(
            account_id=prof.account_id,
            session_token=token,
            profile_level=prof.level,
            currency_inr=prof.currency_inr
        )

    @staticmethod
    def get_player_profile(account_id: str) -> PlayerProfileResponse:
        prof = repo.get_profile(account_id)
        if not prof:
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Player profile not found")
        return PlayerProfileResponse(**prof.__dict__)

    @staticmethod
    def server_grant_mission_reward(req: ServerGrantRewardRequest) -> PlayerProfileResponse:
        prof = repo.get_profile(req.account_id)
        if not prof:
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Player profile not found")

        if req.mission_id not in prof.completed_missions:
            prof.completed_missions.append(req.mission_id)

        prof.currency_inr += req.currency_delta_inr
        prof.experience_xp += req.xp_delta
        prof.level = 1 + (prof.experience_xp // 1000)

        if req.reward_item_id and req.reward_item_id not in prof.inventory_items:
            prof.inventory_items.append(req.reward_item_id)

        repo.upsert_profile(prof)
        repo.transactions_log.append({
            "type": "MISSION_REWARD",
            "account_id": req.account_id,
            "mission_id": req.mission_id,
            "currency_delta_inr": req.currency_delta_inr,
            "xp_delta": req.xp_delta
        })
        return PlayerProfileResponse(**prof.__dict__)

    @staticmethod
    def server_record_combat_result(req: ServerCombatResultRequest) -> Dict[str, object]:
        killer = repo.get_profile(req.killer_account_id)
        victim = repo.get_profile(req.victim_account_id)
        if not killer or not victim:
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Killer or victim profile not found")

        killer.kills += 1
        killer.experience_xp += 250
        if req.is_match_victory:
            killer.br_victories += 1
            killer.currency_inr += 25000
            killer.experience_xp += 2000
        killer.level = 1 + (killer.experience_xp // 1000)

        victim.deaths += 1
        repo.upsert_profile(killer)
        repo.upsert_profile(victim)
        return {
            "match_id": req.match_id,
            "killer_kills": killer.kills,
            "killer_br_victories": killer.br_victories,
            "victim_deaths": victim.deaths
        }

    @staticmethod
    def purchase_vehicle(req: VehiclePurchaseRequest) -> PlayerProfileResponse:
        prof = repo.get_profile(req.account_id)
        if not prof:
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Player profile not found")
        if req.vehicle_id in prof.owned_vehicles:
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Vehicle already owned in garage")
        if prof.currency_inr < req.price_inr:
            raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Insufficient INR balance")

        prof.currency_inr -= req.price_inr
        prof.owned_vehicles.append(req.vehicle_id)
        repo.upsert_profile(prof)
        repo.transactions_log.append({
            "type": "VEHICLE_PURCHASE",
            "account_id": req.account_id,
            "vehicle_id": req.vehicle_id,
            "price_inr": req.price_inr
        })
        return PlayerProfileResponse(**prof.__dict__)

    @staticmethod
    def create_party(req: PartyCreateRequest) -> Dict[str, object]:
        party_id = f"party_hyd_{len(repo.parties) + 1}"
        repo.parties[party_id] = [req.leader_account_id]
        return {"party_id": party_id, "members": repo.parties[party_id], "max_members": req.max_members}

    @staticmethod
    def enqueue_matchmaking(req: MatchmakingEnqueueRequest) -> Dict[str, object]:
        repo.matchmaking_queue.append({"account_id": req.account_id, "mode": req.mode, "region": req.region})
        server_port = 7777 + len(repo.allocated_servers)
        server_info = {
            "server_id": f"ds_hyd_{req.mode.lower()}_{len(repo.allocated_servers) + 1}",
            "mode": req.mode,
            "region": req.region,
            "Map": "/Game/Maps/Hyderabad/L_Hyderabad_Prototype",
            "endpoint": f"127.0.0.1:{server_port}",
            "max_players": 100 if req.mode.startswith("BR_") else 16
        }
        repo.allocated_servers.append(server_info)
        return {"status": "MATCH_ALLOCATED", "server": server_info}
