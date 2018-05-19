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

#include <cassert>
#include <cctype>

TOK Lexer::yylex0() {
again:
  while (isspace(yychar)) yyinp();
  yytokenlineno = yylineno;
  yytext.clear();
  if (!yychar) return TOK::EOF_SYMBOL;
  auto ch = yychar;
  yytext += ch;
  yyinp();
  if (ch == '%' && yychar == '{') {
    yyinp();
    skip('{', '{', '}');
    yytext = std::string(yytext, 2, yytext.length() - 4);
    return TOK::TEXT;
  }
  if (ch == '%' && isalpha(yychar)) {
    while (isalnum(yychar) || yychar == '_') {
      yytext += yychar;
      yyinp();
    }
    if (yytext == "%extern") return TOK::EXTERN;
    if (yytext == "%token") return TOK::TOKEN;
    if (yytext == "%class") return TOK::CLASS;
    return TOK::ERROR;
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
    return TOK::SLASH;
  }
  if (ch == '{') {
    skip(ch, '{', '}');
    return TOK::CODE;
  }
  if (ch == '(') {
    auto pos = yypos - 2;
    assert(*pos == '(');
    if (pos != yycode.cbegin() && (isalnum(pos[-1]) || pos[-1] == '_')) {
      skip(ch, '(', ')');
      yytext = std::string(yytext, 1, yytext.length() - 2);
      return TOK::EXTRA;
    }
    return TOK::LPAREN;
  }
  if (ch == '#') return TOK::POUND;
  if (ch == ')') return TOK::RPAREN;
  if (ch == '*') return TOK::STAR;
  if (ch == '+') return TOK::PLUS;
  if (ch == ',') return TOK::COMMA;
  if (ch == ':') return TOK::COLON;
  if (ch == ';') return TOK::SEMICOLON;
  if (ch == '?') return TOK::QUESTION;
  if (ch == '|') return TOK::BAR;
  if (isalpha(ch) || ch == '_') {
    while (isalnum(yychar) || yychar == '_') {
      yytext += yychar;
      yyinp();
    }
    return TOK::IDENTIFIER;
  }
  return TOK::ERROR;
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
    if (tk.is(TOK::TEXT)) {
      if (verbatim) verbatim->push_back(std::move(tk));
      continue;
    }
    tokens.push_back(std::move(tk));
  } while (tk.isNot(TOK::EOF_SYMBOL));
  return tokens;
}
