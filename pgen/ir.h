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

#include "pgen/ast.h"

#include <forward_list>
#include <vector>

namespace IR {

struct Function;
struct BasicBlock;
// statements
struct Stmt;
struct Exp;
struct Code;
struct Move;
struct Save;
struct Restore;
struct Ret;
struct Jump;
struct CJump;
// expressions
struct Expr;
struct Name;
struct Temp;

struct StmtVisitor {
  virtual ~StmtVisitor() = default;
  virtual void visit(Exp*) = 0;
  virtual void visit(Move*) = 0;
  virtual void visit(Save*) = 0;
  virtual void visit(Restore*) = 0;
  virtual void visit(Ret*) = 0;
  virtual void visit(Jump*) = 0;
  virtual void visit(CJump*) = 0;
};

struct ExprVisitor {
  virtual ~ExprVisitor() = default;
  virtual void visit(Name*) = 0;
  virtual void visit(Temp*) = 0;
  virtual void visit(Code*) = 0;
};

struct Expr {
  virtual ~Expr() = default;
  virtual void accept(ExprVisitor*) = 0;
  virtual Name* asName() { return 0; }
  virtual Temp* asTemp() { return 0; }
  virtual Code* asCode() { return 0; }
};

struct Stmt {
  virtual ~Stmt() = default;
  virtual void accept(StmtVisitor*) = 0;
  virtual bool isTerminator() const { return false; }
};

struct Name final : Expr {
  ast::Symbol* sym;

  Name(ast::Symbol* sym) : sym(sym) {}

  void accept(ExprVisitor* v) override { v->visit(this); }

  Name* asName() override final { return this; }
};

struct Temp final : Expr {
  std::string type;
  std::string name;

  Temp(std::string type, std::string name)
      : type(std::move(type)), name(std::move(name)) {
    if (this->type.empty()) this->type = "unsigned";
  }

  void accept(ExprVisitor* v) override { v->visit(this); }

  Temp* asTemp() override final { return this; }
};

struct Code final : Expr {
  std::string text;
  int line;

  Code(std::string text, int line = -1) : text(std::move(text)), line(line) {}

  void accept(ExprVisitor* v) override { v->visit(this); }

  Code* asCode() override { return this; }
};

struct Exp final : Stmt {
  Expr* expr;

  Exp(Expr* expr) : expr(expr) {}

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Move final : Stmt {
  Expr* target;
  Expr* source;

  Move(Expr* target, Expr* source) : target(target), source(source) {}

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Save final : Stmt {
  Temp* target;

  Save(Temp* target) : target(target) {}

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Restore final : Stmt {
  Temp* source;

  Restore(Temp* source) : source(source) {}

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Ret final : Stmt {
  bool result;

  Ret(bool result) : result(result) {}

  void accept(StmtVisitor* v) override { v->visit(this); }

  bool isTerminator() const override { return true; }
};

struct Jump final : Stmt {
  BasicBlock* target;

  Jump(BasicBlock* target) : target(target) {}

  void accept(StmtVisitor* v) override { v->visit(this); }

  bool isTerminator() const override { return true; }
};

struct CJump final : Stmt {
  Expr* cond;
  BasicBlock* iftrue;
  BasicBlock* iffalse;

  CJump(Expr* cond, BasicBlock* iftrue, BasicBlock* iffalse)
      : cond(cond), iftrue(iftrue), iffalse(iffalse) {}

  void accept(StmtVisitor* v) override { v->visit(this); }

  bool isTerminator() const override { return true; }
};

struct BasicBlock final : std::vector<Stmt*> {
  Function* function;
  int index = -1;

  BasicBlock(Function* function);

  bool isTerminated() const;
  Stmt* terminator() const;

  Name* NAME(ast::Symbol* sym);
  Code* CODE(std::string text, int line = -1);

  void EXP(Expr* expr);
  void EXP(std::string text, int line = -1);
  void SAVE(Temp* target);
  void RESTORE(Temp* source);
  void MOVE(Expr* target, Expr* source);
  void JUMP(BasicBlock* target);
  void CJUMP(Expr* cond, BasicBlock* iftrue, BasicBlock* iffalse);
  void RET(bool result);
};

struct Function final : std::vector<BasicBlock*> {
  std::forward_list<Exp> exps;
  std::forward_list<Code> codes;
  std::forward_list<Move> moves;
  std::forward_list<Save> saves;
  std::forward_list<Restore> restores;
  std::forward_list<Ret> rets;
  std::forward_list<Jump> jumps;
  std::forward_list<CJump> cjumps;
  std::forward_list<Name> names;
  std::forward_list<Temp> temps;
  std::forward_list<BasicBlock> blocks;
  unsigned uniqueTempCount = 0;

  Temp* newTemp(std::string type = std::string());
  Temp* newTemp(std::string type, std::string name);

  BasicBlock* newBasicBlock();

  void place(BasicBlock* block);
};

}  // namespace IR
