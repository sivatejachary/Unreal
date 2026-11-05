import os
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from app.core.config import settings
from app.api.routes import router

app = FastAPI(title=settings.PROJECT_NAME, version=settings.VERSION)
app.include_router(router)

static_dir = os.path.join(os.path.dirname(__file__), "static")
if os.path.exists(static_dir):
    app.mount("/static", StaticFiles(directory=static_dir), name="static")

@app.get("/")
def serve_index():
    index_path = os.path.join(os.path.dirname(__file__), "static", "index.html")
    if os.path.exists(index_path):
        return FileResponse(index_path)
    return {"message": settings.PROJECT_NAME}

@app.get("/character_model.glb")
def serve_character_model():
    glb_path = os.path.join(os.path.dirname(__file__), "static", "character_model.glb")
    if os.path.exists(glb_path):
        return FileResponse(glb_path)
    return {"error": "Character model not found"}

@app.get("/AAA_Female_Rigged.fbx")
def serve_aaa_female_fbx():
    fbx_path = os.path.join(os.path.dirname(__file__), "static", "AAA_Female_Rigged.fbx")
    if os.path.exists(fbx_path):
        return FileResponse(fbx_path, media_type="application/octet-stream")
    return {"error": "AAA Female Rigged FBX not found"}

@app.get("/charminar.glb")
def serve_charminar_model():
    glb_path = os.path.join(os.path.dirname(__file__), "static", "charminar.glb")
    if os.path.exists(glb_path):
        return FileResponse(glb_path)
    return {"error": "Charminar model not found"}

@app.get("/health")
def health_check():
    return {"status": "healthy", "service": settings.PROJECT_NAME, "version": settings.VERSION}
