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

#include "pgen/gen-parser.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cctype>
#include <set>

#include "pgen/flags.h"
#include "pgen/ir-print.h"

namespace {

void print(std::ostream& out, IR::Function* fun) {
  IR::Print print(out);

  for (auto block : *fun) {
    fmt::print(out, "L{0}:", block->index);

    if (size_t(block->index + 1) != fun->size())
      print.setNextBasicBlock(fun->at(block->index + 1));
    else
      print.setNextBasicBlock(0);

    for (auto s : *block) print(s);
  }
}

}  // namespace

GenParser::GenParser(std::ostream& out) : CodeGenerator(out) {}

void GenParser::visit(ast::Grammar* grammar) {
  fmt::print(out, "struct {0}", FLAGS_parser_name);
  if (!FLAGS_extends.empty()) {
    fmt::print(out, ": {0}", FLAGS_extends);
  }
  fmt::print(out, " {{");
  out << std::endl;

  for (const auto& tk : *verbatim_) {
    if (FLAGS_lines) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", tk.line, FLAGS_input);
      out << std::endl;
    }
    out << tk.text << std::endl;
  }

  for (const auto& [name, extra] : grammar->externals) {
    fmt::print(out, "  bool parse_{0}({1});", name, extra);
    out << std::endl;
  }

  if (!grammar->externals.empty()) std::cout << std::endl;

  for (auto rule : grammar->rules) {
    if (FLAGS_lines) {
      fmt::print(out, "#line {0} \"{1}\"", rule->line, FLAGS_input);
      out << std::endl;
    }
    fmt::print(out, "  bool parse_{0}({1});", rule->lhs, rule->extra);
    out << std::endl;
  }

  // clang-format off
  out << std::endl;
  out << "  bool yyinvalid = true;" << std::endl;
  out << "  " << FLAGS_token_type << " yytok{};" << std::endl;
  out << "  inline " << FLAGS_token_type<< " yytoken() { if (yyinvalid) { yytok = yytoken(0); yyinvalid = false; } return yytok; }" << std::endl;
  out << "  inline void yyconsume() { ++yycursor; yyinvalid = true; }" << std::endl;
  out << "  inline void yyrewind(int i) { if (yycursor == i) return; yycursor = i; yyinvalid = true; }" << std::endl;
  out << "  " << FLAGS_token_type << " yytoken(int index);" << std::endl;
  out << std::endl;
  out << "  int yydepth = -1;" << std::endl;
  out << "  unsigned yycursor = 0;" << std::endl;
  out << "};" << std::endl;
  // clang-format on

  for (auto rule : grammar->rules) rule->accept(this);
}

void GenParser::visit(ast::Rule* rule) {
  IR::Function f;

  setFunction(&f);

  auto entryBlock = newBasicBlock();
  auto iftrue = newBasicBlock();
  auto iffalse = newBasicBlock();

  place(entryBlock);
  condition(rule->def, iftrue, iffalse);
  place(iffalse);
  ret(false);
  place(iftrue);
  ret(true);

  out << std::endl;

  if (FLAGS_lines) {
    out << std::endl;
    fmt::print(out, "#line {0} \"{1}\"", rule->line, FLAGS_input);
    out << std::endl;
  }

  fmt::print(out, "bool {0}::parse_{1}({2}) {{", FLAGS_parser_name, rule->lhs,
             rule->extra);
  out << std::endl;

  std::set<std::string> p;

  if (!rule->init.empty()) {
    out << "\t" << rule->init.substr(1, rule->init.length() - 2) << std::endl;
  }

  for (auto& temp : function()->temps) {
    if (!p.insert(temp.name()).second) continue;
    fmt::print(out, "\t{0} {1};", temp.type(), temp.name());
    out << std::endl;
  }

  out << "\tgoto L0;" << std::endl;

  print(out, &f);

  out << "}" << std::endl;
}

void GenParser::visit(ast::CharLiteral* literal) {
  auto iftrue = newBasicBlock();

  cjump(getCharLiteral(literal->value, literal->line), iftrue, code.iffalse);
  place(iftrue);
  exp("yyconsume()");
  jump(code.iftrue);
}

void GenParser::visit(ast::Symbol* sym) {
  if (sym->isTerminal) {
    auto iftrue = newBasicBlock();

    cjump(getName(sym), iftrue, code.iffalse);
    place(iftrue);
    exp("yyconsume()");
    jump(code.iftrue);
    return;
  }

  auto temp = newTemp();
  save(temp);

  auto iffalse = newBasicBlock();

  cjump(getName(sym), code.iftrue, iffalse);

  place(iffalse);
  restore(temp);
  jump(code.iffalse);
}

void GenParser::visit(ast::Code* node) {
  cjump(getCode(node->text, node->line), code.iftrue, code.iffalse);
}

void GenParser::visit(ast::Lookahead* node) {
  auto iftrue = newBasicBlock();
  auto cleanup_iftrue = newBasicBlock();
  auto cleanup_iffalse = newBasicBlock();
  auto target = newTemp();

  condition(node->head, iftrue, code.iffalse);
  place(iftrue);
  save(target);
  condition(node->tail, cleanup_iftrue, cleanup_iffalse);
  place(cleanup_iftrue);
  restore(target);
  jump(code.iftrue);
  place(cleanup_iffalse);
  restore(target);
  jump(code.iffalse);
}

void GenParser::visit(ast::And* node) {
  auto iftrue = newBasicBlock();

  condition(node->head, iftrue, code.iffalse);
  place(iftrue);
  condition(node->tail, code.iftrue, code.iffalse);
}

void GenParser::visit(ast::Or* node) {
  auto iffalse = newBasicBlock();
  auto target = newTemp();

  save(target);
  condition(node->head, code.iftrue, iffalse);
  place(iffalse);
  restore(target);
  condition(node->tail, code.iftrue, code.iffalse);
}

void GenParser::visit(ast::Plus* node) {
  auto more = newBasicBlock();
  auto iftrue = newBasicBlock();
  auto iffalse = newBasicBlock();
  auto target = newTemp();

  condition(node->item, more, code.iffalse);
  place(more);
  jump(iftrue);
  place(iftrue);
  save(target);
  condition(node->item, iftrue, iffalse);
  place(iffalse);
  restore(target);
  jump(code.iftrue);
}

void GenParser::visit(ast::Star* node) {
  auto iftrue = newBasicBlock();
  auto iffalse = newBasicBlock();
  auto target = newTemp();

  jump(iftrue);
  place(iftrue);
  save(target);
  condition(node->item, iftrue, iffalse);
  place(iffalse);
  restore(target);
  jump(code.iftrue);
}

void GenParser::visit(ast::Question* node) {
  auto iffalse = newBasicBlock();
  auto target = newTemp();

  save(target);
  condition(node->item, code.iftrue, iffalse);
  place(iffalse);
  restore(target);
  jump(code.iftrue);
}

void GenParser::operator()(ast::Grammar* grammar,
                           const std::vector<Token>& verbatim) {
  grammar_ = grammar;
  verbatim_ = &verbatim;

  grammar->accept(this);
}
