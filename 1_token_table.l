/* Q1: Token Table for a C++ source program */
%{
#include <bits/stdc++.h>
using namespace std;
struct T { string lx, ct; int f; vector<int> l; };
vector<T> tb; map<string,int> ix; int ln = 1;
void rec(const string &lx, const string &ct) {
    string k = lx + "|" + ct;
    if (!ix.count(k)) { ix[k] = tb.size(); tb.push_back({lx, ct, 1, {ln}}); }
    else { T &e = tb[ix[k]]; e.f++; if (e.l.back() != ln) e.l.push_back(ln); }
}
%}
%option noyywrap
D   [0-9]
ID  [a-zA-Z_][a-zA-Z0-9_]*
FLT ({D}+\.{D}*|\.{D}+|{D}+)[eE][+-]?{D}+[fFlL]?|({D}+\.{D}*|\.{D}+)[fFlL]?
%%
\n                          ln++;
[ \t\r]+|"//".*             ;
"/*"([^*]|\*+[^*/])*\*+"/"  ln += count(yytext, yytext + yyleng, '\n');
^[ \t]*#.*                  rec(yytext, "PREPROCESSOR");
\"([^\\\"\n]|\\.)*\"        rec(yytext, "STRING_LITERAL");
'([^\\'\n]|\\.)'            rec(yytext, "CHAR_LITERAL");
"alignas"|"asm"|"auto"|"bool"|"break"|"case"|"catch"|"char"|"class"|"const"|"constexpr"|"continue"|"default"|"delete"|"do"|"double"|"else"|"enum"|"explicit"|"extern"|"false"|"float"|"for"|"friend"|"goto"|"if"|"inline"|"int"|"long"|"mutable"|"namespace"|"new"|"nullptr"|"operator"|"private"|"protected"|"public"|"register"|"return"|"short"|"signed"|"sizeof"|"static"|"struct"|"switch"|"template"|"this"|"throw"|"true"|"try"|"typedef"|"typename"|"union"|"unsigned"|"using"|"virtual"|"void"|"volatile"|"while"  rec(yytext, "KEYWORD");
0[xX][0-9a-fA-F]+           rec(yytext, "HEX_CONSTANT");
0[0-7]+                     rec(yytext, "OCTAL_CONSTANT");
{FLT}                       rec(yytext, "FLOAT_CONSTANT");
([1-9]{D}*|0)[uUlL]*        rec(yytext, "DECIMAL_CONSTANT");
{ID}                        rec(yytext, "IDENTIFIER");
"+"|"-"|"*"|"/"|"%"|"++"|"--"                            rec(yytext, "ARITHMETIC_OP");
"=="|"!="|"<="|">="|"<"|">"                              rec(yytext, "RELATIONAL_OP");
"&&"|"||"|"!"                                            rec(yytext, "LOGICAL_OP");
"="|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>="  rec(yytext, "ASSIGNMENT_OP");
"&"|"|"|"^"|"~"|"<<"|">>"                                rec(yytext, "BITWISE_OP");
"::"|"->"|"?"|":"|"."                                    rec(yytext, "OTHER_OP");
[;,(){}\[\]]                rec(yytext, "DELIMITER");
.                           rec(yytext, "UNKNOWN");
%%
int main(int argc, char **argv) {
    if (argc > 1 && !(yyin = fopen(argv[1], "r"))) { perror(argv[1]); return 1; }
    yylex();
    printf("\n%-22s %-18s %-6s %s\n", "LEXEME", "CATEGORY", "FREQ", "LINE NUMBER(S)");
    printf("%s\n", string(74, '-').c_str());
    for (T &e : tb) {
        string ls;
        for (int x : e.l) ls += (ls.empty() ? "" : ",") + to_string(x);
        string lx = e.lx.size() > 21 ? e.lx.substr(0, 18) + "..." : e.lx;
        printf("%-22s %-18s %-6d %s\n", lx.c_str(), e.ct.c_str(), e.f, ls.c_str());
    }
    printf("%s\nDistinct lexemes: %zu, Lines scanned: %d\n", string(74, '-').c_str(), tb.size(), ln);
}
