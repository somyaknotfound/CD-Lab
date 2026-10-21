// SLR(1) parser driven by Parsing Table 1
// Grammar (productions numbered as used by the rN entries of the table):
//   1: E -> E + T    2: E -> T    3: T -> T * F    4: T -> F    5: F -> ( E )    6: F -> id
#include <bits/stdc++.h>
using namespace std;

struct Prod { string lhs; int len; string text; };
const vector<Prod> P = {
    {"", 0, ""},                                   // index 0 unused (augmented E' -> E)
    {"E", 3, "E -> E + T"},
    {"E", 1, "E -> T"},
    {"T", 3, "T -> T * F"},
    {"T", 1, "T -> F"},
    {"F", 3, "F -> ( E )"},
    {"F", 1, "F -> id"}
};

// ACTION[state][terminal]: "sN" = shift to N, "rN" = reduce by production N, "acc" = accept
const vector<map<string, string>> ACTION = {
    /* 0 */ {{"id", "s5"}, {"(", "s4"}},
    /* 1 */ {{"+", "s6"}, {"$", "acc"}},
    /* 2 */ {{"+", "r2"}, {"*", "s7"}, {")", "r2"}, {"$", "r2"}},
    /* 3 */ {{"+", "r4"}, {"*", "r4"}, {")", "r4"}, {"$", "r4"}},
    /* 4 */ {{"id", "s5"}, {"(", "s4"}},
    /* 5 */ {{"+", "r6"}, {"*", "r6"}, {")", "r6"}, {"$", "r6"}},
    /* 6 */ {{"id", "s5"}, {"(", "s4"}},
    /* 7 */ {{"id", "s5"}, {"(", "s4"}},
    /* 8 */ {{"+", "s6"}, {")", "s11"}},
    /* 9 */ {{"+", "r1"}, {"*", "s7"}, {")", "r1"}, {"$", "r1"}},
    /*10 */ {{"+", "r3"}, {"*", "r3"}, {")", "r3"}, {"$", "r3"}},
    /*11 */ {{"+", "r5"}, {"*", "r5"}, {")", "r5"}, {"$", "r5"}}
};

// GOTO[state][non-terminal]
const vector<map<string, int>> GOTO = {
    /* 0 */ {{"E", 1}, {"T", 2}, {"F", 3}},
    /* 1 */ {}, /* 2 */ {}, /* 3 */ {},
    /* 4 */ {{"E", 8}, {"T", 2}, {"F", 3}},
    /* 5 */ {},
    /* 6 */ {{"T", 9}, {"F", 3}},
    /* 7 */ {{"F", 10}},
    /* 8 */ {}, /* 9 */ {}, /*10 */ {}, /*11 */ {}
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

    // Stack holds states and grammar symbols alternately: 0 X1 s1 X2 s2 ... (top at back)
    vector<string> st = {"0"};
    size_t ip = 0;
    vector<array<string, 3>> rows;                 // {Stack, Input, Action}
    bool accepted = false, error = false;

    while (!accepted && !error) {
        const int s = stoi(st.back());
        const string a = in[ip];
        array<string, 3> row = {join(st.begin(), st.end(), " "), join(in.begin() + ip, in.end()), ""};

        auto it = ACTION[s].find(a);
        if (it == ACTION[s].end()) {
            row[2] = "ERROR: ACTION[" + to_string(s) + ", " + a + "] is empty";
            error = true;
        } else if (it->second == "acc") {
            row[2] = "ACCEPT";
            accepted = true;
        } else if (it->second[0] == 's') {         // shift: push terminal and new state
            const string nxt = it->second.substr(1);
            st.push_back(a);
            st.push_back(nxt);
            ip++;
            row[2] = "Shift " + nxt;
        } else {                                   // reduce: pop 2*|RHS|, push LHS and GOTO state
            const int p = stoi(it->second.substr(1));
            st.resize(st.size() - 2 * P[p].len);
            const int top = stoi(st.back());
            auto g = GOTO[top].find(P[p].lhs);
            if (g == GOTO[top].end()) {
                row[2] = "ERROR: GOTO[" + to_string(top) + ", " + P[p].lhs + "] is empty";
                error = true;
            } else {
                st.push_back(P[p].lhs);
                st.push_back(to_string(g->second));
                row[2] = "Reduce by " + P[p].text + "  (r" + to_string(p) + "), GOTO[" +
                         to_string(top) + ", " + P[p].lhs + "] = " + to_string(g->second);
            }
        }
        rows.push_back(row);
    }

    // Print table with auto-sized columns
    size_t w0 = 5, w1 = 5;
    for (auto& r : rows) { w0 = max(w0, r[0].size()); w1 = max(w1, r[1].size()); }
    w0 += 4; w1 += 4;
    cout << "\n" << left << setw(w0) << "STACK" << setw(w1) << "INPUT" << "ACTION\n"
         << string(w0 + w1 + 45, '-') << "\n";
    for (auto& r : rows) cout << setw(w0) << r[0] << setw(w1) << r[1] << r[2] << "\n";

    cout << "\nResult: " << (accepted ? "String ACCEPTED" : "String REJECTED") << "\n";
    return accepted ? 0 : 1;
}
