#include "../include/lexor.h"

Token::Token(TokenType tt, string i) {
  type = tt;
  lexeme = i;
}

vector<Token> Lexor::lex_input(string &input) {
  vector<Token> lex_return = {};
  string buffer = "";
  for (size_t i = 0; i < input.size(); i++) {
    char c = input[i];
    switch (current_state) {
    case DEFAULT:
      if (c == '\'') {
        current_state = IN_SINGLE_QUOTE;
      } else if (c == '"') {
        current_state = IN_DOUBLE_QUOTE;
      } else if (c == '\\') {
        current_state = ESCAPE;
      } else if (c == '&') {
        if (i + 1 < input.size() && input[i + 1] == '&') {
          lex_return.push_back(Token(TokenType::AND_IF, ""));
          i++; // consume second '&'
        } else {
          current_state = WORD;
        }
      } else if (c == '|') {
        lex_return.push_back(Token(TokenType::PIPE, ""));
      } else {
        current_state = WORD;
      }
      break;
    case WORD:
      if (c == ' ') {
        if (!buffer.empty()) {
          lex_return.push_back(Token(TokenType::WORD, buffer));
          buffer = "";
        }
        current_state = DEFAULT;
      } else if (c == '&' || c == '|') {
        // finalize current word before a symbol
        if (!buffer.empty()) {
          lex_return.push_back(Token(TokenType::WORD, buffer));
          buffer = "";
        }
        i--; // reprocess the symbol in DEFAULT
        current_state = DEFAULT;
      } else {
        buffer += c;
      }
      break;
    case IN_SINGLE_QUOTE:
      if (c == '\'') {
        lex_return.push_back(Token(TokenType::WORD, buffer));
        buffer = "";
        current_state = DEFAULT;
      } else {
        buffer += c;
      }
      break;
    case IN_DOUBLE_QUOTE:
      if (c == '"') {
        lex_return.push_back(Token(TokenType::WORD, buffer));
        buffer = "";
        current_state = DEFAULT;
      } else {
        buffer += c;
      }
      break;
    case ESCAPE:
      buffer += c;
      current_state = WORD; // or previous state
      break;
    }
  }
  if (!buffer.empty()) {
    lex_return.push_back(Token(TokenType::WORD, buffer));
  }
  return lex_return;
}