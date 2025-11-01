#include <bits/stdc++.h>
using namespace std;

//USER NODE STRUCTURE
struct User {
    string name;
    unordered_map<string, double> friends;  // friend -> strength (weight)
    explicit User(string n = "") : name(std::move(n)) {}
};
