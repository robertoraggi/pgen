// Copyright (c) 2018 Roberto Raggi <roberto.raggi@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pgen/lexer.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cassert>
#include <cctype>

TOK Lexer::yylex0() {
again:
  while (isspace(yychar)) yyinp();
  yytokenlineno = yylineno;
  yytext.clear();
  if (!yychar) return T_EOF_SYMBOL;
  auto ch = yychar;
  yytext += ch;
  yyinp();
  if (ch == '%' && yychar == '{') {
    yyinp();
    skip('{', '{', '}');
    yytext = std::string(yytext, 2, yytext.length() - 4);
    return T_TEXT;
  }
  if (ch == '%' && isalpha(yychar)) {
    while (isalnum(yychar) || yychar == '_') {
      yytext += yychar;
      yyinp();
    }
    if (yytext == "%extern") return T_EXTERN;
    if (yytext == "%token") return T_TOKEN;
    if (yytext == "%class") return T_CLASS;
    return T_ERROR;
  }
  if (ch == '/') {
    if (yychar == '/') {
      for (; yychar; yyinp()) {
        if (yychar == '\n') break;
      }
      goto again;
    }
    if (yychar == '*') {
      yyinp();
      while (yychar) {
        auto ch = yychar;
        yyinp();
        if (ch == '*' && yychar == '/') {
          yyinp();
          break;
        }
      }
      goto again;
    }
    return T_SLASH;
  }
  if (ch == '\'') {
    yytext = ch;
    while (yychar && yychar != '\'' && yychar != '\n') {
      if (yychar == '\\') {
        yytext += yychar;
        yyinp();
        if (!yychar || yychar == '\n') break;
      }
      yytext += yychar;
      yyinp();
    }
    if (yychar != '\'') return T_ERROR;
    yytext += yychar;
    yyinp();
    return T_CHAR_LITERAL;
  }
  if (ch == '{') {
    skip(ch, '{', '}');
    return T_CODE;
  }
  if (ch == '(') {
    auto pos = yypos - 2;
    assert(*pos == '(');
    if (pos != yycode.cbegin() && (isalnum(pos[-1]) || pos[-1] == '_')) {
      skip(ch, '(', ')');
      yytext = std::string(yytext, 1, yytext.length() - 2);
      return T_EXTRA;
    }
    return T_LPAREN;
  }
  if (ch == '#') return T_POUND;
  if (ch == ')') return T_RPAREN;
  if (ch == '*') return T_STAR;
  if (ch == '+') return T_PLUS;
  if (ch == ',') return T_COMMA;
  if (ch == ':') return T_COLON;
  if (ch == ';') return T_SEMICOLON;
  if (ch == '?') return T_QUESTION;
  if (ch == '|') return T_BAR;
  if (isalpha(ch) || ch == '_') {
    while (isalnum(yychar) || yychar == '_') {
      yytext += yychar;
      yyinp();
    }
    return T_IDENTIFIER;
  }
  return T_ERROR;
}

void Lexer::skip(char ch, char left, char right) {
  if (ch == left) {
    int count = 1;
    auto yynext = [this] {
      yytext += yychar;
      yyinp();
    };
    while (auto ch = yychar) {
      yynext();
      if (ch == '/' && yychar == '/') {
        while (yychar && yychar != '\n') yynext();
        continue;
      }
      if (ch == '/' && yychar == '*') {
        yynext();
        while (auto ch = yychar) {
          yynext();
          if (ch == '*' && yychar == '/') {
            yynext();
            break;
          }
        }
        continue;
      }
      if (ch == left) {
        ++count;
      } else if (ch == right) {
        if (!--count) break;
      }
      if (ch == '"' || ch == '\'') {
        auto quote = ch;
        while (auto ch = yychar) {
          yynext();
          if (ch == quote) break;
          if (ch == '\\' && yychar) yynext();
        }
      }
    }
  }
}

Lexer::Lexer(std::string code) : yycode(std::move(code)) {
  yypos = yycode.cbegin();
}

Token Lexer::operator()() {
  Token tk;
  tk.kind = yylex0();
  tk.line = yytokenlineno;
  std::swap(tk.text, yytext);
  return tk;
}

std::vector<Token> Lexer::tokenize(std::string code,
                                   std::vector<Token>* verbatim) {
  Lexer yylex(std::move(code));
  std::vector<Token> tokens;
  Token tk;
  do {
    tk = yylex();
    if (tk.is(T_TEXT)) {
      if (verbatim) verbatim->push_back(std::move(tk));
      continue;
    }
    tokens.push_back(std::move(tk));
  } while (tk.isNot(T_EOF_SYMBOL));
  return tokens;
}
