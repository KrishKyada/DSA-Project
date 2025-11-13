#ifndef PLAGIARISM_CORE_H
#define PLAGIARISM_CORE_H

#include <bits/stdc++.h>
using namespace std;

struct AnalyzeResult {
    double jaccard = 0.0;
    int tokensA = 0, tokensB = 0;
    int fpsA = 0, fpsB = 0;
    string message;
};

string normalizeStrip(const string& s);
vector<string> tokenize(const string& code);
vector<uint64_t> computeFingerprints(const vector<string>& tokens, int window_size);
double jaccardSimilarity(const vector<uint64_t>& fp1, const vector<uint64_t>& fp2);
double editSimilarity(const vector<string>& tok1, const vector<string>& tok2);
double astStructuralSimilarity(const vector<string>& tok1, const vector<string>& tok2);
double pdgSimilarity(const vector<string>& tok1, const vector<string>& tok2);

#endif
