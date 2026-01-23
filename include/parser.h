#ifndef PARSER_H
#define PARSER_H
#include <memory>
#include <string>
#include <vector>
#include "lexer.h"
#include <stdexcept>
using namespace std;
enum class NodeType {
    COMMAND,
    PIPE,
    AND_IF,
    OR_IF
};

struct ASTNode {
    NodeType type;
    vector<string> argv;
    unique_ptr<ASTNode> left;
    unique_ptr<ASTNode> right;
    
};

class Parser {
public:
    explicit Parser(const vector<Token>& tokens);
    unique_ptr<ASTNode> parse();

private:
    const vector<Token>& tokens;
    size_t pos;

    unique_ptr<ASTNode> parse_logical();
    unique_ptr<ASTNode> parse_pipeline();
    unique_ptr<ASTNode> parse_command();

    bool match(TokenType type);
    const Token& peek() const;
    bool at_end() const;
};

#endif