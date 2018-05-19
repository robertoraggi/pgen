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
  return &function->names.emplace_front(sym);
}

Code* BasicBlock::CODE(std::string text, int line) {
  return &function->codes.emplace_front(std::move(text), line);
}

void BasicBlock::EXP(Expr* expr) {
  if (isTerminated()) return;
  push_back(&function->exps.emplace_front(expr));
}

void BasicBlock::EXP(std::string text, int line) {
  if (isTerminated()) return;
  auto code = &function->codes.emplace_front(std::move(text), line);
  push_back(&function->exps.emplace_front(code));
}

void BasicBlock::SAVE(Temp* target) {
  if (isTerminated()) return;
  push_back(&function->saves.emplace_front(target));
}

void BasicBlock::RESTORE(Temp* source) {
  if (isTerminated()) return;
  push_back(&function->restores.emplace_front(source));
}

void BasicBlock::MOVE(Expr* target, Expr* source) {
  if (isTerminated()) return;
  push_back(&function->moves.emplace_front(target, source));
}

void BasicBlock::JUMP(BasicBlock* target) {
  if (isTerminated()) return;
  push_back(&function->jumps.emplace_front(target));
}

void BasicBlock::CJUMP(Expr* cond, BasicBlock* iftrue, BasicBlock* iffalse) {
  if (isTerminated()) return;
  push_back(&function->cjumps.emplace_front(cond, iftrue, iffalse));
}

void BasicBlock::RET(bool result) {
  if (isTerminated()) return;
  push_back(&function->rets.emplace_front(result));
}

Temp* Function::newTemp(std::string type) {
  return newTemp(std::move(type), fmt::format("yy{}", uniqueTempCount++));
}

Temp* Function::newTemp(std::string type, std::string name) {
  return &temps.emplace_front(std::move(type), std::move(name));
}

BasicBlock* Function::newBasicBlock() { return &blocks.emplace_front(this); }

void Function::place(BasicBlock* block) {
  assert(block->index == -1);
  block->index = size();
  push_back(block);
}

}  // namespace IR
