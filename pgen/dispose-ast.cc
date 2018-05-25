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

#include "pgen/dispose-ast.h"

namespace ast {

void DisposeAST::operator()(ast::Node* node) {
  if (node) node->accept(this);
}

void DisposeAST::visit(ast::Grammar* node) {
  for (auto rule : node->rules) {
    rule->accept(this);
  }
  delete node;
}

void DisposeAST::visit(ast::Rule* node) {
  node->def->accept(this);
  delete node;
}

void DisposeAST::visit(ast::CharLiteral* node) { delete node; }

void DisposeAST::visit(ast::Symbol* node) { delete node; }

void DisposeAST::visit(ast::Code* node) { delete node; }

void DisposeAST::visit(ast::And* node) {
  node->head->accept(this);
  node->tail->accept(this);
  delete node;
}

void DisposeAST::visit(ast::Or* node) {
  node->head->accept(this);
  node->tail->accept(this);
  delete node;
}

void DisposeAST::visit(ast::Lookahead* node) {
  node->head->accept(this);
  node->tail->accept(this);
  delete node;
}

void DisposeAST::visit(ast::Plus* node) {
  node->item->accept(this);
  delete node;
}

void DisposeAST::visit(ast::Star* node) {
  node->item->accept(this);
  delete node;
}

void DisposeAST::visit(ast::Question* node) {
  node->item->accept(this);
  delete node;
}

}  // namespace ast
