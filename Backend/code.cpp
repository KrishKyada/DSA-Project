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
                result += ' ';
        }
        else
            result += code[i];
    }

    while (!result.empty() && result.back() == ' ')
        result.pop_back();
    while (!result.empty() && result.front() == ' ')
        result.erase(0, 1);

    return result;
}

// ============================================================================
// 2. JACCARD SIMILARITY
// ============================================================================
double jaccardSimilarity(const vector<uint64_t> &fp1, const vector<uint64_t> &fp2)
{
    if (fp1.empty() && fp2.empty()) return 1.0;
    if (fp1.empty() || fp2.empty()) return 0.0;

    unordered_set<uint64_t> set1(fp1.begin(), fp1.end());
    unordered_set<uint64_t> set2(fp2.begin(), fp2.end());

    size_t intersection = 0;
    for (uint64_t v : set1)
        if (set2.count(v)) intersection++;

    size_t uni = set1.size() + set2.size() - intersection;
    return uni > 0 ? (double)intersection / uni : 0.0;
}

// ============================================================================
// 3. TOKENIZATION
// ============================================================================
vector<string> tokenize(const string &code)
{
    vector<string> tokens;
    string current;

    for (char c : code)
    {
        if (isalnum(c) || c == '_')
            current += c;
        else
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }
            if (!isspace(c))
                tokens.push_back(string(1, c));
        }
    }

    if (!current.empty())
        tokens.push_back(current);

    return tokens;
}

// ============================================================================
// 4. EDIT DISTANCE (Levenshtein)
// ============================================================================
double editSimilarity(const vector<string> &tok1, const vector<string> &tok2)
{
    int n = tok1.size();
    int m = tok2.size();

    if (n == 0 && m == 0) return 1.0;
    if (n == 0 || m == 0) return 0.0;

    vector<int> prev(m + 1), curr(m + 1);
    for (int j = 0; j <= m; j++) prev[j] = j;

    for (int i = 1; i <= n; i++)
    {
        curr[0] = i;
        for (int j = 1; j <= m; j++)
        {
            if (tok1[i - 1] == tok2[j - 1])
                curr[j] = prev[j - 1];
            else
                curr[j] = 1 + min({prev[j], curr[j - 1], prev[j - 1]});
        }
        prev.swap(curr);
    }

    int dist = prev[m];
    return 1.0 - (double)dist / max(n, m);
}

// ============================================================================
// 5. FINGERPRINTING (Winnowing)
// ============================================================================
vector<uint64_t> computeFingerprints(const vector<string> &tokens, int window_size)
{
    if (tokens.empty() || window_size <= 0)
        return {};

    const uint64_t BASE = 911382323ULL;
    const uint64_t MOD = 1000000007ULL;

    vector<uint64_t> fp;
    vector<uint64_t> roll;

    for (auto &t : tokens)
    {
        uint64_t h = 0;
        for (char c : t)
            h = (h * BASE + (uint64_t)c) % MOD;
        roll.push_back(h);
    }

    if ((int)roll.size() < window_size)
        return fp;

    for (size_t i = 0; i + window_size <= roll.size(); i++)
    {
        uint64_t mn = roll[i];
        for (int j = 1; j < window_size; j++)
            mn = min(mn, roll[i + j]);
        fp.push_back(mn);
    }

    return fp;
}

// ============================================================================
// 6. AST STRUCTURAL SIMILARITY
// ============================================================================
double astStructuralSimilarity(const vector<string> &tok1, const vector<string> &tok2)
{
    static const unordered_set<string> ast_symbols = {
        "{","}","(",")","[","]",";",
        "if","else","for","while","switch","case","default",
        "return","+","-","*","/","%","++","--","=","==",
        "!=","<",">","<=",">=","&&","||","?",":",","
    };

    unordered_map<string,int> c1, c2;

    for (auto &t : tok1) if (ast_symbols.count(t)) c1[t]++;
    for (auto &t : tok2) if (ast_symbols.count(t)) c2[t]++;

    unordered_set<string> all;
    for (auto &p : c1) all.insert(p.first);
    for (auto &p : c2) all.insert(p.first);

    if (all.empty()) return 1.0;

    int common = 0, total = 0;

    for (auto &k : all)
    {
        int a = c1[k], b = c2[k];
        common += min(a, b);
        total += max(a, b);
    }

    return total > 0 ? (double)common / total : 0.0;
}

// ============================================================================
// 7. PDG (Program Dependence Graph)
// ============================================================================
double pdgSimilarity(const vector<string> &tok1, const vector<string> &tok2)
{
    auto extractor = [](const vector<string> &t)
    {
        vector<pair<string,int>> flow;
        int depth = 0;
        for (size_t i = 0; i < t.size(); i++)
        {
            if (t[i] == "{") depth++;
            if (t[i] == "}") depth = max(0, depth - 1);

            if (t[i] == "if" || t[i] == "else" || t[i] == "for" ||
                t[i] == "while" || t[i] == "switch" || t[i] == "return")
            {
                flow.push_back({t[i], depth});
            }
        }
        return flow;
    };

    auto f1 = extractor(tok1);
    auto f2 = extractor(tok2);

    if (f1.empty() && f2.empty()) return 1.0;
    if (f1.empty() || f2.empty()) return 0.0;

    int mn = min(f1.size(), f2.size());
    int mx = max(f1.size(), f2.size());
    double sc = 0;

    for (int i = 0; i < mn; i++)
    {
        if (f1[i].first == f2[i].first)
        {
            int d1 = f1[i].second;
            int d2 = f2[i].second;
            if (abs(d1 - d2) <= 1) sc += 1.0;
            else sc += 0.5;
        }
    }

    return sc / mx;
}

// ============================================================================
// MAIN
// ============================================================================
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string l1, l2;
    if (!getline(cin, l1) || !getline(cin, l2))
    {
        cout << "{\"error\":\"Missing input headers\"}\n";
        return 0;
    }

    size_t p1 = l1.find(' ');
    size_t p2 = l2.find(' ');
    if (p1 == string::npos || p2 == string::npos)
    {
        cout << "{\"error\":\"Invalid format\"}\n";
        return 0;
    }

    int lenA = stoi(l1.substr(p1 + 1));
    int lenB = stoi(l2.substr(p2 + 1));

    string A(lenA, '\0'), B(lenB, '\0');
    cin.read(&A[0], lenA);
    cin.read(&B[0], lenB);

    int window = 4;
    if (const char *env = getenv("WINDOW"))
        window = max(1, stoi(env));

    string nA = normalize(A);
    string nB = normalize(B);

    auto tokA = tokenize(nA);
    auto tokB = tokenize(nB);

    auto fpA = computeFingerprints(tokA, window);
    auto fpB = computeFingerprints(tokB, window);

    double j = jaccardSimilarity(fpA, fpB);
    double e = editSimilarity(tokA, tokB);
    double ast = astStructuralSimilarity(tokA, tokB);
    double pdg = pdgSimilarity(tokA, tokB);

    double majority = max({j, e, ast, pdg});
    double minority = min({j, e, ast, pdg});
    double final_score = (j + e + ast + pdg) / 4.0;

    cout << fixed << setprecision(4);
    cout << "{\n";
    cout << "  \"tokensA\": " << tokA.size() << ",\n";
    cout << "  \"tokensB\": " << tokB.size() << ",\n";
    cout << "  \"fingerprintsA\": " << fpA.size() << ",\n";
    cout << "  \"fingerprintsB\": " << fpB.size() << ",\n";
    cout << "  \"jaccard\": " << j << ",\n";
    cout << "  \"editSimilarity\": " << e << ",\n";
    cout << "  \"astStructural\": " << ast << ",\n";
    cout << "  \"pdgSimilarity\": " << pdg << ",\n";
    cout << "  \"majorityScore\": " << majority << ",\n";
    cout << "  \"minorityScore\": " << minority << ",\n";
    cout << "  \"FinalScore\": " << final_score << "\n";
    cout << "}\n";

    return 0;
}
