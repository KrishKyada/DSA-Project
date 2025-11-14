# 🕵️‍♂️ Plagiarism Checker (DSA Project)

A compact demo that uses a native C++ core to compare two code/text blobs for similarity and a minimal Node.js web UI to run comparisons. The Express server forwards requests to the compiled C++ binary and returns the results.

Live demo (local): compile the C++ core, run `npm start`, then open the frontend in your browser.

---

## 📋 Quick overview

- Backend core: C++ implementation (`plagiarism_core.*`) that performs code/text comparison.
- Server: `server.js` — Express bridge that accepts requests and invokes the compiled C++ binary.
- Frontend: `public/` — minimal UI to submit two code snippets and view results (HTML, CSS, JS).

This README describes how to compile, run, and use the plagiarism checker with the files present in this repo.

---

## 📁 Project structure

```
DSA-Project/
├── Backend/
│   ├── plagiarism_core.cpp
│   ├── plagiarism_core.hpp
│   └── plagiarism_core        # compiled binary
├── public/
│   ├── index.html
│   ├── style.css
│   └── script.js
├── package.json
├── server.js
├── README.md
└── node_modules/
```

---

## 🛠️ Tech stack

- Backend core: C++ (plagiarism/comparison logic)
- Server layer: Node.js + Express (simple HTTP bridge)
- Frontend: HTML + vanilla JavaScript + CSS
- Data / I/O: binary stdin/stdout communication between server and C++ core

---


## 🚀 Quick start (Linux / macOS)

1. Compile the C++ core

   Compile the C++ source into a binary named `plagiarism_core` placed in `Backend/`:

   ```bash
   cd Backend
   g++ -std=c++17 -O2 plagiarism_core.cpp -o plagiarism_core
   chmod +x plagiarism_core
   cd ..
   ```

2. Install Node.js dependencies

   ```bash
   npm install
   ```

3. Start the Express server

   ```bash
   npm start
   ```

   By default the server runs on `http://localhost:5000` and serves the `public/` folder.

4. Open the frontend

   Open your browser to:

   ```
   http://localhost:5000/
   ```

## 🚀 Quick start (Windows)

Option A — MSYS2 / MinGW (recommended if you have GNU toolchain):

1. Install MSYS2 and open the MinGW64 shell. Install the toolchain:

   ```powershell
   pacman -Syu
   pacman -S mingw-w64-x86_64-toolchain
   ```

2. Compile the C++ core (inside the MinGW64 shell):

   ```bash
   cd /c/path/to/DSA-Project/Backend
   g++ -std=c++17 -O2 plagiarism_core.cpp -o plagiarism_core.exe
   cd ..
   ```

Option B — Visual Studio (MSVC):

1. Open "x64 Native Tools Command Prompt for VS" and compile with cl:

   ```powershell
   cl /std:c++17 /O2 plagiarism_core.cpp /Fe:plagiarism_core.exe
   ```

Node.js & Express server (Windows)

1. Install Node.js dependencies (PowerShell):

   ```powershell
   npm install
   ```

2. Run the server:

   ```powershell
   npm start
   ```

3. Open the frontend:

   ```
   http://localhost:5000/
   ```

Notes:
- The server automatically detects `.exe` extension on Windows.
- Use the MinGW shell when running gcc-built binaries if you encounter runtime/locale issues.
- If your g++ doesn't support C++17, try C++11 instead: `g++ -std=c++11 -O2 ...`

---

## 🔌 Server details

`server.js` is a small Express app that serves the frontend and forwards compare requests to the compiled C++ core.

- Static frontend directory: `public/` (served at `/`) — visiting `/` returns `index.html`.
- Binary path (built-in): `Backend/plagiarism_core` — the binary should be named `plagiarism_core` (or `plagiarism_core.exe` on Windows) and placed in the `Backend/` directory.

Endpoints implemented by `server.js`:

- `GET /` → serves `public/index.html`
- `POST /compare`
   - Expects a JSON body: `{ "codeA": "...", "codeB": "...", "window": <int> }`
   - `codeA` and `codeB` are the two source texts to compare (strings).
   - `window` is optional and passed to the binary via the `WINDOW` environment variable.
   - The server builds a byte stream (header + code blobs) and runs the binary, returning its stdout as the HTTP response.
- `GET /<filename>` → serves static file from `public/` (e.g., CSS, JS)

Example cURL:

```bash
curl -X POST http://localhost:5000/compare \
   -H "Content-Type: application/json" \
   -d '{"codeA":"print(1)","codeB":"print(2)","window":4}'
```

Note: `server.js` listens on `http://localhost:5000` by default.

---

## 📖 Usage guide (UI)

- Open the UI and paste or type the two code snippets you want to compare.
- Click the action button (Analyze) to send both snippets to the server.
- The server will return the C++ core's output (similarity/metrics) and the UI will display it.

Check the Express server console for incoming requests and for any stderr from the binary.

---

## ⚙️ Notes on building and configuration

- The C++ binary should be compiled and placed in the `Backend/` directory.
- On Windows, the binary should be named `plagiarism_core.exe`; on Linux/macOS, just `plagiarism_core`.
- If your backend requires a CSV dataset (e.g., `dataset/users.csv`), create that file under `Backend/` and ensure the C++ binary reads from the correct path.

To customize the binary path in `server.js`:

```javascript
const binaryPath = path.join(__dirname, "Backend", "plagiarism_core");
```

---

## 🐛 Troubleshooting

- Server port in use: change the port in `server.js` (line with `app.listen()`) or stop the other process.
- Binary not found: compile the C++ sources and ensure the binary path matches the path used in `server.js`.
- Frontend fetches fail: ensure the server is running via `npm start`.
- Node modules missing: run `npm install` to install dependencies.

---

## 🎓 Learning outcomes

- Native-code integration: calling a compiled C++ program from a Node.js server
- Text similarity and plagiarism detection heuristics (sliding-window comparisons, matching substrings)
- Building a minimal full-stack demo with a static frontend, Express bridge, and native core

---

## 📝 License

MIT License — feel free to reuse and modify for coursework.
