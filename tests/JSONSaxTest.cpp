#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stack>

#define IndexType short int
#include "../JSON.hpp"

void useTokens(char * buffer, JSON::Token & t)
{
    static IndexType lastParent = JSON::InvalidPos;
    static int level = 0;
    const char * type[] = {"Undefined", "Object", "Array", "Key", "String", "Null", "True", "False", "Number"};
    const char * state[] = {"Unknown", "EnteringObject", "LeavingObject", "EnteringArray", "LeavingArray", "HadKey", "HadValue", "DoneParsing"};
    int i = 0;
    if (t.parent == JSON::EnteringObject || t.parent == JSON::EnteringArray) level++;
    if (t.parent == JSON::LeavingObject || t.parent == JSON::LeavingArray) level--;
    
    fprintf(stdout, "%*s%d. Token[%s], state[%s], start at %d", level, "", i, type[t.type], state[t.parent + 1], t.start);
    if (t.type == JSON::Token::Object || t.type == JSON::Token::Array)
        fprintf(stdout, " (id:%u) end at %d:\n", (unsigned)t.id, t.end);
    else
        fprintf(stdout, " with value: %.*s\n", t.end - t.start, &buffer[t.start]);
}

int handleError(JSON & a, IndexType err)
{
    if (err == JSON::NotEnoughTokens) fprintf(stderr, "Not enough tokens\n");
    else if (err == JSON::Invalid)    fprintf(stderr, "Invalid stream at pos: %d\n", a.pos);
    else if (err == JSON::Starving)   fprintf(stderr, "Starving at pos: %d\n", a.pos);
    else if (err == JSON::NeedRefill) fprintf(stderr, "Need refill at pos: %d\n", a.pos);
    return -1;
}
int runTest(char * buffer, IndexType realLen, bool verbose)
{
    JSON a;
    std::stack<IndexType> superPos;
    JSON::Token token;

    IndexType lastSuper = JSON::InvalidPos;
    while (true) {
        IndexType err = a.parseOne(buffer, realLen, token, lastSuper);
        if (err < 0) handleError(a, err);
        // Deal with entering an object or array
        if (err == JSON::SaveSuper)
            superPos.push(lastSuper);
        if (err == JSON::RestoreSuper) {
            if (superPos.size()) superPos.pop();
            lastSuper = superPos.size() ? superPos.top() : JSON::InvalidPos; 
        }

        // Use the token
        if (verbose) useTokens(buffer, token);
        if (err == JSON::Finished) break;
    }
    return 0;

}


int main(int argc, char ** argv)
{
    char * buffer = new char[0x10000];
    FILE * f = argc > 1 ? fopen(argv[1], "rb") : stdin;
    size_t len = fread(buffer, 1, 0x10000, f);
    
    
    if (runTest(buffer, len, true))
    {
        fprintf(stderr, "Failed test for SAX parsing\n");
    } else fprintf(stdout, "Test ok\n");
}
