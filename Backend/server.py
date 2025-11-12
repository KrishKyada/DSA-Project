# app.py — Commit 1: Flask server + static routes

from flask import Flask, request, jsonify, send_from_directory

app = Flask(__name__, static_folder="../frontend")

# -------- Serve frontend (optional) --------
@app.route("/")
def index():
    return send_from_directory(app.static_folder, "index.html")

@app.route("/<path:path>")
def static_files(path):
    return send_from_directory(app.static_folder, path)

# -------- API --------
@app.route("/compare", methods=["POST"])
def compare_codes():
    payload = request.get_json(force=True, silent=True) or {}
    codeA = payload.get("codeA", "")
    codeB = payload.get("codeB", "")
    window = payload.get("window", 4)

    if not codeA.strip() or not codeB.strip():
        return jsonify({"error": "One or both code inputs are empty.", "jaccard": 0.0})

    # Placeholder — actual comparison logic will come in next commit
    return jsonify({"message": "Comparison logic not implemented yet."})

if __name__ == "__main__":
    # Bind explicitly so you can open it in browser as http://127.0.0.1:8080
    app.run(host="127.0.0.1", port=8080, debug=True)
