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

#include <map>
#include <set>
#include <string>
#include <vector>

namespace ast {

struct Node;
struct Grammar;
struct Rule;
struct CharLiteral;
struct Symbol;
struct Lookahead;
struct Code;
struct Postfix;
struct Pair;
struct And;
struct Or;
struct Plus;
struct Star;
struct Question;

struct Visitor {
  virtual ~Visitor() = default;
  virtual void visit(Grammar*) = 0;
  virtual void visit(Rule*) = 0;
  virtual void visit(CharLiteral*) = 0;
  virtual void visit(Symbol*) = 0;
  virtual void visit(Code*) = 0;
  virtual void visit(And*) = 0;
  virtual void visit(Or*) = 0;
  virtual void visit(Lookahead*) = 0;
  virtual void visit(Plus*) = 0;
  virtual void visit(Star*) = 0;
  virtual void visit(Question*) = 0;
};

struct Node {
  virtual ~Node() = default;
  virtual void accept(Visitor*) = 0;
  virtual Symbol* asSymbol() { return 0; }
};

struct Postfix : Node {
  Node* item;

  Postfix(Node* item) : item(item) {}
};

struct Pair : Node {
  Node* head;
  Node* tail;

  Pair(Node* head, Node* tail) : head(head), tail(tail) {}
};

struct And final : Pair {
  using Pair::Pair;

  void accept(Visitor* v) override { v->visit(this); }
};

struct Or final : Pair {
  using Pair::Pair;

  void accept(Visitor* v) override { v->visit(this); }
};

struct Lookahead final : Pair {
  using Pair::Pair;

  void accept(Visitor* v) override { v->visit(this); }
};

struct Plus final : Postfix {
  using Postfix::Postfix;

  void accept(Visitor* v) override { v->visit(this); }
};

struct Star final : Postfix {
  using Postfix::Postfix;

  void accept(Visitor* v) override { v->visit(this); }
};

struct Question final : Postfix {
  using Postfix::Postfix;

  void accept(Visitor* v) override { v->visit(this); }
};

struct CharLiteral final : Node {
  std::string value;
  int line;

  CharLiteral(std::string value, int line) : value(value), line(line) {}

  void accept(Visitor* v) override { v->visit(this); }
};

struct Symbol final : Node {
  std::string name;
  std::string extra;
  int line;

  Symbol(std::string name, std::string extra, int line)
      : name(std::move(name)), extra(std::move(extra)), line(line) {}

  void accept(Visitor* v) override { v->visit(this); }

  Symbol* asSymbol() override { return this; }
};

struct Code final : Node {
  std::string text;
  int line;

  Code(std::string text, int line) : text(std::move(text)), line(line) {}

  void accept(Visitor* v) override { v->visit(this); }
};

struct Rule final : Node {
  int line = 0;
  std::string lhs;
  std::string extra;
  std::string init;
  Node* def = nullptr;

  void accept(Visitor* v) override { v->visit(this); }
};

struct Grammar final : Node {
  std::vector<Rule*> rules;
  std::set<std::string> non_terminals;
  std::set<std::string> terminals;
  std::set<std::string> names;
  std::map<std::string, std::string> externals;
  std::set<std::string> undef;

  void accept(Visitor* v) override { v->visit(this); }
};

}  // namespace ast