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

const analyzeBtn = document.getElementById('analyzeBtn');
const clearBtn = document.getElementById('clearBtn');
const windowInput = document.getElementById('windowInput');
const statusMsg = document.getElementById('statusMsg');
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

const examples = {
  factorial: {
    a: `int factorial(int n) {
  if (n <= 1) return 1;
  return n * factorial(n - 1);
}`,
    b: `int fact(int num) {
  if (num <= 1) return 1;
  return num * fact(num - 1);
}`
  },
  sorting: {
    a: `void bubbleSort(vector<int>& arr) {
  int n = arr.size();
  for (int i = 0; i < n - 1; ++i) {
    for (int j = 0; j < n - i - 1; ++j) {
      if (arr[j] > arr[j + 1]) {
        swap(arr[j], arr[j + 1]);
      }
    }
  }
}`,
    b: `void selectionSort(vector<int>& arr) {
  int n = arr.size();
  for (int i = 0; i < n - 1; ++i) {
    int minIdx = i;
    for (int j = i + 1; j < n; ++j) {
      if (arr[j] < arr[minIdx]) {
        minIdx = j;
      }
    }
    swap(arr[i], arr[minIdx]);
  }
}`
  },
  strings: {
    a: `def normalize_phrase(text):
  words = text.lower().split()
  return " ".join(word.strip(".,?!") for word in words)

def count_letters(text):
  return sum(1 for ch in text if ch.isalpha())`,
    b: `def prepare_phrase(content):
  tokens = content.lower().split()
  return " ".join(piece.strip(".,?!") for piece in tokens)

def count_alpha(text):
  return sum(1 for ch in text if ch.isalpha())`
  }
};

loadExample('factorial');
editorA.on('change', updateTokens);
editorB.on('change', updateTokens);

function updateTokens() {
  tokensA.textContent = `Tokens: ${countTokens(editorA.getValue())}`;
  tokensB.textContent = `Tokens: ${countTokens(editorB.getValue())}`;
}

function countTokens(code) {
  const trimmed = code.trim();
  if (!trimmed) return 0;
  const tokens = [];
  let current = '';
  for (const ch of trimmed) {
    if (/[a-zA-Z0-9_]/.test(ch)) {
      current += ch;
    } else {
      if (current) {
        tokens.push(current);
        current = '';
      }
      if (!/\s/.test(ch)) {
        tokens.push(ch);
      }
    }
  }
  if (current) tokens.push(current);
  return tokens.length;
}

function loadExample(key) {
  const preset = examples[key];
  if (!preset) return;
  editorA.setValue(preset.a);
  editorB.setValue(preset.b);
  updateTokens();
  setStatus(`Loaded ${key} example.`, 'success');
}

document.querySelectorAll('.example-btn').forEach(btn => {
  btn.addEventListener('click', () => loadExample(btn.dataset.example));
});

analyzeBtn.addEventListener('click', async () => {
  const codeA = editorA.getValue().trim();
  const codeB = editorB.getValue().trim();
  if (!codeA || !codeB) {
    setStatus('Please enter code in both editors', 'error');
    return;
  }

  analyzeBtn.disabled = true;
  setStatus('<span class="loading-spinner"></span>Analyzing code...', 'loading');

  try {
    const window = parseInt(windowInput.value, 10);
    const response = await fetch('/compare', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ codeA, codeB, window })
    });

    if (!response.ok) {
      throw new Error(`Server error: ${response.status}`);
    }

    const raw = await response.text();
    const data = safeParse(raw);
    if (!data) throw new Error('Invalid JSON from server');
    if (data.error) throw new Error(data.error);

    displayResults(data);
    setStatus('Analysis complete!', 'success');
  } catch (error) {
    console.error('Analysis error:', error);
    setStatus(`Error: ${error.message}`, 'error');
  } finally {
    analyzeBtn.disabled = false;
  }
});

clearBtn.addEventListener('click', () => {
  editorA.setValue('');
  editorB.setValue('');
  resetResults();
  setStatus('Ready to analyze', '');
});

function safeParse(text) {
  try {
    return JSON.parse(text);
  } catch {
    try {
      const cleaned = text
        .replace(/\n\s*"FinalScore"/, ',\n  "FinalScore"')
        .replace(/,(\s*[}\]])/g, '$1');
      return JSON.parse(cleaned);
    } catch {
      return null;
    }
  }
}

function displayResults(data) {
  tokensA.textContent = `Tokens: ${data.tokensA ?? '—'}`;
  tokensB.textContent = `Tokens: ${data.tokensB ?? '—'}`;
  jaccardValue.textContent = formatScore(data.jaccard);
  editValue.textContent = formatScore(data.editSimilarity);
  astValue.textContent = formatScore(data.astStructural);
  pdgValue.textContent = formatScore(data.pdgSimilarity);
  majorityValue.textContent = formatScore(data.majorityScore);
  minorityValue.textContent = formatScore(data.minorityScore);

  const score = typeof data.FinalScore === 'number' ? data.FinalScore : 0;
  finalScore.textContent = formatScore(score);
  progressFill.style.width = `${Math.max(0, Math.min(1, score)) * 100}%`;
}

function formatScore(value) {
  if (typeof value !== 'number' || Number.isNaN(value)) return '—';
  return (value * 100).toFixed(2) + '%';
}

function resetResults() {
  tokensA.textContent = 'Tokens: —';
  tokensB.textContent = 'Tokens: —';
  finalScore.textContent = '—';
  progressFill.style.width = '0%';
  jaccardValue.textContent = '—';
  editValue.textContent = '—';
  astValue.textContent = '—';
  pdgValue.textContent = '—';
  majorityValue.textContent = '—';
  minorityValue.textContent = '—';
}

function setStatus(msg, type) {
  statusMsg.innerHTML = msg;
  statusMsg.className = 'status';
  if (type) statusMsg.classList.add(type);
}
