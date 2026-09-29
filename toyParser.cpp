#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "toyTokenizer.cpp"
using namespace std;
// =====================================================================
// PARSER — recursive descent following the BNF
// =====================================================================
struct Parser
{
    Token *tokens;
    int count;
    int pos;
};
const Token *prCurrent(const Parser &p) { return &p.tokens[p.pos]; }
bool prCheck(const Parser &p, TokenType t) { return prCurrent(p)->tokenType == t; }
void prAdvance(Parser &p)
{
    p.pos++;
}
bool prMatch(Parser &p, TokenType t)
{
    if (prCheck(p, t))
    {
        prAdvance(p);
        return true;
    }
    return false;
}
void prError(const Parser &p, string msg)
{
    const Token *t = prCurrent(p);
    cerr << "Parser error at line "
         << t->line
         << ": column "
         << t->column
         << ":" << msg
         << "(got " << tokenTypeName(t->tokenType) << ")\n ";
    exit(1);
}
const Token *prExpect(Parser &p, TokenType t, string what)
{
    if (!prCheck(p, t))
    {
        prError(p, "expected " + what + " (" + tokenTypeName(t) + ")");
    }
    const Token *tok = prCurrent(p);
    // prAdvance(p);
    return tok;
}
// -------- Forward decl --------
void parseA(Parser &p)
{
    if (!prCheck(p, TOK_A))
        prError(p, "expected 'a'");
    prAdvance(p);
    if (prCheck(p, TOK_A))
        parseA(p);
}

void parseB(Parser &p)
{
    if (!prCheck(p, TOK_B))
        prError(p, "expected 'b'");
    prAdvance(p);
    if (prCheck(p, TOK_B))
        parseB(p);
}

void parseC(Parser &p)
{
    if (!prCheck(p, TOK_C))
        prError(p, "expected 'c'");
    prAdvance(p);
}

void parseS(Parser &p)
{
    parseA(p);
    if (prCheck(p, TOK_C))
    {
        parseC(p);
        parseB(p);
    }
}

void parseProgram(Parser &p)
{
    parseS(p);
    if (!prCheck(p, TOK_EOF))
        prError(p, "expected EOF");
}

// =====================================================================
// MAIN
// =====================================================================
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (!fp)
    {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 1;
    }
    // ---- Tokenize ----
    Tokenizer tk;
    tkInit(&tk, fp);
    Token tokens[MAX_TOKENS];
    int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);
    fclose(fp);
    Parser parser;
    parser.tokens = tokens;
    parser.pos = 0;
    parseProgram(parser);
    cout << "No errors found\n";
    return 0;
}