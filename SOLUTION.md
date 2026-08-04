# Week 3 (S2) — Lexical Analysis using Lex

**Course:** Compiler Design Lab (CS304) · NITK Surathkal
**Files:** [`1_token_table.l`](./1_token_table.l) (51 lines), [`2_symbol_table.l`](./2_symbol_table.l) (93 lines), [`sample.cpp`](./sample.cpp)

Both analyzers read a C++ source file (`argv[1]`, or stdin) and print their table at EOF.

## Build and run

```bash
flex -o lex.yy.c 1_token_table.l && g++ lex.yy.c -o token_table && ./token_table sample.cpp
```

```bash
flex -o lex.yy.c 2_symbol_table.l && g++ lex.yy.c -o symbol_table && ./symbol_table sample.cpp
```

Actions are C++ (`<bits/stdc++.h>`, `std::string`, `std::vector`, `std::map`), so the Flex output must be compiled with **`g++`, not `gcc`**. `%option noyywrap` is set, so `-lfl` is not needed and no `yywrap()` is written. Both files compile with no Flex warnings.

---

# Question 1 — Token Table

## Approach

One helper does all the bookkeeping:

```cpp
struct T { string lx, ct; int f; vector<int> l; };
vector<T> tb; map<string,int> ix; int ln = 1;
void rec(const string &lx, const string &ct) {
    string k = lx + "|" + ct;
    if (!ix.count(k)) { ix[k] = tb.size(); tb.push_back({lx, ct, 1, {ln}}); }
    else { T &e = tb[ix[k]]; e.f++; if (e.l.back() != ln) e.l.push_back(ln); }
}
```

The `map` keys on `lexeme|category` so one spelling under two categories stays distinct, while the `vector` preserves first-seen order for printing. Each entry counts every occurrence and appends a line number only when it changes, giving the distinct-lines list for free. `ln` is bumped by the `\n` rule and by `count(yytext, yytext+yyleng, '\n')` inside multi-line comments.

Every rule is a single line whose action is just a `rec(...)` call, so the rules section *is* the classification table. Ordering carries the logic, since Flex takes the **longest match** and breaks ties by **first rule listed**:

| Order | Rules | Why here |
|---|---|---|
| 1 | whitespace, comments, preprocessor | consumed before anything else can claim them |
| 2 | string / char literals | must beat the operator rules on the quote characters |
| 3 | keyword list | equal length to `{ID}`, so it must precede it |
| 4 | numeric constants | hex → octal → float → decimal, most specific first |
| 5 | `{ID}` | everything alphabetic that is not a keyword |
| 6 | operators, then delimiters | multi-char forms (`<=`, `+=`, `<<=`) win by longest match |
| 7 | `.` → `UNKNOWN` | catch-all for illegal characters |

### Numeric constants

```lex
D   [0-9]
FLT ({D}+\.{D}*|\.{D}+|{D}+)[eE][+-]?{D}+[fFlL]?|({D}+\.{D}*|\.{D}+)[fFlL]?

0[xX][0-9a-fA-F]+     HEX_CONSTANT
0[0-7]+               OCTAL_CONSTANT
{FLT}                 FLOAT_CONSTANT
([1-9]{D}*|0)[uUlL]*  DECIMAL_CONSTANT
```

`0755` also matches the decimal rule's `0` branch, but the octal match is 4 characters against 1, so longest-match picks octal; `0x1F4` works the same way. `2.5e-3` needs the exponent alternative in `FLT`, otherwise it splits into `2.5`, `e`, `-`, `3`.

### Operator categories

```lex
"+"|"-"|"*"|"/"|"%"|"++"|"--"                            ARITHMETIC_OP
"=="|"!="|"<="|">="|"<"|">"                              RELATIONAL_OP
"&&"|"||"|"!"                                            LOGICAL_OP
"="|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>="  ASSIGNMENT_OP
"&"|"|"|"^"|"~"|"<<"|">>"                                BITWISE_OP
"::"|"->"|"?"|":"|"."                                    OTHER_OP
```

`+` and `+=` sit in different rules, but `+=` is two characters, so longest match reaches the assignment rule. `&&` (logical) beats `&` (bitwise) the same way — no ordering trick needed for either.

## Output on `sample.cpp`

```
LEXEME                 CATEGORY           FREQ   LINE NUMBER(S)
--------------------------------------------------------------------------
#include <iostream>    PREPROCESSOR       1      2
using                  KEYWORD            1      3
namespace              KEYWORD            1      3
std                    IDENTIFIER         1      3
;                      DELIMITER          33     3,5,6,7,8,10,11,12,15,16,20,...
const                  KEYWORD            1      5
double                 KEYWORD            5      5,11,19,20,42
PI                     IDENTIFIER         2      5,54
=                      ASSIGNMENT_OP      14     5,6,8,15,20,21,22,35,36,37,...
3.14159                FLOAT_CONSTANT     1      5
int                    KEYWORD            19     6,10,11,14,15,19,21,22,34,35,36,37,41
globalCount            IDENTIFIER         3      6,45,47
0                      DECIMAL_CONSTANT   6      6,21,22,23,41,55
float                  KEYWORD            2      7,38
rates                  IDENTIFIER         1      7
[                      DELIMITER          6      7,11,19,23,24,41
3                      DECIMAL_CONSTANT   3      7,41,42
]                      DELIMITER          6      7,11,19,23,24,41
char                   KEYWORD            1      8
grade                  IDENTIFIER         1      8
'A'                    CHAR_LITERAL       1      8
add                    IDENTIFIER         3      10,14,50
(                      DELIMITER          13     10,11,12,14,19,22,23,30,34,42,44,50,53
a                      IDENTIFIER         3      10,14,15
,                      DELIMITER          11     10,11,14,19,41,42,50
b                      IDENTIFIER         3      10,14,15
)                      DELIMITER          13     10,11,12,14,19,22,23,30,34,42,44,50,53
average                IDENTIFIER         3      11,19,42
arr                    IDENTIFIER         4      11,19,23,24
n                      IDENTIFIER         3      11,19,22
bool                   KEYWORD            3      11,19,39
skipZero               IDENTIFIER         3      11,19,23
void                   KEYWORD            2      12,30
banner                 IDENTIFIER         3      12,30,53
{                      DELIMITER          8      14,19,22,30,34,41,44,46
sum                    IDENTIFIER         2      15,16
+                      ARITHMETIC_OP      1      15
return                 KEYWORD            3      16,27,55
}                      DELIMITER          8      17,26,28,32,41,46,48,56
total                  IDENTIFIER         3      20,24,27
0.0                    FLOAT_CONSTANT     2      20,27
used                   IDENTIFIER         4      21,25,27
for                    KEYWORD            1      22
i                      IDENTIFIER         5      22,23,24
<                      RELATIONAL_OP      1      22
++                     ARITHMETIC_OP      3      22,25,45
if                     KEYWORD            2      23,44
&&                     LOGICAL_OP         2      23,44
==                     RELATIONAL_OP      1      23
continue               KEYWORD            1      23
+=                     ASSIGNMENT_OP      2      24,50
?                      OTHER_OP           1      27
/                      ARITHMETIC_OP      1      27
:                      OTHER_OP           1      27
cout                   IDENTIFIER         2      31,54
<<                     BITWISE_OP         8      31,54
"Lexical Analysis ...  STRING_LITERAL     1      31
endl                   IDENTIFIER         2      31,54
main                   IDENTIFIER         1      34
octal                  IDENTIFIER         2      35,50
0755                   OCTAL_CONSTANT     1      35
hexa                   IDENTIFIER         3      36,44,50
0x1F4                  HEX_CONSTANT       1      36
decimal                IDENTIFIER         3      37,44,50
250                    DECIMAL_CONSTANT   1      37
ratio                  IDENTIFIER         3      38,51,54
2.5e-3                 FLOAT_CONSTANT     1      38
flag                   IDENTIFIER         3      39,42,44
true                   KEYWORD            1      39
values                 IDENTIFIER         2      41,42
10                     DECIMAL_CONSTANT   1      41
20                     DECIMAL_CONSTANT   1      41
avg                    IDENTIFIER         2      42,54
>=                     RELATIONAL_OP      1      44
!                      LOGICAL_OP         1      44
else                   KEYWORD            1      46
--                     ARITHMETIC_OP      1      47
*=                     ASSIGNMENT_OP      1      51
2                      DECIMAL_CONSTANT   1      51
" "                    STRING_LITERAL     2      54
--------------------------------------------------------------------------
Distinct lexemes: 80, Lines scanned: 57
```

The long line lists for `;` and `=` are elided with `...` above; the program prints them in full. All four numeric forms required by the question appear: `250` decimal, `0755` octal, `0x1F4` hexadecimal, and `3.14159` / `0.0` / `2.5e-3` floating-point.

---

# Question 2 — Symbol Table

## Approach

A token stream alone cannot tell a declaration from a use, so the scanner carries a little state between rules — this is where a lexer starts doing work that properly belongs to the parser. Six globals are enough:

| State | Purpose |
|---|---|
| `pt` | type words seen since the last `;`, e.g. `const double` |
| `init` | true between `=` and the `,`/`;` ending the initializer |
| `bd` | brace depth: `0` → global scope, `> 0` → local |
| `cf` | current function, used to disambiguate locals of the same name |
| `ad` | `()`/`[]` nesting outside declarations, so call arguments aren't read as declarations |
| `idp` | `{}` nesting inside a brace initializer, kept separate from `bd` |

That collapses into one predicate reused by every identifier rule:

```cpp
bool decl() { return !pt.empty() && !init && !ad; }
```

It is what separates `int a` (declaration) from the `flag` in `average(values, 3, flag)` (call argument).

### Distinguishing a function from a variable

Flex cannot look ahead, so the identifier and the following `(` are matched by a single rule, and `head()` strips the bracket back off:

```lex
{ID}[ \t]*"("  if (decl()) { fn = head(yytext); ft = pt; ... BEGIN(PARAMS); } else ad++;
{ID}[ \t]*"["  { if (decl()) var(head(yytext), pt + "[]"); ad++; }
{ID}           if (decl()) var(yytext, pt);
```

`if (`, `while (`, `switch (`, `return (` and `sizeof(` are listed **before** this rule so they aren't mistaken for function headers — same match length, so the earlier rule wins. `for (` and `catch (` get their own rule that does *not* bump `ad`, because a loop header may legitimately declare `int i`.

### Parameters — exclusive start condition

```lex
%x PARAMS
```

On a function header the scanner switches to `PARAMS`, buffers the raw parameter text into `pb` and tracks `pd` so nested parentheses don't close the list early. On the matching `)`, `params()` does three things:

1. Splits `pb` on commas at nesting depth 0, ignoring commas inside `()`, `[]`, `{}` or `<>` — so `map<int,int> m` counts as one. A lone `void` gives 0.
2. Peels the trailing identifier off each parameter for the name, leaving the rest as the type, and folds `[` into an `[]` suffix. An unnamed parameter (legal in a prototype) is skipped.
3. Adds the function row with its parameter count.

Parameters become local variables **lazily** — held in `pp` and committed only when a `{` follows, so a prototype's parameter names never pollute the table. A `;` instead of `{` discards them.

### Scope and de-duplication

Rows are keyed on `name | scope | owner | category`. A prototype and its later definition collapse into one row listing both line numbers (`add … 10,14`), while an `i` declared in two different functions stays as two rows because the owner differs.

## Output on `sample.cpp`

```
NAME             CATEGORY   TYPE/RETURN    SCOPE    DEFINED IN   PARAMS  LINE(S)
-------------------------------------------------------------------------------------
PI               Variable   const double   Global   -            -       5
globalCount      Variable   int            Global   -            -       6
rates            Variable   float[]        Global   -            -       7
grade            Variable   char           Global   -            -       8
add              Function   int            Global   -            2       10,14
average          Function   double         Global   -            3       11,19
banner           Function   void           Global   -            0       12,30
a                Variable   int            Local    add          -       14
b                Variable   int            Local    add          -       14
sum              Variable   int            Local    add          -       15
arr              Variable   int[]          Local    average      -       19
n                Variable   int            Local    average      -       19
skipZero         Variable   bool           Local    average      -       19
total            Variable   double         Local    average      -       20
used             Variable   int            Local    average      -       21
i                Variable   int            Local    average      -       22
main             Function   int            Global   -            0       34
octal            Variable   int            Local    main         -       35
hexa             Variable   int            Local    main         -       36
decimal          Variable   int            Local    main         -       37
ratio            Variable   float          Local    main         -       38
flag             Variable   bool           Local    main         -       39
values           Variable   int[]          Local    main         -       41
avg              Variable   double         Local    main         -       42
-------------------------------------------------------------------------------------
Total symbols: 24
```

Worth checking against the source:

- `add` and `average` each appear once with **two** line numbers — prototype (10, 11) and definition (14, 19).
- `banner` takes 0 parameters; `average` takes 3, counted across `int arr[], int n, bool skipZero`.
- `avg` on line 42 is recorded, but `values` and `flag` on that same line are not re-recorded — they are call arguments, suppressed by `init`/`ad`.
- `rates` and `arr` carry the `[]` suffix on their type.
- `i` from the `for` header on line 22 is captured as a local of `average`.

## Known limitations

Deliberate, since the assignment asks for a lexical analyzer and not a parser:

- Block scope is not distinguished — every `bd > 0` declaration is "Local" to the enclosing function, so a variable declared inside an inner `{ }` isn't separated from one in the function body.
- Class members, `struct` fields, `typedef`/`using` aliases and templates aren't modelled.
- User-defined types (`MyClass obj;`) aren't recognised as declarations, since `pt` only fills from the built-in type keyword list.
- A defaulted parameter (`int n = 10`) is still **counted** correctly, but its name isn't extracted — the trailing token is the literal, so it's treated as unnamed and skipped. Function-pointer parameters are likewise counted but not named.
