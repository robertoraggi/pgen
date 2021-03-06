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

#pragma once

#include <iostream>
#include <string>

enum TOK {
  T_EOF_SYMBOL,
  T_ERROR,
  T_CHAR_LITERAL,
  T_IDENTIFIER,
  T_BAR,
  T_EXTRA,
  T_CODE,
  T_COLON,
  T_LPAREN,
  T_PLUS,
  T_POUND,
  T_QUESTION,
  T_COMMA,
  T_RPAREN,
  T_SEMICOLON,
  T_SLASH,
  T_STAR,
  T_TEXT,
  T_EXTERN,
  T_TOKEN,
  T_CLASS,
  T_EXTENDS,
  T_TOKEN_TYPE,
};

struct Token {
  TOK kind = T_EOF_SYMBOL;
  int line = 0;
  std::string text;

  inline bool is(TOK k) const { return kind == k; }
  inline bool isNot(TOK k) const { return kind != k; }
};

inline std::ostream& operator<<(std::ostream& out, const Token& tk) {
  return out << tk.text;
}
