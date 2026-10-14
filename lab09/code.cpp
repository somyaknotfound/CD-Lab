// Predictive (LL(1)) parser driven by Parsing Table 1
// Grammar:  E -> TE'   E' -> +TE' | e   T -> FT'   T' -> *FT' | e   F -> (E) | id
#include <bits/stdc++.h>
using namespace std;

using Prod = vector<string>;                       // RHS of a production; empty = epsilon
const map<string, map<string, Prod>> M = {         // M[NonTerminal][terminal]
    {"E",  {{"id", {"T", "E'"}}, {"(", {"T", "E'"}}}},
    {"E'", {{"+", {"+", "T", "E'"}}, {")", {}}, {"$", {}}}},
    {"T",  {{"id", {"F", "T'"}}, {"(", {"F", "T'"}}}},
    {"T'", {{"+", {}}, {"*", {"*", "F", "T'"}}, {")", {}}, {"$", {}}}},
    {"F",  {{"id", {"id"}}, {"(", {"(", "E", ")"}}}}
};

// Any identifier/number becomes token "id"; + * ( ) are single-char tokens; $ appended at end
vector<string> tokenize(const string& s) {
    vector<string> t;
    for (size_t i = 0; i < s.size();) {
        if (isspace((unsigned char)s[i])) { i++; continue; }
        if (s[i] == '$') {                         // optional user-typed end marker
            if (s.find_first_not_of(" \t\r\n", i + 1) != string::npos)
                throw runtime_error("Lexical error: '$' must be the last symbol");
            break;
        }
        if (isalnum((unsigned char)s[i]) || s[i] == '_') {
            while (i < s.size() && (isalnum((unsigned char)s[i]) || s[i] == '_')) i++;
            t.push_back("id");
        } else if (strchr("+*()", s[i])) {
            t.push_back(string(1, s[i++]));
        } else {
            throw runtime_error(string("Lexical error: invalid character '") + s[i] + "'");
        }
    }
    t.push_back("$");
    return t;
}

template <class It>
string join(It b, It e, const string& sep = "") {
    string r;
    for (It i = b; i != e; ++i) r += (i == b ? "" : sep) + *i;
    return r;
}

int main() {
    string line;
    cout << "Enter input string (e.g. id+id*id): ";
    getline(cin, line);

    vector<string> in;
    try { in = tokenize(line); }
    catch (const exception& e) { cout << "\n" << e.what() << "\nResult: String REJECTED\n"; return 1; }

    vector<string> st = {"$", "E"};                // stack: bottom ... top
    size_t ip = 0;
    vector<array<string, 3>> rows;                 // {Stack, Input, Action}
    bool accepted = false;

    while (true) {
        const string X = st.back(), a = in[ip];
        array<string, 3> row = {join(st.begin(), st.end()), join(in.begin() + ip, in.end()), ""};

        if (X == "$" && a == "$") {
            row[2] = "ACCEPT";
            accepted = true;
        } else if (!M.count(X)) {                  // X is a terminal (or $)
            if (X == a) { st.pop_back(); ip++; row[2] = "Match " + a; }
            else row[2] = "ERROR: expected '" + X + "', found '" + a + "'";
        } else {                                   // X is a non-terminal
            auto it = M.at(X).find(a);
            if (it == M.at(X).end()) {
                row[2] = "ERROR: M[" + X + ", " + a + "] is empty";
            } else {
                const Prod& rhs = it->second;
                st.pop_back();
                st.insert(st.end(), rhs.rbegin(), rhs.rend());   // push RHS in reverse
                row[2] = "Output " + X + " -> " + (rhs.empty() ? "epsilon" : join(rhs.begin(), rhs.end()));
            }
        }
        rows.push_back(row);
        if (accepted || row[2].rfind("ERROR", 0) == 0) break;
    }

    // Print table with auto-sized columns
    size_t w0 = 5, w1 = 5;
    for (auto& r : rows) { w0 = max(w0, r[0].size()); w1 = max(w1, r[1].size()); }
    w0 += 4; w1 += 4;
    cout << "\n" << left << setw(w0) << "STACK" << setw(w1) << "INPUT" << "ACTION\n"
         << string(w0 + w1 + 30, '-') << "\n";
    for (auto& r : rows) cout << setw(w0) << r[0] << setw(w1) << r[1] << r[2] << "\n";

    cout << "\nResult: " << (accepted ? "String ACCEPTED" : "String REJECTED") << "\n";
    return accepted ? 0 : 1;
}
