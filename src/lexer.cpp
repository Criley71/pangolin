#include "../include/lexer.h"

Token::Token(TokenType tt, string i) {
  type = tt;
  lexeme = i;
}

vector<Token> Lexer::lex_input(string &input) {
  vector<Token> lex_return = {};
  string buffer = "";
  current_state = Lexer_State::DEFAULT;
  Lexer_State prev_state;
  for (size_t i = 0; i < input.size(); i++) {
    char c = input[i];
    switch (current_state) {
    case Lexer_State::DEFAULT:
      if (c == '\'') {
        current_state = Lexer_State::IN_SINGLE_QUOTE;
      } else if (c == '"') {
        current_state = Lexer_State::IN_DOUBLE_QUOTE;
      } else if (c == '\\') {
        current_state = Lexer_State::ESCAPE;
      } else if (c == '&') {
        if (!buffer.empty()) {
          lex_return.push_back(Token(TokenType::WORD, buffer));
          buffer.clear();
        }
        if (i + 1 < input.size() && input[i + 1] == '&') {
          lex_return.push_back(Token(TokenType::AND_IF, ""));
          i++; // consume second '&'

        } else {
          lex_return.push_back(Token(TokenType::BACKGROUND, ""));
        }
      } else if (c == '|') {
        if (!buffer.empty()) {
          lex_return.push_back(Token(TokenType::WORD, buffer));
          buffer.clear();
        }
        if (i + 1 < input.size() && input[i + 1] == '|') {
          lex_return.push_back(Token(TokenType::OR_IF, ""));
          i++;
        } else {
          lex_return.push_back(Token(TokenType::PIPE, ""));
        }
      } else {
        if (c != ' ') {
          buffer += c;
        }
        current_state = Lexer_State::WORD;
      }
      break;
    case Lexer_State::WORD:
      if (c == ' ') {
        if (!buffer.empty()) {
          lex_return.push_back(Token(TokenType::WORD, buffer));
          buffer = "";
        }
        current_state = Lexer_State::DEFAULT;
      } else if (c == '&' || c == '|') {
        // finalize current word before a symbol
        if (!buffer.empty()) {
          lex_return.push_back(Token(TokenType::WORD, buffer));
          buffer = "";
        }
        i--; // reprocess the symbol in DEFAULT
        current_state = Lexer_State::DEFAULT;
      } else if (c == '\'') {
        current_state = Lexer_State::IN_SINGLE_QUOTE;
      } else if (c == '"') {
        current_state = Lexer_State::IN_DOUBLE_QUOTE;
      } else {
        buffer += c;
      }
      break;
    case Lexer_State::IN_SINGLE_QUOTE:
      if (c == '\'') {
        // lex_return.push_back(Token(TokenType::WORD, buffer));
        // buffer = "";
        current_state = Lexer_State::WORD;
      } else {
        buffer += c;
      }
      break;
    case Lexer_State::IN_DOUBLE_QUOTE:
      if (c == '"') {
        // lex_return.push_back(Token(TokenType::WORD, buffer));
        // buffer = "";
        current_state = Lexer_State::WORD;
      } else {
        buffer += c;
      }
      break;
    case Lexer_State::ESCAPE:
      buffer += c;
      current_state = prev_state;
      break;
    }
    prev_state = current_state;
  }
  if (!buffer.empty()) {
    lex_return.push_back(Token(TokenType::WORD, buffer));
  }
  return lex_return;
}