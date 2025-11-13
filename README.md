# 🕵️‍♂️ Plagiarism Checker (DSA Project)

A compact demo that uses a native C++ core to compare two code/text blobs for similarity and a minimal Flask web UI to run comparisons. The Python server forwards requests to the compiled C++ binary and returns the results.

Live demo (local): compile the C++ core, run the server, then open the frontend in your browser.

---

## 📋 Quick overview

- Backend core: C++ implementation (`plagiarism_core.*`) that performs code/text comparison.
- Server: `Backend/server.py` — Flask bridge that accepts requests and invokes the compiled C++ binary.
- Frontend: `Frontend/index.html` — minimal UI to submit two code snippets and view results.

This README describes how to compile, run, and use the plagiarism checker with the files present in this repo.

---

## 📁 Project structure

```
DSA-Project/
├── Backend/
│   ├── plagiarism_core.cpp
│   ├── plagiarism_core.h
│   ├── plagiarism_core        # compiled binary (optional)
│   └── server.py
└── Frontend/
   └── index.html
```

> Note: If you have additional frontend assets (CSS/JS) they should live under `Frontend/` alongside `index.html`.

---

## 🛠️ Tech stack

- Backend core: C++ (plagiarism/comparison logic)
- Server layer: Python 3 + Flask (simple HTTP bridge)
- Frontend: HTML + vanilla JavaScript
- Data / I/O: binary stdin/stdout communication between server and C++ core

---


## 🚀 Quick start (Linux)

1. Compile the C++ core

   Compile the C++ source into a binary named `plagiarism_core` placed in `Backend/`:

   ```bash
   cd Backend
   g++ -std=c++23 -O2 plagiarism_core.cpp -o plagiarism_core
   chmod +x plagiarism_core
   ```

2. Install Python dependencies

   Create a virtual environment and install Flask (recommended):

   ```bash
   cd Backend
   python3 -m venv .venv
   source .venv/bin/activate
   pip install --upgrade pip
   pip install flask flask-cors
   ```

3. Start the Flask server

   ```bash
   cd Backend
   python3 server.py
   ```

   By default the server runs on `127.0.0.1:5000` and serves `Frontend/index.html`.

4. Open the frontend

   Open your browser to:

   ```
   http://127.0.0.1:5000/
   ```

   Note: If you prefer to serve the frontend separately, you can run a static server in the `Frontend/` folder (for example, `python3 -m http.server 5000`) and open the served page.

   ### Quick start (Windows)

   Option A — MSYS2 / MinGW (recommended if you have GNU toolchain):

   1. Install MSYS2 and open the MinGW64 shell. Install the toolchain:

   ```powershell
   pacman -Syu
   pacman -S mingw-w64-x86_64-toolchain
   ```

   2. Compile the C++ core (inside the MinGW64 shell):

   ```bash
   cd /c/path/to/DSA-Project/Backend
   g++ -std=c++23 -O2 plagiarism_core.cpp -o plagiarism_core.exe
   ```

   3. Edit `Backend/server.py` to point to the `.exe` on Windows (one-line change):

   Open `Backend/server.py` and change the binary path line near the top to:

   ```python
   BINARY_PATH = os.path.join(SCRIPT_DIR, "plagiarism_core.exe")
   ```

   Option B — Visual Studio (MSVC):

   1. Open "x64 Native Tools Command Prompt for VS" and compile with cl:

   ```powershell
   cl /std:c++23 /O2 plagiarism_core.cpp /Fe:plagiarism_core.exe
   ```

   2. Same as above: ensure `Backend/server.py` references `plagiarism_core.exe`.

   Python environment & server (Windows)

   1. Create and activate venv (PowerShell):

   ```powershell
   cd C:\path\to\DSA-Project\Backend
   python -m venv .venv
   .\.venv\Scripts\Activate.ps1   # PowerShell
   # or for cmd.exe: .\.venv\Scripts\activate.bat
   pip install --upgrade pip
   pip install flask flask-cors
   ```

   2. Run the server (PowerShell or cmd):

   ```powershell
   python server.py
   ```

   3. Open the frontend:

   ```
   http://127.0.0.1:5000/
   ```

   Notes:
   - If you compiled `plagiarism_core.exe`, you must update `server.py` to point to that filename on Windows (see above). Alternatively, you can rename the `.exe` to `plagiarism_core` but keeping the `.exe` and updating `server.py` is clearer.
   - Use the MinGW shell when running gcc-built binaries if you encounter runtime/locale issues.

---

## 🔌 Server details

`Backend/server.py` is a small Flask app that serves the frontend and forwards compare requests to the compiled C++ core.

- Static frontend directory: `../Frontend` (served at `/`) — visiting `/` returns `index.html`.
- Binary path (built-in): `os.path.join(SCRIPT_DIR, "plagiarism_core")` — the binary should be named `plagiarism_core` and placed in the `Backend/` directory.

Endpoint implemented by `server.py`:

- `POST /compare`
   - Expects a JSON body: `{ "codeA": "...", "codeB": "...", "window": <int> }`
   - `codeA` and `codeB` are the two source texts to compare (strings).
   - `window` is optional and passed to the binary via the `WINDOW` environment variable.
   - The server builds a byte stream (header + code blobs) and runs the binary, returning its stdout as the HTTP response.

Static file serving:

- `GET /` → serves `Frontend/index.html`
- `GET /<filename>` → serves static file from `Frontend/` (e.g., CSS or JS if present)

Example cURL:

```bash
curl -X POST http://127.0.0.1:5000/compare \
   -H "Content-Type: application/json" \
   -d '{"codeA":"print(1)","codeB":"print(2)","window":4}'
```

Note: `server.py` runs Flask in debug mode by default which listens on `127.0.0.1:5000`.

---

## 📖 Usage guide (UI)

- Open the UI and paste or type the two code snippets you want to compare.
- Click the action button (Compare / Submit) to send both snippets to the server.
- The server will return the C++ core's output (similarity/metrics) and the UI will display it.

Check the Flask server console for incoming requests and for any stderr from the binary.

---

## ⚙️ Notes on building and configuration

- If `server.py` executes the C++ binary, it may reference an absolute path. Edit `server.py` to point to the local binary path (e.g., `./social_graph_core`).
- If your backend requires a CSV dataset (e.g., `dataset/users.csv`), create that file under `Backend/` and ensure `server.py` or the C++ binary reads from the correct path.

Common change (edit inside `server.py` if present):

```python
# Example: adjust these constants to your layout
BACKEND_BINARY = './social_graph_core'
DATA_CSV = './dataset/users.csv'
```

---

## 🐛 Troubleshooting

- Server port in use: change the port in `server.py` or stop the other process.
- Binary not found: compile the C++ sources and ensure the binary path matches the path used in `server.py`.
- Frontend fetches fail when opened via `file://`: serve the folder over HTTP (see Quick Start).

---

## 🎓 Learning outcomes

- Native-code integration: calling a compiled C++ program from a Python server
- Text similarity and plagiarism detection heuristics (sliding-window comparisons, matching substrings)
- Building a minimal full-stack demo with a static frontend, server bridge, and native core

---

## 📝 License

MIT License — feel free to reuse and modify for coursework.

---

If you want, I can also:

1. Make `server.py` accept a configurable `BINARY_PATH` env var (so the binary can live elsewhere).
2. Produce a tiny test script to exercise `/compare` and validate end-to-end behavior.
3. Add a short note in `README.md` explaining how to run the server on a different port.

Tell me which of those you'd like next.
