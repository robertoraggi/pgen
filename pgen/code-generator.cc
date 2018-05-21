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

#include "pgen/code-generator.h"

#include <cassert>

CodeGenerator::CodeGenerator(std::ostream& out) : out(out) {}

IR::Temp* CodeGenerator::newTemp(std::string type) {
  return function->newTemp(std::move(type));
}

IR::Temp* CodeGenerator::newTemp(std::string type, std::string name) {
  return function->newTemp(std::move(type), std::move(name));
}

IR::BasicBlock* CodeGenerator::newBasicBlock() {
  return function->newBasicBlock();
}

void CodeGenerator::place(IR::BasicBlock* block) {
  function->place(block);
  this->block = block;
}

void CodeGenerator::condition(ast::Node* item, IR::BasicBlock* iftrue,
                              IR::BasicBlock* iffalse) {
  Result r(iftrue, iffalse);
  std::swap(code, r);
  item->accept(this);
  std::swap(code, r);
  assert(block->isTerminated());
}