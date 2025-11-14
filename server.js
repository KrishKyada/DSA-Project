const express = require("express");
const cors = require("cors");
const bodyParser = require("body-parser");
const { spawn } = require("child_process");
const path = require("path");

const app = express();
const port = 5000;

// Middleware
app.use(cors());
app.use(bodyParser.json());

// Serve static files from public directory
app.use(express.static(path.join(__dirname, "public")));

// Handle favicon requests
app.get("/favicon.ico", (req, res) => {
  res.status(204).end();
});

// Path to the C++ binary
const binaryPath = path.join(__dirname, "Backend", "plagiarism_core");

// Root route - serve index.html
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "public", "index.html"));
});

// Compare endpoint
app.post("/compare", (req, res) => {
  const { codeA, codeB, window } = req.body;
  
  if (!codeA || !codeB) {
    return res.status(400).json({ error: "Missing codeA or codeB" });
  }

  const win = window || 4;
  const lenA = Buffer.byteLength(codeA, "utf8");
  const lenB = Buffer.byteLength(codeB, "utf8");

  const header = `A ${lenA}\nB ${lenB}\n`;
  const inputData = Buffer.concat([
    Buffer.from(header, "utf8"),
    Buffer.from(codeA, "utf8"),
    Buffer.from(codeB, "utf8"),
  ]);

  // Spawn the C++ binary process
  const childProcess = spawn(binaryPath, {
    env: {
      ...process.env,
      WINDOW: String(win),
    },
  });

  let output = "";
  let errorOutput = "";

  childProcess.stdout.on("data", (data) => {
    output += data.toString();
  });

  childProcess.stderr.on("data", (data) => {
    errorOutput += data.toString();
  });

  childProcess.on("close", (code) => {
    if (code !== 0) {
      return res.status(500).json({
        error: "C++ binary execution failed",
        details: errorOutput,
      });
    }

    // Parse and return JSON response
    try {
      const result = JSON.parse(output);
      return res.json(result);
    } catch (e) {
      // If output is not JSON, return as-is
      res.setHeader("Content-Type", "application/json");
      return res.send(output);
    }
  });

  // Send input data to the C++ binary
  childProcess.stdin.write(inputData);
  childProcess.stdin.end();

  // Timeout after 30 seconds
  setTimeout(() => {
    if (!childProcess.killed) {
      childProcess.kill();
      return res.status(500).json({ error: "Process timeout" });
    }
  }, 30000);
});

// Serve static files (CSS, JS, etc.)
app.get("/:filename", (req, res) => {
  res.sendFile(path.join(__dirname, "public", req.params.filename));
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
