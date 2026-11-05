import sqlite3
import json
from typing import Optional, Dict, List
from app.models.domain import PlayerProfileRecord

class PersistentRepository:
    """
    PostgreSQL / SQLite-compatible transactional persistence layer + in-memory
    Redis-equivalent session/party/matchmaking state store.
    """
    def __init__(self) -> None:
        self.conn = sqlite3.connect(":memory:", check_same_thread=False)
        self._init_schema()
        self.parties: Dict[str, List[str]] = {}
        self.matchmaking_queue: List[Dict[str, str]] = []
        self.allocated_servers: List[Dict[str, object]] = []
        self.transactions_log: List[Dict[str, object]] = []

    def _init_schema(self) -> None:
        cur = self.conn.cursor()
        cur.execute("""
            CREATE TABLE IF NOT EXISTS player_profiles (
                account_id TEXT PRIMARY KEY,
                display_name TEXT NOT NULL,
                currency_inr INTEGER NOT NULL,
                experience_xp INTEGER NOT NULL,
                level INTEGER NOT NULL,
                kills INTEGER NOT NULL,
                deaths INTEGER NOT NULL,
                br_victories INTEGER NOT NULL,
                current_district TEXT NOT NULL,
                inventory_json TEXT NOT NULL,
                vehicles_json TEXT NOT NULL,
                missions_json TEXT NOT NULL
            )
        """)
        self.conn.commit()

    def upsert_profile(self, rec: PlayerProfileRecord) -> PlayerProfileRecord:
        cur = self.conn.cursor()
        cur.execute("""
            INSERT INTO player_profiles (
                account_id, display_name, currency_inr, experience_xp, level,
                kills, deaths, br_victories, current_district,
                inventory_json, vehicles_json, missions_json
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            ON CONFLICT(account_id) DO UPDATE SET
                display_name=excluded.display_name,
                currency_inr=excluded.currency_inr,
                experience_xp=excluded.experience_xp,
                level=excluded.level,
                kills=excluded.kills,
                deaths=excluded.deaths,
                br_victories=excluded.br_victories,
                current_district=excluded.current_district,
                inventory_json=excluded.inventory_json,
                vehicles_json=excluded.vehicles_json,
                missions_json=excluded.missions_json
        """, (
            rec.account_id, rec.display_name, rec.currency_inr, rec.experience_xp, rec.level,
            rec.kills, rec.deaths, rec.br_victories, rec.current_district,
            json.dumps(rec.inventory_items), json.dumps(rec.owned_vehicles), json.dumps(rec.completed_missions)
        ))
        self.conn.commit()
        return rec

    def get_profile(self, account_id: str) -> Optional[PlayerProfileRecord]:
        cur = self.conn.cursor()
        cur.execute("SELECT * FROM player_profiles WHERE account_id = ?", (account_id,))
        row = cur.fetchone()
        if not row:
            return None
        return PlayerProfileRecord(
            account_id=row[0],
            display_name=row[1],
            currency_inr=row[2],
            experience_xp=row[3],
            level=row[4],
            kills=row[5],
            deaths=row[6],
            br_victories=row[7],
            current_district=row[8],
            inventory_items=json.loads(row[9]),
            owned_vehicles=json.loads(row[10]),
            completed_missions=json.loads(row[11]),
        )

    def get_leaderboard(self, limit: int = 10) -> List[PlayerProfileRecord]:
        cur = self.conn.cursor()
        cur.execute("SELECT account_id FROM player_profiles ORDER BY experience_xp DESC, kills DESC LIMIT ?", (limit,))
        rows = cur.fetchall()
        return [self.get_profile(r[0]) for r in rows if self.get_profile(r[0]) is not None]

repo = PersistentRepository()
