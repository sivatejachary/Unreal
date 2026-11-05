import os

class Settings:
    PROJECT_NAME: str = "INDIA UNLEASHED Authoritative Backend API"
    VERSION: str = "0.1.0-alpha"
    SERVER_HMAC_SECRET: str = os.getenv("IU_SERVER_HMAC_SECRET", "iu_dedicated_server_master_secret_2026")
    JWT_SECRET: str = os.getenv("IU_JWT_SECRET", "iu_player_jwt_secret_2026")
    SQLITE_FALLBACK_PATH: str = os.getenv("IU_SQLITE_PATH", ":memory:")

settings = Settings()
