#ifndef LEXOR_H
#define LEXOR_H
#include <string>
#include <vector>
using namespace std;
enum TokenType {
  WORD,
  PIPE,
  AND_IF,
  OR_IF,
  REDIRECT_OUT,
  REDIRECT_IN,
  REDIRECT_APPEND,
  SEMICOLON,
  NEWLINE,
  BACKGROUND,
  LPAREN,
  RPAREN,
  EOI,
  INVALID
};

enum Lexor_State {
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

struct Lexor {
  Lexor_State current_state = DEFAULT;
  vector<Token> lexed_tokens = {};
  vector<Token> lex_input(string &input);
};

#endif