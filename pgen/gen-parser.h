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

#include "pgen/code-generator.h"
#include "pgen/token.h"

#include <iostream>

class GenParser final : CodeGenerator, IR::IRBuilder {
 public:
  GenParser(std::ostream& out);

  void operator()(ast::Grammar* g, const std::vector<Token>& verbatim);

 private:
  void visit(ast::Grammar* node) override;
  void visit(ast::Rule* node) override;
  void visit(ast::CharLiteral* node) override;
  void visit(ast::Symbol* node) override;
  void visit(ast::Code* node) override;
  void visit(ast::And* node) override;
  void visit(ast::Or* node) override;
  void visit(ast::Lookahead* node) override;
  void visit(ast::Plus* node) override;
  void visit(ast::Star* node) override;
  void visit(ast::Question* node) override;

 private:
  ast::Grammar* grammar_ = nullptr;
  const std::vector<Token>* verbatim_ = nullptr;
};
