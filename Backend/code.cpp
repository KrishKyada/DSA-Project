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
// Calculates Jaccard similarity between two fingerprint vectors to check code similarity

double jaccardFingerprint(const vector<uint64_t> &a, const vector<uint64_t> &b) {
    if (a.empty() && b.empty()) return 1.0;
    unordered_set<uint64_t> setA(a.begin(), a.end());
    unordered_set<uint64_t> setB(b.begin(), b.end());
    int common = 0;
    for (auto x : setA) if (setB.count(x)) common++;
    int total = setA.size() + setB.size() - common;
    if (total == 0) return 0.0;
    return (double)common / total;
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

struct Node {
    int id;                // Unique identifier for this node
    string stmt;           // Code statement, e.g., "a=b+c"
    vector<int> edges;     // Edges pointing to dependent nodes
};

struct PDG {
    vector<Node> nodes;                     // All nodes in the PDG
    unordered_map<string, int> def;         // Variable to last definition mapping

    // Inside PDG struct

// Extract left-hand side of a statement
string extractLHS(const string& stmt) {
    size_t pos = stmt.find('=');
    return (pos != string::npos) ? stmt.substr(0, pos) : "";
}

// Extract right-hand side of a statement
string extractRHS(const string& stmt) {
    size_t pos = stmt.find('=');
    return (pos != string::npos && pos + 1 < stmt.size()) ? stmt.substr(pos + 1) : "";
}

};
