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

#include "pgen/classify-symbols.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <iostream>

void ClassifySymbols::operator()(ast::Grammar* grammar) {
  grammar->accept(this);

  for (auto&& n : grammar->names) {
    if (grammar->non_terminals.find(n) != grammar->non_terminals.end())
      continue;

    if (grammar->externals.find(n) != grammar->externals.end()) continue;

    if (grammar->terminals.find(n) != grammar->terminals.end()) continue;

    grammar->undef.insert(n);
  }

  for (auto&& u : grammar->undef) {
    fmt::print(std::cerr, "undefined symbol '{}'", u);
    std::cerr << std::endl;
  }
}

void ClassifySymbols::visit(ast::Grammar* node) {
  grammar_ = node;
  for (auto rule : node->rules) rule->accept(this);
}

void ClassifySymbols::visit(ast::Rule* rule) {
  grammar_->non_terminals.emplace(rule->lhs);
  if (rule->def) rule->def->accept(this);
}

void ClassifySymbols::visit(ast::CharLiteral*) {}

void ClassifySymbols::visit(ast::Symbol* sym) {
  grammar_->names.emplace(sym->name);

  if (grammar_->terminals.find(sym->name) != grammar_->terminals.end())
    sym->isTerminal = true;
}

void ClassifySymbols::visit(ast::Code*) {}

void ClassifySymbols::visit(ast::And* p) {
  p->head->accept(this);
  p->tail->accept(this);
}

void ClassifySymbols::visit(ast::Or* p) {
  p->head->accept(this);
  p->tail->accept(this);
}

void ClassifySymbols::visit(ast::Lookahead* p) {
  p->head->accept(this);
  p->tail->accept(this);
}

void ClassifySymbols::visit(ast::Plus* p) { p->item->accept(this); }

void ClassifySymbols::visit(ast::Star* p) { p->item->accept(this); }

void ClassifySymbols::visit(ast::Question* p) { p->item->accept(this); }
