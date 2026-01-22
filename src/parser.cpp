#include "../include/parser.h"


//AST is a tree structure for order of operations for a command line input

Parser::Parser(const vector<Token> &tokens)
    : tokens(tokens), pos(0) {
}

unique_ptr<ASTNode> Parser::parse() {
  auto node = parse_and_if();

  if (!at_end()) {
    throw runtime_error("Unexpected token at end of input");
  }

  return node;
}

unique_ptr<ASTNode> Parser::parse_command() {
    if (at_end() || peek().type != TokenType::WORD) {
        throw runtime_error("Expected command");
    }

    auto node = std::make_unique<ASTNode>();
    node->type = NodeType::COMMAND;

    while (!at_end() && peek().type == TokenType::WORD) {
        node->argv.push_back(tokens[pos].lexeme);
        pos++;
    }

    return node;
}

unique_ptr<ASTNode> Parser::parse_pipeline() {
    auto left = parse_command();

    while (match(TokenType::PIPE)) {
        auto pipe = make_unique<ASTNode>();
        pipe->type = NodeType::PIPE;
        pipe->left = move(left);
        pipe->right = parse_command();
        left = move(pipe);
    }

    return left;
}

unique_ptr<ASTNode> Parser::parse_and_if() {
    auto left = parse_pipeline();

    while (match(TokenType::AND_IF)) {
        auto node = make_unique<ASTNode>();
        node->type = NodeType::AND_IF;
        node->left = move(left);
        node->right = parse_pipeline();
        left = move(node);
    }

    return left;
}

bool Parser::match(TokenType type) {
    if (!at_end() && tokens[pos].type == type) {
        pos++;
        return true;
    }
    return false;
}

const Token& Parser::peek() const {
    return tokens[pos];
}

bool Parser::at_end() const {
    return pos >= tokens.size();
}

