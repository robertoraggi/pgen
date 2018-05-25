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
#include <tuple>
#include <vector>

namespace IR {

class IRBuilder;
class Function;

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
struct CharLiteral;

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
  virtual void visit(const Name*) = 0;
  virtual void visit(const Temp*) = 0;
  virtual void visit(const Code*) = 0;
  virtual void visit(const CharLiteral*) = 0;
};

struct Expr {
  virtual ~Expr() = default;
  virtual void accept(ExprVisitor*) const = 0;
  virtual const Name* asName() const { return nullptr; }
  virtual const Temp* asTemp() const { return nullptr; }
  virtual const Code* asCode() const { return nullptr; }
  virtual const CharLiteral* asCharLiteral() const { return nullptr; }
};

struct Stmt {
  virtual ~Stmt() = default;
  virtual void accept(StmtVisitor*) = 0;
  virtual bool isTerminator() const { return false; }
};

struct Name final : Expr, std::tuple<ast::Symbol*> {
  using tuple::tuple;

  ast::Symbol* sym() const { return std::get<0>(*this); }

  void accept(ExprVisitor* v) const override { v->visit(this); }

  const Name* asName() const override { return this; }
};

struct Temp final : Expr, std::tuple<std::string, std::string> {
  using tuple::tuple;

  const std::string& type() const {
    static std::string kDefaultType{"unsigned"};
    const std::string& ty = std::get<0>(*this);
    return !ty.empty() ? ty : kDefaultType;
  }

  const std::string& name() const { return std::get<1>(*this); }

  void accept(ExprVisitor* v) const override { v->visit(this); }

  const Temp* asTemp() const override { return this; }
};

struct Code final : Expr, std::tuple<std::string, int> {
  using tuple::tuple;

  const std::string& text() const { return std::get<0>(*this); }
  int line() const { return std::get<1>(*this); }

  void accept(ExprVisitor* v) const override { v->visit(this); }

  const Code* asCode() const override { return this; }
};

struct CharLiteral final : Expr, std::tuple<std::string, int> {
  using tuple::tuple;

  const std::string& value() const { return std::get<0>(*this); }
  int line() const { return std::get<1>(*this); }

  void accept(ExprVisitor* v) const override { v->visit(this); }

  const CharLiteral* asCharLiteral() const override { return this; }
};

struct Exp final : Stmt, std::tuple<const Expr*> {
  using tuple::tuple;

  const Expr* expr() const { return std::get<0>(*this); }

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Move final : Stmt, std::tuple<const Expr*, const Expr*> {
  using tuple::tuple;

  const Expr* target() const { return std::get<0>(*this); }
  const Expr* source() const { return std::get<0>(*this); }

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Save final : Stmt, std::tuple<const Temp*> {
  using tuple::tuple;

  const Temp* target() const { return std::get<0>(*this); }

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Restore final : Stmt, std::tuple<const Temp*> {
  using tuple::tuple;

  const Temp* source() const { return std::get<0>(*this); }

  void accept(StmtVisitor* v) override { v->visit(this); }
};

struct Ret final : Stmt, std::tuple<bool> {
  using tuple::tuple;

  bool result() const { return std::get<0>(*this); }

  void accept(StmtVisitor* v) override { v->visit(this); }

  bool isTerminator() const override { return true; }
};

struct Jump final : Stmt, std::tuple<BasicBlock*> {
  using tuple::tuple;

  BasicBlock* target() const { return std::get<0>(*this); }

  void accept(StmtVisitor* v) override { v->visit(this); }

  bool isTerminator() const override { return true; }
};

struct CJump final : Stmt, std::tuple<const Expr*, BasicBlock*, BasicBlock*> {
  using tuple::tuple;

  const Expr* cond() const { return std::get<0>(*this); }
  BasicBlock* iftrue() const { return std::get<1>(*this); }
  BasicBlock* iffalse() const { return std::get<2>(*this); }

  void accept(StmtVisitor* v) override { v->visit(this); }

  bool isTerminator() const override { return true; }
};

struct BasicBlock final : std::vector<Stmt*> {
  Function* function;
  int index = -1;

  BasicBlock(Function* function);

  bool isTerminated() const;
  Stmt* terminator() const;
};

class Function final : public std::vector<BasicBlock*> {
 public:
  using vector::vector;

  std::set<Temp> temps;

 private:
  friend class IRBuilder;

  std::set<Name> names_;
  std::set<CharLiteral> charLiterals_;
  std::set<Code> codes_;

  std::forward_list<Exp> exps_;
  std::forward_list<Move> moves_;
  std::forward_list<Save> saves_;
  std::forward_list<Restore> restores_;
  std::forward_list<Ret> rets_;
  std::forward_list<Jump> jumps_;
  std::forward_list<CJump> cjumps_;
  std::forward_list<BasicBlock> blocks_;
  unsigned uniqueTempCount_ = 0;
};

class IRBuilder {
 public:
  IRBuilder() = default;
  IRBuilder(Function* function);

  Function* function() const;
  void setFunction(Function* function);

  BasicBlock* newBasicBlock();

  const Temp* newTemp(std::string type = std::string());
  const Temp* getTemp(std::string type, std::string name);
  const Name* getName(ast::Symbol* sym);
  const Code* getCode(std::string text, int line);
  const CharLiteral* getCharLiteral(std::string value, int line);

  void place(BasicBlock* block);
  void exp(const Expr* expr);
  void exp(std::string text, int line = -1);
  void save(const Temp* target);
  void restore(const Temp* source);
  void move(const Expr* target, const Expr* source);
  void jump(BasicBlock* target);
  void cjump(const Expr* cond, BasicBlock* iftrue, BasicBlock* iffalse);
  void ret(bool result);

 private:
  Function* function_ = nullptr;
  BasicBlock* block_ = nullptr;
};

}  // namespace IR
