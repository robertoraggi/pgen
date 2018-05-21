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

#include "pgen/ir.h"

#include <fmt/format.h>

#include <cassert>

namespace IR {

BasicBlock::BasicBlock(Function* function) : function(function) {}

bool BasicBlock::isTerminated() const { return terminator() != nullptr; }

Stmt* BasicBlock::terminator() const {
  if (empty()) return nullptr;
  if (back()->isTerminator()) return back();
  return nullptr;
}

Name* BasicBlock::NAME(ast::Symbol* sym) {
  function->names.emplace_front(sym);
  return &function->names.front();
}

Code* BasicBlock::CODE(std::string text, int line) {
  function->codes.emplace_front(std::move(text), line);
  return &function->codes.front();
}

CharLiteral* BasicBlock::CHAR_LITERAL(std::string value, int line) {
  function->charLiterals.emplace_front(std::move(value), line);
  return &function->charLiterals.front();
}

void BasicBlock::EXP(Expr* expr) {
  if (isTerminated()) return;
  function->exps.emplace_front(expr);
  push_back(&function->exps.front());
}

void BasicBlock::EXP(std::string text, int line) {
  if (isTerminated()) return;
  function->codes.emplace_front(std::move(text), line);
  auto code = &function->codes.front();
  function->exps.emplace_front(code);
  push_back(&function->exps.front());
}

void BasicBlock::SAVE(Temp* target) {
  if (isTerminated()) return;
  function->saves.emplace_front(target);
  push_back(&function->saves.front());
}

void BasicBlock::RESTORE(Temp* source) {
  if (isTerminated()) return;
  function->restores.emplace_front(source);
  push_back(&function->restores.front());
}

void BasicBlock::MOVE(Expr* target, Expr* source) {
  if (isTerminated()) return;
  function->moves.emplace_front(target, source);
  push_back(&function->moves.front());
}

void BasicBlock::JUMP(BasicBlock* target) {
  if (isTerminated()) return;
  function->jumps.emplace_front(target);
  push_back(&function->jumps.front());
}

void BasicBlock::CJUMP(Expr* cond, BasicBlock* iftrue, BasicBlock* iffalse) {
  if (isTerminated()) return;
  function->cjumps.emplace_front(cond, iftrue, iffalse);
  push_back(&function->cjumps.front());
}

void BasicBlock::RET(bool result) {
  if (isTerminated()) return;
  function->rets.emplace_front(result);
  push_back(&function->rets.front());
}

Temp* Function::newTemp(std::string type) {
  return newTemp(std::move(type), fmt::format("yy{}", uniqueTempCount++));
}

Temp* Function::newTemp(std::string type, std::string name) {
  temps.emplace_front(std::move(type), std::move(name));
  return &temps.front();
}

BasicBlock* Function::newBasicBlock() {
  blocks.emplace_front(this);
  return &blocks.front();
}

void Function::place(BasicBlock* block) {
  assert(block->index == -1);
  block->index = size();
  push_back(block);
}

}  // namespace IR
