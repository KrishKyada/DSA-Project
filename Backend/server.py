from flask import Flask, request, Response, jsonify, send_from_directory
from flask_cors import CORS
import subprocess, os

app = Flask(__name__, static_folder="../static", static_url_path="")
CORS(app)

# Path to binary
BINARY_PATH = "./plagiarism_core.exe"

@app.route("/")
def index():
    return send_from_directory(app.static_folder, "index.html")

@app.route("/compare", methods=["POST"])
def compare():
    data = request.get_json()
    codeA = data.get("codeA", "")
    codeB = data.get("codeB", "")
    win = data.get("window", 4)

    lenA = len(codeA.encode())
    lenB = len(codeB.encode())

    header = f"A {lenA}\nB {lenB}\n"
    input_bytes = header.encode() + codeA.encode() + codeB.encode()

    env = os.environ.copy()
    env["WINDOW"] = str(win)

    p = subprocess.run(
        [BINARY_PATH],
        input=input_bytes,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env
    )

    output = p.stdout.decode()
    return Response(output, mimetype="application/json")

# Serve css/js
@app.route("/<path:filename>")
def serve_static(filename):
    return send_from_directory(app.static_folder, filename)

if __name__ == "__main__":
    app.run(debug=True)
