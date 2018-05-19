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

#include <map>
#include <set>
#include <vector>

#include "pgen/ast.h"
#include "pgen/token.h"

class Parser {
 public:
  using token_list = std::vector<Token>;
  using iterator = token_list::const_iterator;

  Parser(std::string yyfilename, token_list tokens);

  ast::Grammar* operator()();

 private:
  bool yymatch(TOK tk);
  iterator yyexpect(TOK tk);
  iterator yyconsume();

  void parseDirectives();
  void parseRules();

  void parseTokens();
  void parseClass();
  void parseExternSymbols();

  ast::Rule* parseRule();
  ast::Node* parseDefinition();
  ast::Node* parseOr();
  ast::Node* parseAnd();
  ast::Node* parsePostfix();
  ast::Node* parseId();
  ast::Node* parsePrimary();

  bool lookAtNode();

 private:
  ast::Grammar* grammar_ = nullptr;
  std::string yyfilename;
  std::vector<Token> yytokens;
  iterator yytoken;
};
