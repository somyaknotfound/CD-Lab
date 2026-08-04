/* Q2: Symbol Table of variables and function prototypes/definitions */
%{
#include <bits/stdc++.h>
using namespace std;
struct S { string n, c, t, sc, ow; int np; vector<int> l; };
vector<S> tb; map<string,int> ix;
int ln = 1, bd = 0, ad = 0, idp = 0, pd = 0, fnLn = 0;
string pt, cf, fn, ft, pb; bool init = false;
vector<pair<string,string>> pp;

string trim(string s) {
    size_t a = s.find_first_not_of(" \t\n\r");
    return a == string::npos ? "" : s.substr(a, s.find_last_not_of(" \t\n\r") - a + 1);
}
void add(string n, string c, string t, string sc, string ow, int np, int l) {
    if (n.empty()) return;
    string k = n + "|" + sc + "|" + ow + "|" + c;
    if (ix.count(k)) { tb[ix[k]].l.push_back(l); return; }
    ix[k] = tb.size(); tb.push_back({n, c, t, sc, ow, np, {l}});
}
void var(string n, string t) { add(n, "Variable", t, bd ? "Local" : "Global", bd ? cf : "", -1, ln); }
bool decl() { return !pt.empty() && !init && !ad; }        /* are we in a declarator? */
string head(const char *s) { return trim(string(s).substr(0, strlen(s) - 1)); }

void params() {                                            /* split pb, count, name them */
    vector<string> ps; string cur; int d = 0;
    for (char c : pb) {
        if (strchr("([{<", c)) d++; else if (strchr(")]}>", c)) d--;
        if (c == ',' && !d) { ps.push_back(cur); cur.clear(); } else cur += c;
    }
    if (!trim(cur).empty()) ps.push_back(cur);
    if (ps.size() == 1 && trim(ps[0]) == "void") ps.clear();
    pp.clear();
    for (string p : ps) {
        p = trim(p); string sfx; size_t b = p.find('[');
        if (b != string::npos) { sfx = "[]"; p = trim(p.substr(0, b)); }
        size_t e = p.size();
        while (e && (isalnum((unsigned char)p[e-1]) || p[e-1] == '_')) e--;
        string nm = p.substr(e), hd = trim(p.substr(0, e));
        if (!hd.empty() && !nm.empty() && !isdigit((unsigned char)nm[0])) pp.push_back({nm, hd + sfx});
    }
    add(fn, "Function", ft.empty() ? "int" : ft, bd ? "Local" : "Global", "", ps.size(), fnLn);
}
%}
%option noyywrap
%x PARAMS
ID   [a-zA-Z_][a-zA-Z0-9_]*
TYPE "int"|"float"|"double"|"char"|"bool"|"void"|"long"|"short"|"unsigned"|"signed"|"string"|"auto"|"size_t"|"const"|"static"|"extern"|"inline"|"virtual"|"constexpr"
%%
\n                          ln++;
[ \t\r]+|"//".*|^[ \t]*#.*  ;
"/*"([^*]|\*+[^*/])*\*+"/"  ln += count(yytext, yytext + yyleng, '\n');
\"([^\\\"\n]|\\.)*\"|'([^\\'\n]|\\.)'   ;
("for"|"catch")[ \t]*"("                        ;  /* may declare a loop variable */
("if"|"while"|"switch"|"return"|"sizeof")[ \t]*"("  ad++;
{TYPE}                      pt += (pt.empty() ? "" : " ") + string(yytext);
"*"|"&"                     if (decl()) pt += yytext;
{ID}[ \t]*"("               if (decl()) { fn = head(yytext); ft = pt; fnLn = ln; pt.clear(); pb.clear(); pd = 1; BEGIN(PARAMS); } else ad++;
{ID}[ \t]*"["               { if (decl()) var(head(yytext), pt + "[]"); ad++; }
{ID}                        if (decl()) var(yytext, pt);
"("|"["                     ad++;
")"|"]"                     if (ad) ad--;
"="                         init = true;
","                         if (!ad && !idp) init = false;
";"                         { pt.clear(); fn.clear(); pp.clear(); init = false; ad = idp = 0; }
"{"                         { if (init) idp++;
                              else if (++bd == 1 && !fn.empty()) {
                                  cf = fn;
                                  for (auto &p : pp) add(p.first, "Variable", p.second, "Local", cf, -1, fnLn);
                                  fn.clear(); pp.clear();
                              } }
"}"                         { if (idp) idp--; else if (bd && !--bd) { cf.clear(); pt.clear(); } }
.                           ;
<PARAMS>\n                  { ln++; pb += ' '; }
<PARAMS>"("                 { pd++; pb += yytext; }
<PARAMS>")"                 if (--pd) pb += yytext; else { params(); BEGIN(INITIAL); }
<PARAMS>.                   pb += yytext;
%%
int main(int argc, char **argv) {
    if (argc > 1 && !(yyin = fopen(argv[1], "r"))) { perror(argv[1]); return 1; }
    yylex();
    printf("\n%-16s %-10s %-14s %-8s %-12s %-7s %s\n",
           "NAME", "CATEGORY", "TYPE/RETURN", "SCOPE", "DEFINED IN", "PARAMS", "LINE(S)");
    printf("%s\n", string(85, '-').c_str());
    for (S &e : tb) {
        string ls;
        for (int x : e.l) ls += (ls.empty() ? "" : ",") + to_string(x);
        printf("%-16s %-10s %-14s %-8s %-12s %-7s %s\n", e.n.c_str(), e.c.c_str(), e.t.c_str(),
               e.sc.c_str(), e.ow.empty() ? "-" : e.ow.c_str(),
               e.np < 0 ? "-" : to_string(e.np).c_str(), ls.c_str());
    }
    printf("%s\nTotal symbols: %zu\n", string(85, '-').c_str(), tb.size());
}
