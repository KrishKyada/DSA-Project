#include <bits/stdc++.h>
using namespace std;

// Step 1: Normalize code — remove comments and extra spaces
string normalize(const string &s) {
    string out; bool in_sl = false, in_ml = false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (!in_sl && !in_ml && i + 1 < s.size() && s[i]=='/' && s[i+1]=='/') {
            in_sl = true; ++i; continue;
        }
        if (!in_sl && !in_ml && i + 1 < s.size() && s[i]=='/' && s[i+1]=='*') {
            in_ml = true; ++i; continue;
        }
        if (in_sl && s[i]=='\n') { in_sl = false; continue; }
        if (in_ml && i + 1 < s.size() && s[i]=='*' && s[i+1]=='/') { in_ml = false; ++i; continue; }
        if (in_sl || in_ml) continue;
        if (isspace((unsigned char)s[i])) {
            if (!out.empty() && out.back() != ' ') out.push_back(' ');
        } else out.push_back(s[i]);
    }
    return out;
}

// Step 2: Tokenize normalized code
vector<string> tokenize(const string &code) {
    vector<string> tokens; string cur;
    for (char c : code) {
        if (isalnum(c) || c == '_') cur.push_back(c);
        else {
            if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
            if (!isspace(c)) tokens.push_back(string(1, c));
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

// ---------------- Fingerprints via rolling hash + winnowing ------------------
vector<uint64_t> fingerprintTokens(const vector<string> &tokens, int window) {
    vector<uint64_t> hashes;
    if (tokens.empty() || window <= 0) return hashes;

    // Map tokens to integer ids (stable vocabulary in this run)
    unordered_map<string,int> id;
    id.reserve(tokens.size()*2);
    vector<int> ids; ids.reserve(tokens.size());
    int nxt = 1;
    for (auto &t : tokens) {
        auto it = id.find(t);
        if (it == id.end()) it = id.emplace(t, nxt++).first;
        ids.push_back(it->second);
    }

    // Rolling hash of k=1 token (simple and fast)
    vector<uint64_t> roll(ids.size());
    uint64_t h = 0;
    if ((int)roll.size() < window)
    return;

}
int main() {
    string code;
    cout << "Enter code (end with ~):\n";
    getline(cin, code, '~');

    string norm = normalize(code);
    vector<string> tok = tokenize(norm);

    cout << "\nNormalized Code:\n" << norm << "\n";
    cout << "\nTokens:\n";
    for (auto &t : tok) cout << "[" << t << "] ";
    cout << "\nToken Count: " << tok.size() << "\n";
    return 0;
}
