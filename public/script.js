// Theme Toggle
const themeToggle = document.getElementById('themeToggle');
const body = document.body;

const currentTheme = localStorage.getItem('theme') || 'dark';
body.setAttribute('data-theme', currentTheme);
themeToggle.textContent = currentTheme === 'dark' ? '🌙' : '☀';

themeToggle.addEventListener('click', () => {
  const newTheme = body.getAttribute('data-theme') === 'dark' ? 'light' : 'dark';
  body.setAttribute('data-theme', newTheme);
  localStorage.setItem('theme', newTheme);
  themeToggle.textContent = newTheme === 'dark' ? '🌙' : '☀';
});

// Initialize CodeMirror editors
const editorA = CodeMirror.fromTextArea(document.getElementById('codeA'), {
  mode: 'text/x-c++src',
  theme: 'default',
  lineNumbers: true,
  lineWrapping: true
});

const editorB = CodeMirror.fromTextArea(document.getElementById('codeB'), {
  mode: 'text/x-c++src',
  theme: 'default',
  lineNumbers: true,
  lineWrapping: true
});

// DOM elements
const analyzeBtn = document.getElementById('analyzeBtn');
const clearBtn = document.getElementById('clearBtn');
const windowInput = document.getElementById('windowInput');
const statusMsg = document.getElementById('statusMsg');

// Result elements
const finalScore = document.getElementById('finalScore');
const progressFill = document.getElementById('progressFill');
const tokensA = document.getElementById('tokensA');
const tokensB = document.getElementById('tokensB');
const jaccardValue = document.getElementById('jaccardValue');
const editValue = document.getElementById('editValue');
const astValue = document.getElementById('astValue');
const pdgValue = document.getElementById('pdgValue');
const majorityValue = document.getElementById('majorityValue');
const minorityValue = document.getElementById('minorityValue');

// Sample code
const sampleCodeA = `int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}`;
const sampleCodeB = `int fact(int num) {
    if (num <= 1) return 1;
    return num * fact(num - 1);
}`;

editorA.setValue(sampleCodeA);
editorB.setValue(sampleCodeB);

// Token counting
function updateTokenCount() {
  const codeA = editorA.getValue().trim();
  const codeB = editorB.getValue().trim();

  const tokensACount = codeA ? tokenize(codeA).length : 0;
  const tokensBCount = codeB ? tokenize(codeB).length : 0;

  tokensA.textContent = `Tokens: ${tokensACount}`;
  tokensB.textContent = `Tokens: ${tokensBCount}`;
}

// Simple tokenizer
function tokenize(code) {
  const tokens = [];
  let current = '';

  for (let i = 0; i < code.length; i++) {
    const c = code[i];
    if (/[a-zA-Z0-9_]/.test(c)) {
      current += c;
    } else {
      if (current) {
        tokens.push(current);
        current = '';
      }
      if (!/\s/.test(c)) {
        tokens.push(c);
      }
    }
  }

  if (current) tokens.push(current);
  return tokens;
}

editorA.on('change', updateTokenCount);
editorB.on('change', updateTokenCount);

updateTokenCount();

// Analyze button
analyzeBtn.addEventListener('click', async () => {
  const codeA = editorA.getValue().trim();
  const codeB = editorB.getValue().trim();

  if (!codeA || !codeB) {
    setStatus("⚠ Please enter code in both editors", "error");
    return;
  }

  analyzeBtn.disabled = true;
  setStatus(`<span class="loading-spinner"></span>Analyzing code...`, "loading");

  try {
    const window = parseInt(windowInput.value);

    const response = await fetch('/compare', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ codeA, codeB, window })
    });

    if (!response.ok) {
      throw new Error(`Server error: ${response.status}`);
    }

    let result;
    const text = await response.text();

    try {
      result = JSON.parse(text);
    } catch (e) {
      const fixed = text
        .replace(/\n\s*"FinalScore"/, ',\n  "FinalScore"')
        .replace(/,(\s*[}\]])/g, '$1');
      result = JSON.parse(fixed);
    }

    if (result.error) throw new Error(result.error);

    displayResults(result);
    setStatus("✅ Analysis complete!", "success");
    
  } catch (error) {
    console.error("Analysis error:", error);
    setStatus(`❌ Error: ${error.message}`, "error");
  } finally {
    analyzeBtn.disabled = false;
  }
});

// Clear button
clearBtn.addEventListener('click', () => {
  editorA.setValue('');
  editorB.setValue('');
  resetResults();
  setStatus("Ready to analyze code", "");
});

// Display results
function displayResults(data) {
  tokensA.textContent = `Tokens: ${data.tokensA || '—'}`;
  tokensB.textContent = `Tokens: ${data.tokensB || '—'}`;

  jaccardValue.textContent = formatScore(data.jaccard);
  editValue.textContent = formatScore(data.editSimilarity);
  astValue.textContent = formatScore(data.astStructural);
  pdgValue.textContent = formatScore(data.pdgSimilarity);
  majorityValue.textContent = formatScore(data.majorityScore);
  minorityValue.textContent = formatScore(data.minorityScore);

  const score = data.FinalScore || 0;
  animateScore(score);
  progressFill.style.width = `${score * 100}%`;
}

// Score animation
function animateScore(targetScore) {
  const duration = 1500;
  const fps = 60;
  const totalFrames = (duration / 1000) * fps;
  let currentFrame = 0;

  const interval = setInterval(() => {
    currentFrame++;
    const ease = 1 - Math.pow(1 - currentFrame / totalFrames, 3);
    finalScore.textContent = formatScore(targetScore * ease);

    if (currentFrame >= totalFrames) {
      clearInterval(interval);
      finalScore.textContent = formatScore(targetScore);
    }
  }, 1000 / fps);
}

function formatScore(value) {
  if (value == null) return "—";
  return (value * 100).toFixed(2) + "%";
}

// Reset UI
function resetResults() {
  tokensA.textContent = "Tokens: —";
  tokensB.textContent = "Tokens: —";
  finalScore.textContent = "—";
  progressFill.style.width = "0%";

  jaccardValue.textContent = '—';
  editValue.textContent = '—';
  astValue.textContent = '—';
  pdgValue.textContent = '—';
  majorityValue.textContent = '—';
  minorityValue.textContent = '—';
}

function setStatus(msg, type) {
  statusMsg.innerHTML = msg;
  statusMsg.className = "status " + type;
}
