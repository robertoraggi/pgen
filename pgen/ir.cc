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

const Name* IRBuilder::getName(ast::Symbol* sym) {
  return &*function_->names_.emplace(sym).first;
}

const Code* IRBuilder::getCode(std::string text, int line) {
  return &*function_->codes_.emplace(std::move(text), line).first;
}

const CharLiteral* IRBuilder::getCharLiteral(std::string value, int line) {
  return &*function_->charLiterals_.emplace(std::move(value), line).first;
}

void IRBuilder::exp(const Expr* expr) {
  if (block_->isTerminated()) return;
  function_->exps_.emplace_front(expr);
  block_->push_back(&function_->exps_.front());
}

void IRBuilder::exp(std::string text, int line) {
  if (block_->isTerminated()) return;
  function_->exps_.emplace_front(getCode(std::move(text), line));
  block_->push_back(&function_->exps_.front());
}

void IRBuilder::save(const Temp* target) {
  if (block_->isTerminated()) return;
  function_->saves_.emplace_front(target);
  block_->push_back(&function_->saves_.front());
}

void IRBuilder::restore(const Temp* source) {
  if (block_->isTerminated()) return;
  function_->restores_.emplace_front(source);
  block_->push_back(&function_->restores_.front());
}

void IRBuilder::move(const Temp* target, const Temp* source) {
  if (block_->isTerminated()) return;
  function_->moves_.emplace_front(target, source);
  block_->push_back(&function_->moves_.front());
}

void IRBuilder::jump(BasicBlock* target) {
  if (block_->isTerminated()) return;
  function_->jumps_.emplace_front(target);
  block_->push_back(&function_->jumps_.front());
}

void IRBuilder::cjump(const Expr* cond, BasicBlock* iftrue,
                      BasicBlock* iffalse) {
  if (block_->isTerminated()) return;
  function_->cjumps_.emplace_front(cond, iftrue, iffalse);
  block_->push_back(&function_->cjumps_.front());
}

void IRBuilder::ret(bool result) {
  if (block_->isTerminated()) return;
  function_->rets_.emplace_front(result);
  block_->push_back(&function_->rets_.front());
}

const Temp* IRBuilder::newTemp(std::string type) {
  return getTemp(std::move(type),
                 fmt::format("yy{}", function_->uniqueTempCount_++));
}

const Temp* IRBuilder::getTemp(std::string type, std::string name) {
  return &*function_->temps.emplace(std::move(type), std::move(name)).first;
}

Function* IRBuilder::function() const { return function_; }

void IRBuilder::setFunction(Function* function) { function_ = function; }

void IRBuilder::place(BasicBlock* block) {
  assert(block);
  assert(block->function == function_);
  assert(block->index == -1);
  block_ = block;
  block_->index = function_->size();
  function_->push_back(block_);
}

BasicBlock* IRBuilder::newBasicBlock() {
  function_->blocks_.emplace_front(function_);
  return &function_->blocks_.front();
}

}  // namespace IR
