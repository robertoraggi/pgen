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

#include "pgen/ir-print.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cassert>
#include <cctype>
#include <iostream>

#include "pgen/flags.h"
#include "pgen/token.h"

namespace IR {

namespace {

std::string tokenName(const std::string& spell) {
  std::string name;
  if (FLAGS_token_type != "int") {
    name = FLAGS_token_type;
    name += "::";
  }
  name += "T_";
  for (auto ch : spell) name += std::toupper(ch);
  return name;
}

}  // namespace

Print::Print(std::ostream& out) : out(out) {}

void Print::operator()(IR::Stmt* s) { s->accept(this); }

void Print::setNextBasicBlock(IR::BasicBlock* block) { nextBlock_ = block; }

void Print::visit(IR::Exp* e) {
  if (auto code = e->expr()->asCode()) {
    fmt::print(out, "\t{0};", code->text());
    out << std::endl;
  } else {
    assert(!"unreachable");
  }
}

void Print::visit(IR::Move* m) {
  fmt::print(out, "{0} = {1};", m->target()->name(), m->source()->name());
  out << std::endl;
}

void Print::visit(IR::Save* s) {
  fmt::print(out, "\t{0} = yycursor;", s->target()->name());
  out << std::endl;
}

void Print::visit(IR::Restore* s) {
  fmt::print(out, "\tif ({0} > yyparsed) yyparsed = {0};", s->source()->name());
  out << std::endl;
  fmt::print(out, "\tyyrewind({0});", s->source()->name());
  out << std::endl;
}

void Print::visit(IR::Ret* r) {
  if (r->result()) {
    out << "\treturn true;" << std::endl;
    return;
  }
  out << "\tyyrewind(yyparsed);" << std::endl;
  out << "\treturn false;" << std::endl;
}

void Print::visit(IR::Jump* j) {
  if (nextBlock_ == j->target()) {
    out << std::endl;
    return;
  }

  fmt::print(out, "\tgoto L{0};", j->target()->index);
  out << std::endl;
}

void Print::visit(IR::CJump* cj) {
  IR::BasicBlock* target = cj->iftrue();
  IR::BasicBlock* cont = cj->iffalse();
  std::string unop = "";
  std::string binop = "==";

  if (nextBlock_ == cj->iftrue()) {
    std::swap(target, cont);
    unop = "!";
    binop = "!=";
  }

  if (auto name = cj->cond()->asName()) {
    if (FLAGS_lines) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", name->sym()->line, FLAGS_input);
      out << std::endl;
    }

    const auto& id = name->sym()->name;

    if (name->sym()->isTerminal) {
      fmt::print(out, "\tif (yytoken() {0} {1}) goto L{2};", binop,
                 tokenName(id), target->index);
    } else {
      fmt::print(out, "\tif ({0}parse_{1}({2})) goto L{3};", unop, id,
                 name->sym()->extra, target->index);
    }
    out << std::endl;
  } else if (auto code = cj->cond()->asCode()) {
    if (FLAGS_lines && code->line() != 1) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", code->line(), FLAGS_input);
      out << std::endl;
    }

    fmt::print(out,
               "\tif ({0}([&]() -> bool {{{1} return true; }})()) goto L{2};",
               unop, code->text(), target->index);

    out << std::endl;
  } else if (auto literal = cj->cond()->asCharLiteral()) {
    if (FLAGS_lines) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", literal->line(), FLAGS_input);
      out << std::endl;
    }

    fmt::print(out, "\tif (yytoken() {0} {1}) goto L{2};", binop,
               literal->value(), target->index);

    out << std::endl;
  } else {
    assert(!"unreachable");
  }

  if (cont != nextBlock_) {
    fmt::print(out, "\tgoto L{0};", cont->index);
    out << std::endl;
  }
}

}  // namespace IR
