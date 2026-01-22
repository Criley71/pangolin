#ifndef lexer_H
#define lexer_H
#include <string>
#include <vector>
#include <iostream>
using namespace std;
enum class TokenType {
  WORD,
  PIPE,
  AND_IF,
  OR_IF,
  REDIRECT_OUT,
  REDIRECT_IN,
  REDIRECT_APPEND,
  SEMICOLON,
  NLINE,
  BACKGROUND,
  LPAREN,
  RPAREN,
  EOI,
  INVALID
};

enum class Lexer_State {
  DEFAULT,         // between tokens
  WORD,            // unquoted text
  IN_SINGLE_QUOTE, // single quote
  IN_DOUBLE_QUOTE, // double quote
  ESCAPE_IN_QUOTE,
  OP,
  WHITESPACE,
  COMMENT,
  ESCAPE, // after backslash
  ERROR
};

struct Token {
  TokenType type;
  string lexeme;
  Token(TokenType, string);
};

struct Lexer {
  Lexer_State current_state = Lexer_State::DEFAULT;
  vector<Token> lexed_tokens = {};
  vector<Token> lex_input(string &input);
};

#endif