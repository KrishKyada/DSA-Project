#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <cstdint>

using namespace std;
// Result struct for pairwise comparison
struct AnalyzeResult {
    int tokensA;
    int tokensB;
    int fpsA;
    int fpsB;
    double jaccard;
    string message;
};

// ============================================================================
// 1. TEXT NORMALIZATION
// ============================================================================
string normalize(const string &code)
{
    string result;
    result.reserve(code.size());
    bool in_line_comment = false;
    bool in_block_comment = false;

    for (size_t i = 0; i < code.size(); i++)
    {
        if (!in_line_comment && !in_block_comment && i + 1 < code.size())
        {
            if (code[i] == '/' && code[i + 1] == '/')
            {
                in_line_comment = true;
                i++;
                continue;
            }
            if (code[i] == '/' && code[i + 1] == '*')
            {
                in_block_comment = true;
                i++;
                continue;
            }
        }

        if (in_line_comment && code[i] == '\n')
        {
            in_line_comment = false;
            result += ' ';
            continue;
        }

        if (in_block_comment && i + 1 < code.size() && code[i] == '*' && code[i + 1] == '/')
        {
            in_block_comment = false;
            i++;
            continue;
        }

        if (in_line_comment || in_block_comment)
            continue;

        if (isspace(code[i]))
        {
            if (!result.empty() && result.back() != ' ')
            {
                result += ' ';
            }
        }
        else
        {
            result += code[i];
        }
    }

    while (!result.empty() && result.back() == ' ')
        result.pop_back();
    while (!result.empty() && result.front() == ' ')
        result.erase(0, 1);

    return result;
}

// ============================================================================
// 2. TOKENIZATION
// ============================================================================
vector<string> tokenize(const string &code)
{
    vector<string> tokens;
    string current;

    for (char c : code)
    {
        if (isalnum(c) || c == '_')
        {
            current += c;
        }
        else
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }
            if (!isspace(c))
            {
                tokens.push_back(string(1, c));
            }
        }
    }

    if (!current.empty())
    {
        tokens.push_back(current);
    }

    return tokens;
}

// ============================================================================
// 3. ROLLING HASH FINGERPRINTS (Winnowing)
// ============================================================================
vector<uint64_t> computeFingerprints(const vector<string> &tokens, int window_size)
{
    if (tokens.empty() || window_size <= 0)
        return {};

    const uint64_t BASE = 911382323ULL;
    const uint64_t MOD = 1000000007ULL;

    vector<uint64_t> hashes;
    vector<uint64_t> rolling_hashes;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        uint64_t hash_val = 0;
        for (char c : tokens[i])
        {
            hash_val = (hash_val * BASE + (uint64_t)c) % MOD;
        }
        rolling_hashes.push_back(hash_val);
    }

    if ((int)rolling_hashes.size() < window_size)
        return hashes;

    for (size_t i = 0; i + window_size <= rolling_hashes.size(); i++)
    {
        uint64_t min_hash = rolling_hashes[i];
        for (int j = 1; j < window_size; j++)
        {
            min_hash = min(min_hash, rolling_hashes[i + j]);
        }
        hashes.push_back(min_hash);
    }

    return hashes;
}

// ============================================================================
// 4. JACCARD SIMILARITY
// ============================================================================
double jaccardSimilarity(const vector<uint64_t> &fp1, const vector<uint64_t> &fp2)
{
    if (fp1.empty() && fp2.empty())
        return 1.0;
    if (fp1.empty() || fp2.empty())
        return 0.0;

    unordered_set<uint64_t> set1(fp1.begin(), fp1.end());
    unordered_set<uint64_t> set2(fp2.begin(), fp2.end());

    size_t intersection = 0;
    for (uint64_t val : set1)
    {
        if (set2.count(val))
            intersection++;
    }

    size_t union_size = set1.size() + set2.size() - intersection;
    return union_size > 0 ? (double)intersection / union_size : 0.0;
}
// This function compares two code files by tokenizing them, creating fingerprints,
// and calculating how similar they are using Jaccard similarity (used in plagiarism checkers).

AnalyzeResult analyzePair(const string &codeAraw, const string &codeBraw, int window)
{
    AnalyzeResult result;
    string A = normalize(codeAraw);
    string B = normalize(codeBraw);
    auto tokA = tokenize(A);
    auto tokB = tokenize(B);
    result.tokensA = tokA.size();
    result.tokensB = tokB.size();
    auto fpA = computeFingerprints(tokA, window);
    auto fpB = computeFingerprints(tokB, window);
    result.fpsA = fpA.size();
    result.fpsB = fpB.size();

    result.jaccard = jaccardSimilarity(fpA, fpB);

    result.message = "tokensA=" + to_string(result.tokensA) + ", tokensB=" + to_string(result.tokensB) + ", fpsA=" + to_string(result.fpsA) + ", fpsB=" + to_string(result.fpsB);

    return result;
}

// ============================================================================
// 5. EDIT DISTANCE (Levenshtein) - REPLACEMENT
// ============================================================================
double editSimilarity(const vector<string> &tok1, const vector<string> &tok2)
{
    int n = (int)tok1.size();
    int m = (int)tok2.size();

    if (n == 0 && m == 0)
        return 1.0;
    if (n == 0 || m == 0)
        return 0.0;

    // Use two rolling rows to save memory
    vector<int> prev(m + 1), curr(m + 1);

    for (int j = 0; j <= m; ++j)
        prev[j] = j;

    for (int i = 1; i <= n; ++i)
    {
        curr[0] = i;
        for (int j = 1; j <= m; ++j)
        {
            if (tok1[i - 1] == tok2[j - 1])
            {
                curr[j] = prev[j - 1];
            }
            else
            {
                curr[j] = 1 + min({ prev[j], curr[j - 1], prev[j - 1] });
            }
        }
        prev.swap(curr);
    }

    int distance = prev[m];
    int longer = max(n, m);
    return 1.0 - (double)distance / (double)longer;
}

// ============================================================================
// 6. AST STRUCTURAL SIMILARITY (Simplified) - REPLACEMENT
// ============================================================================
double astStructuralSimilarity(const vector<string> &tok1, const vector<string> &tok2)
{
    // Broader set of structural tokens/operators that matter for structure
    static const unordered_set<string> ast_symbols = {
        "{", "}", "(", ")", "[", "]", ";",
        "if", "else", "for", "while", "switch", "case", "default",
        "return",
        "+", "-", "*", "/", "%", "++", "--",
        "=", "==", "!=", "<", ">", "<=", ">=",
        "&&", "||", "?", ":", ","
    };

    unordered_map<string, int> struct1, struct2;

    for (const string &t : tok1)
    {
        if (ast_symbols.count(t)) struct1[t]++;
    }
    for (const string &t : tok2)
    {
        if (ast_symbols.count(t)) struct2[t]++;
    }

    if (struct1.empty() && struct2.empty()) return 1.0;
    if (struct1.empty() || struct2.empty()) return 0.0;

    // union of keys
    unordered_set<string> all_keys;
    for (auto &p : struct1) all_keys.insert(p.first);
    for (auto &p : struct2) all_keys.insert(p.first);

    int common = 0, total = 0;
    for (const string &key : all_keys)
    {
        int c1 = struct1[key];
        int c2 = struct2[key];
        common += min(c1, c2);
        total += max(c1, c2);
    }

    return total > 0 ? (double)common / (double)total : 0.0;
}

// ============================================================================
// 7. PDG (Program Dependence Graph) - Simplified Control Flow
// ============================================================================
double pdgSimilarity(const vector<string> &tok1, const vector<string> &tok2)
{
    auto extractControlFlow = [](const vector<string> &tokens)
    {
        vector<pair<string, int>> flow;
        int depth = 0;  // <-- ADDED

        for (size_t i = 0; i < tokens.size(); i++)
        {
            if (tokens[i] == "{") depth++;              // <-- ADDED
            if (tokens[i] == "}") depth = max(0, depth - 1); // <-- ADDED

            if (tokens[i] == "if" || tokens[i] == "for" ||
                tokens[i] == "while" || tokens[i] == "switch" ||
                tokens[i] == "return")
            {
                flow.push_back({tokens[i], depth});  // <-- FIXED
            }
        }
        return flow;
    };

    vector<pair<string, int>> flow1 = extractControlFlow(tok1);
    vector<pair<string, int>> flow2 = extractControlFlow(tok2);

    if (flow1.empty() && flow2.empty())
        return 1.0;
    if (flow1.empty() || flow2.empty())
        return 0.0;

    int matches = 0;
    size_t min_size = min(flow1.size(), flow2.size());
    for (size_t i = 0; i < min_size; i++)
    {
        if (flow1[i].first == flow2[i].first &&   // <-- CHANGED
            abs(flow1[i].second - flow2[i].second) <= 1)
        {
            matches++;
        }
    }
    return (double)matches / max(flow1.size(), flow2.size());
}


// ============================================================================
// MAIN ANALYSIS
// ============================================================================
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line1, line2;
    if (!getline(cin, line1) || !getline(cin, line2))
    {
        cout << "{\"error\":\"Missing input headers\"}\n";
        return 0;
    }

    size_t pos1 = line1.find(' ');
    size_t pos2 = line2.find(' ');

    if (pos1 == string::npos || pos2 == string::npos)
    {
        cout << "{\"error\":\"Invalid format\"}\n";
        return 0;
    }

    int len_a = stoi(line1.substr(pos1 + 1));
    int len_b = stoi(line2.substr(pos2 + 1));

    string code_a(len_a, '\0');
    string code_b(len_b, '\0');

    cin.read(&code_a[0], len_a);
    cin.read(&code_b[0], len_b);

    int window = 4;
    if (const char *env = getenv("WINDOW"))
    {
        window = max(1, stoi(env));
    }

    string norm_a = normalize(code_a);
    string norm_b = normalize(code_b);

    vector<string> tokens_a = tokenize(norm_a);
    vector<string> tokens_b = tokenize(norm_b);

    vector<uint64_t> fp_a = computeFingerprints(tokens_a, window);
    vector<uint64_t> fp_b = computeFingerprints(tokens_b, window);

    double jaccard = jaccardSimilarity(fp_a, fp_b);
    double edit_sim = editSimilarity(tokens_a, tokens_b);
    double ast_sim = astStructuralSimilarity(tokens_a, tokens_b);
    double pdg_sim = pdgSimilarity(tokens_a, tokens_b);

    double majority_score = max({jaccard, edit_sim, ast_sim, pdg_sim});
    double minority_score = min({jaccard, edit_sim, ast_sim, pdg_sim});

    double final_score = (jaccard + edit_sim + ast_sim + pdg_sim) / 4.0;

    cout << fixed << setprecision(4);
    cout << "{\n";
    cout << "  \"tokensA\": " << tokens_a.size() << ",\n";
    cout << "  \"tokensB\": " << tokens_b.size() << ",\n";
    cout << "  \"fingerprintsA\": " << fp_a.size() << ",\n";
    cout << "  \"fingerprintsB\": " << fp_b.size() << ",\n";
    cout << "  \"jaccard\": " << jaccard << ",\n";
    cout << "  \"editSimilarity\": " << edit_sim << ",\n";
    cout << "  \"astStructural\": " << ast_sim << ",\n";
    cout << "  \"pdgSimilarity\": " << pdg_sim << ",\n";
    cout << "  \"majorityScore\": " << majority_score << ",\n";
    cout << "  \"minorityScore\": " << minority_score << ",\n";
    cout << "  \"FinalScore\": " << final_score << "\n";
    cout << "}\n";

    return 0;
}