# 🌐 Social Graph Friend Recommender (DSA Project)

A compact full-stack project that demonstrates graph algorithms, prefix search and friend recommendations with a small web UI. The project combines a C++ core for graph operations and a simple server layer alongside a lightweight frontend.

Live demo (local): start the server and open the frontend in your browser.

---

## 📋 Quick overview

- Backend core: C++ implementation (plagiarism_core.*) that contains graph/search algorithms and utilities.
- Server: `Backend/server.py` — provides a bridge between HTTP requests and the compiled C++ binary (or executes the C++ program in silent/CLI mode).
- Frontend: `Frontend/index.html` — simple web UI to interact with the server.

This README adapts the full template to the actual files in this repository.

---

## 📁 Project structure

```
/home/elarion/Desktop/DSA-Project/
├── Backend/
│   ├── plagiarism_core.cpp
│   ├── plagiarism_core.h
│   └── server.py
└── Frontend/
    └── index.html
```

> Note: If you have additional frontend assets (CSS/JS) they should live under `Frontend/` alongside `index.html`.

---

## 🛠️ Tech stack

- Backend core: C++ (graph algorithms, trie, PageRank style scoring)
- Server layer: Python (the provided `server.py` maps HTTP requests to backend operations)
- Frontend: HTML (vanilla JS in `index.html`)
- Data persistence: CSV files (if present/used by the backend)

---

## 🚀 Quick start (Linux)

1. Build the C++ backend

   Use the provided `Makefile` to build the backend binary. From the `Backend/` folder run:

   ```bash
   cd /home/elarion/Desktop/DSA-Project/Backend
   make
   ```

   The Makefile compiles `plagiarism_core.cpp` and produces the binary `plagiarism_core` in `Backend/`.

   If you prefer to compile manually with g++, you can run:

   ```bash
   g++ -std=c++23 -O2 plagiarism_core.cpp -o plagiarism_core
   ```

2. Start the server

   The repository includes `Backend/server.py`. Run it with Python 3:

   ```bash
   cd /home/elarion/Desktop/DSA-Project/Backend
   python3 server.py
   ```

   The server should expose HTTP endpoints (see Server details below). `server.py` expects the backend binary to be named `plagiarism_core` and located in the same `Backend/` folder (the script builds `BINARY_PATH` as `os.path.join(SCRIPT_DIR, "plagiarism_core")`).

3. Open the frontend

   - Option A (recommended): Serve the frontend directory and open in browser (avoids file:// issues):

     ```bash
     cd /home/elarion/Desktop/DSA-Project/Frontend
     python3 -m http.server 5000
     # then open http://localhost:5000 in your browser
     ```

   - Option B: Open `Frontend/index.html` directly in the browser (some fetch requests may be blocked by CORS/file:// restrictions).

---

## 🔌 Server details (actual)

The provided `Backend/server.py` is a small Flask app that serves the frontend and forwards a specific operation to the compiled C++ binary. Key details discovered from the script:

- Static frontend directory: `../Frontend` (served at `/`) — visiting `/` returns `index.html`.
- Binary path (built-in): `os.path.join(SCRIPT_DIR, "plagiarism_core")` — the binary should be named `plagiarism_core` and placed in the `Backend/` directory.

Endpoint implemented by `server.py`:

- `POST /compare`
   - Expects a JSON body: `{ "codeA": "...", "codeB": "...", "window": <int> }`
   - `codeA` and `codeB` are the two source texts to compare (strings).
   - `window` is optional (the script sets `WINDOW` environment variable when invoking the binary). Default used in the server is `4` if not provided.
   - The server builds a custom byte stream for the binary: a header with lengths, then the two code blobs. It runs the binary with `WINDOW` in the environment and returns the binary's stdout as the response with `application/json` mimetype.

Static file serving:

- `GET /` → serves `Frontend/index.html`
- `GET /<filename>` → serves static file from `Frontend/` (e.g., CSS or JS if present)

Example cURL for the actual endpoint:

```bash
curl -X POST http://localhost:5000/compare \
   -H "Content-Type: application/json" \
   -d '{"codeA":"print(1)","codeB":"print(2)","window":4}'
```

Note: `server.py` runs Flask in debug mode by default which listens on `127.0.0.1:5000`.

---

## 📖 Usage guide (UI)

- Use the frontend controls to add users, create friendships, search, and request recommendations.
- The frontend issues requests to the server; check the server console for incoming requests and the backend logs for processing output.

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

- Graph algorithms (adjacency lists, BFS)
- Local PageRank-like scoring and recommendation heuristics
- Trie/prefix search for autocomplete
- Simple full-stack integration between compiled code and a web UI via a lightweight server

---

## 📝 License

MIT License — feel free to reuse and modify for coursework.

---

If you want, I can also:

1. Inspect `Backend/server.py` and adapt README instructions to its exact routes and expected binary/CSV paths.
2. Add a minimal `Makefile` or a build script for the C++ binary.
3. Serve the frontend automatically from the server (merge server + static file serving).

Tell me which of those you'd like next.
