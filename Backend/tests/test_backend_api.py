import unittest
from fastapi.testclient import TestClient
from app.main import app
from app.core.security import compute_server_hmac

class TestIndiaUnleashedBackendAPI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.client = TestClient(app)

    def test_01_health_and_auth_login(self):
        r_health = self.client.get("/health")
        self.assertEqual(r_health.status_code, 200)
        self.assertEqual(r_health.json()["status"], "healthy")

        r_login = self.client.post("/api/v1/auth/login", json={
            "account_id": "acct_hyd_vikram",
            "display_name": "Vikram_Hyd",
            "platform": "WindowsPC"
        })
        self.assertEqual(r_login.status_code, 200)
        data = r_login.json()
        self.assertEqual(data["account_id"], "acct_hyd_vikram")
        self.assertEqual(data["currency_inr"], 5000)

        # Register Player 2 as well
        self.client.post("/api/v1/auth/login", json={
            "account_id": "acct_hyd_ananya",
            "display_name": "Ananya_Hyd",
            "platform": "Android"
        })

    def test_02_client_cannot_self_award_currency_or_xp(self):
        # Attempt to call /api/v1/missions/complete WITHOUT authoritative server HMAC header
        r_forged = self.client.post("/api/v1/missions/complete", json={
            "account_id": "acct_hyd_vikram",
            "mission_id": "M01_OldCityArrival",
            "currency_delta_inr": 500000,
            "xp_delta": 50000,
            "reward_item_id": "WPN_AR_VAJRA"
        })
        self.assertEqual(r_forged.status_code, 403)

    def test_03_authoritative_server_mission_reward_succeeds(self):
        payload = {
            "account_id": "acct_hyd_vikram",
            "mission_id": "M01_OldCityArrival",
            "currency_delta_inr": 25000,
            "xp_delta": 1500,
            "reward_item_id": "WPN_AR_VAJRA"
        }
        canonical = f"{payload['account_id']}:{payload['mission_id']}:{payload['currency_delta_inr']}:{payload['xp_delta']}"
        sig = compute_server_hmac("/api/v1/missions/complete", canonical)

        r_ok = self.client.post(
            "/api/v1/missions/complete",
            json=payload,
            headers={"X-IU-Server-Signature": sig}
        )
        self.assertEqual(r_ok.status_code, 200)
        prof = r_ok.json()
        self.assertEqual(prof["currency_inr"], 30000) # 5000 initial + 25000 mission reward
        self.assertEqual(prof["level"], 2)
        self.assertIn("WPN_AR_VAJRA", prof["inventory_items"])

    def test_04_vehicle_purchase_and_garage_validation(self):
        # Purchase Deccan Sedan for 18,000 INR
        r_buy = self.client.post("/api/v1/transactions/purchase-vehicle", json={
            "account_id": "acct_hyd_vikram",
            "vehicle_id": "VH_DECCAN_SEDAN_GT",
            "price_inr": 18000
        })
        self.assertEqual(r_buy.status_code, 200)
        self.assertEqual(r_buy.json()["currency_inr"], 12000)
        self.assertIn("VH_DECCAN_SEDAN_GT", r_buy.json()["owned_vehicles"])

    def test_05_authoritative_combat_kill_and_leaderboard(self):
        payload = {
            "match_id": "match_hyd_001",
            "killer_account_id": "acct_hyd_vikram",
            "victim_account_id": "acct_hyd_ananya",
            "is_match_victory": True
        }
        canonical = f"{payload['match_id']}:{payload['killer_account_id']}:{payload['victim_account_id']}:1"
        sig = compute_server_hmac("/api/v1/combat/record-kill", canonical)

        r_kill = self.client.post(
            "/api/v1/combat/record-kill",
            json=payload,
            headers={"X-IU-Server-Signature": sig}
        )
        self.assertEqual(r_kill.status_code, 200)
        self.assertEqual(r_kill.json()["killer_kills"], 1)
        self.assertEqual(r_kill.json()["killer_br_victories"], 1)

        r_lb = self.client.get("/api/v1/leaderboards/global")
        self.assertEqual(r_lb.status_code, 200)
        self.assertGreaterEqual(len(r_lb.json()["entries"]), 2)

    def test_06_parties_and_matchmaking_server_allocation(self):
        r_party = self.client.post("/api/v1/parties/create", json={
            "leader_account_id": "acct_hyd_vikram",
            "max_members": 4
        })
        self.assertEqual(r_party.status_code, 200)

        r_mm = self.client.post("/api/v1/matchmaking/enqueue", json={
            "account_id": "acct_hyd_vikram",
            "mode": "OpenWorld_Coop",
            "region": "ap-south-hyderabad"
        })
        self.assertEqual(r_mm.status_code, 200)
        self.assertEqual(r_mm.json()["status"], "MATCH_ALLOCATED")
        self.assertEqual(r_mm.json()["server"]["Map"], "/Game/Maps/Hyderabad/L_Hyderabad_Prototype")

if __name__ == "__main__":
    unittest.main()
