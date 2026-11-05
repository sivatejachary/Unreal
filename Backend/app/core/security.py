import hashlib
import hmac
from fastapi import Header, HTTPException, status
from app.core.config import settings

def compute_server_hmac(endpoint_path: str, payload_canonical: str, secret: str = settings.SERVER_HMAC_SECRET) -> str:
    msg = f"{endpoint_path}|{payload_canonical}".encode("utf-8")
    return hmac.new(secret.encode("utf-8"), msg, hashlib.sha256).hexdigest()

def verify_dedicated_server_signature(
    endpoint_path: str,
    payload_canonical: str,
    signature_header: str
) -> None:
    """
    Enforces Rule 10: Never allow the client to independently award currency,
    items, experience, kills, or match victories. Only authoritative Unreal
    dedicated/listen servers holding SERVER_HMAC_SECRET can sign these mutations.
    """
    if not signature_header:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Client-direct mutation forbidden: missing authoritative server HMAC signature."
        )
    expected = compute_server_hmac(endpoint_path, payload_canonical)
    if not hmac.compare_digest(expected, signature_header):
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Client-direct mutation forbidden: invalid authoritative server HMAC signature."
        )
