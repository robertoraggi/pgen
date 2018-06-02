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

#include "pgen/ir.h"

#include <iostream>

namespace IR {

class Print final : IR::StmtVisitor {
 public:
  Print(std::ostream& out);

  void operator()(IR::Stmt* s);

  void setNextBasicBlock(IR::BasicBlock* block);

 private:
  void visit(IR::Exp* s) override;
  void visit(IR::Move* s) override;
  void visit(IR::Save* s) override;
  void visit(IR::Restore* s) override;
  void visit(IR::Ret* s) override;
  void visit(IR::Jump* s) override;
  void visit(IR::CJump* s) override;

 private:
  std::ostream& out;
  IR::BasicBlock* nextBlock_ = nullptr;
};

}  // namespace IR
