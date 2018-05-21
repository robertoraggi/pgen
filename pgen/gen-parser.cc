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
#include "pgen/flags.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <cctype>
#include <set>

namespace {

std::string tokenName(const std::string& t) {
  std::string name = "T_";
  for (auto&& x : t) name += std::toupper(x);
  return name;
}

std::string indent(const std::string& text) {
  std::string prefix;

  size_t i = 0;
  for (; i < text.size(); ++i) {
    if (text[i] != '\n') break;
  }

  for (; i < text.size(); ++i) {
    if (text[i] == '\n' || !std::isspace(text[i])) break;
    prefix += text[i];
  }

  std::string out;

  for (size_t i = 0; i < text.size();) {
    if (text[i] == '\n' && text.compare(i + 1, prefix.size(), prefix) == 0) {
      i += prefix.size() + 1;
      out += "\n\t";
    } else {
      out += text[i++];
    }
  }

  return out;
}

}  // namespace

class Print final : IR::ExprVisitor, IR::StmtVisitor {
 public:
  Print(std::ostream& out, ast::Grammar* grammar)
      : out(out), grammar(grammar) {}

  void operator()(IR::Stmt* s) { s->accept(this); }

  void setNextBasicBlock(IR::BasicBlock* block) { nextBlock = block; }

 private:
  void visit(IR::Exp* e) {
    if (auto c = e->expr->asCode()) {
      out << '\t' << c->text << ';' << std::endl;
    } else {
      assert(!"unreachable");
    }
  }

  void visit(IR::Code*) { assert(!"unreachable"); }
  void visit(IR::CharLiteral*) { assert(!"unreachable"); }

  void visit(IR::Move* m) {
    m->target->accept(this);
    out << " = ";
    m->source->accept(this);
    out << ';' << std::endl;
  }

  void visit(IR::Save* s) {
    out << "\t";
    s->target->accept(this);
    out << " = yycursor;" << std::endl;
  }

  void visit(IR::Restore* s) {
    out << "\tif (";
    s->source->accept(this);
    out << " > yyparsed) yyparsed = ";
    s->source->accept(this);
    out << ";" << std::endl;
    out << "\tyyrewind(";
    s->source->accept(this);
    out << ");" << std::endl;
  }

  void visit(IR::Ret* r) {
    if (r->result) {
      out << "\treturn true;" << std::endl;
      return;
    }
    out << "\tyyrewind(yyparsed);" << std::endl;
    out << "\treturn false;" << std::endl;
  }

  void visit(IR::Jump* j) {
    if (nextBlock == j->target) {
      out << std::endl;
      return;
    }

    fmt::print(out, "\tgoto L{0};", j->target->index);
    out << std::endl;
  }

  void outCJump(IR::CJump* cj, IR::Name* name) {
    IR::BasicBlock* target = cj->iftrue;
    IR::BasicBlock* cont = cj->iffalse;
    std::string unop = "";
    std::string binop = "==";

    if (nextBlock == cj->iftrue) {
      std::swap(target, cont);
      unop = "!";
      binop = "!=";
    }

    if (FLAGS_lines) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", name->sym->line, FLAGS_input);
      out << std::endl;
    }

    const auto& id = name->sym->name;

    if (grammar->terminals.find(id) != grammar->terminals.end()) {
      fmt::print(out, "\tif (yytoken() {0} {1}) goto L{2};", binop,
                 tokenName(id), target->index);
    } else {
      fmt::print(out, "\tif ({0}parse_{1}({2})) goto L{3};", unop, id,
                 name->sym->extra, target->index);
    }

    out << std::endl;

    if (cont != nextBlock) {
      fmt::print(out, "\tgoto L{0};", cont->index);
      out << std::endl;
    }
  }

  void outCJump(IR::CJump* cj, IR::Code* c) {
    IR::BasicBlock* target = cj->iftrue;
    IR::BasicBlock* cont = cj->iffalse;
    std::string unop = "";

    if (nextBlock == cj->iftrue) {
      std::swap(target, cont);
      unop = "!";
    }

    if (FLAGS_lines) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", c->line, FLAGS_input);
      out << std::endl;
    }

    fmt::print(out,
               "\tif ({0}([&]() -> bool {{{1} return true; }})()) goto L{2};",
               unop, indent(c->text), target->index);

    out << std::endl;

    if (cont != nextBlock) {
      fmt::print(out, "\tgoto L{0};", cont->index);
      out << std::endl;
    }
  }

  void outCJump(IR::CJump* cj, IR::CharLiteral* literal) {
    IR::BasicBlock* target = cj->iftrue;
    IR::BasicBlock* cont = cj->iffalse;
    std::string unop = "";
    std::string binop = "==";

    if (nextBlock == cj->iftrue) {
      std::swap(target, cont);
      unop = "!";
      binop = "!=";
    }

    if (FLAGS_lines) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", literal->line, FLAGS_input);
      out << std::endl;
    }

    fmt::print(out, "\tif (yytoken() {0} {1}) goto L{2};", binop,
               literal->value, target->index);

    out << std::endl;

    if (cont != nextBlock) {
      fmt::print(out, "\tgoto L{0};", cont->index);
      out << std::endl;
    }
  }

  void visit(IR::CJump* cj) {
    if (auto name = cj->cond->asName()) {
      outCJump(cj, name);
    } else if (auto code = cj->cond->asCode()) {
      outCJump(cj, code);
    } else if (auto charLiteral = cj->cond->asCharLiteral()) {
      outCJump(cj, charLiteral);
    } else {
      assert(!"unreachable");
    }
  }

  void visit(IR::Name*) { assert(!"unreachable"); }

  void visit(IR::Temp* temp) { out << temp->name; }

 private:
  std::ostream& out;
  ast::Grammar* grammar = nullptr;
  IR::BasicBlock* nextBlock = nullptr;
};

namespace {

void print(std::ostream& out, ast::Grammar* grammar, IR::Function* fun) {
  Print print(out, grammar);

  for (auto&& block : *fun) {
    fmt::print(out, "L{0}:", block->index);

    if (size_t(block->index + 1) != fun->size())
      print.setNextBasicBlock(fun->at(block->index + 1));
    else
      print.setNextBasicBlock(0);

    for (auto&& s : *block) print(s);
  }
}

}  // namespace

GenParser::GenParser(std::ostream& out) : CodeGenerator(out) {}

void GenParser::visit(ast::Grammar* grammar) {
  fmt::print(out, "struct {0} {{", FLAGS_parser_name);
  out << std::endl;

  for (auto&& tk : *verbatim_) {
    if (FLAGS_lines) {
      out << std::endl;
      fmt::print(out, "#line {0} \"{1}\"", tk.line, FLAGS_input);
      out << std::endl;
    }
    out << tk.text << std::endl;
  }

  for (auto&& decl : grammar->externals) {
    fmt::print(out, "  bool parse_{0}({1});", decl.first, decl.second);
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
  out << "  int yytok = 0;" << std::endl;
  out << "  inline int yytoken() { if (yyinvalid) { yytok = yytoken(0); yyinvalid = false; } return yytok; }" << std::endl;
  out << "  inline void yyconsume() { ++yycursor; yyinvalid = true; }" << std::endl;
  out << "  inline void yyrewind(int i) { if (yycursor == i) return; yycursor = i; yyinvalid = true; }" << std::endl;
  out << "  int yytoken(int index);" << std::endl;
  out << std::endl;
  out << "  unsigned yyparsed = 0;" << std::endl;
  out << "  int yydepth = -1;" << std::endl;
  out << "  unsigned yycursor = 0;" << std::endl;
  out << "};" << std::endl;
  // clang-format on

  for (auto rule : grammar->rules) rule->accept(this);
}

void GenParser::visit(ast::Rule* rule) {
  IR::Function f;
  function = &f;

  auto entryBlock = newBasicBlock();
  auto iftrue = newBasicBlock();
  auto iffalse = newBasicBlock();

  place(entryBlock);
  auto target = newTemp();
  block->SAVE(target);
  condition(rule->def, iftrue, iffalse);
  place(iffalse);
  block->RET(false);
  place(iftrue);
  block->RET(true);

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
    out << "\t" << indent(std::string(rule->init, 1, rule->init.length() - 2))
        << std::endl;
  }

  for (auto& temp : function->temps) {
    if (!p.insert(temp.name).second) continue;
    fmt::print(out, "\t{0} {1};", temp.type, temp.name);
    out << std::endl;
  }

  out << "\tgoto L0;" << std::endl;

  print(out, grammar_, &f);

  out << "}" << std::endl;
}

void GenParser::visit(ast::CharLiteral* node) {
  auto iftrue = newBasicBlock();
  block->CJUMP(block->CHAR_LITERAL(node->value, node->line), iftrue,
               code.iffalse);
  place(iftrue);
  block->EXP("yyconsume()");
  block->JUMP(code.iftrue);
}

void GenParser::visit(ast::Symbol* sym) {
  if (grammar_->terminals.find(sym->name) != grammar_->terminals.end()) {
    auto iftrue = newBasicBlock();
    block->CJUMP(block->NAME(sym), iftrue, code.iffalse);
    place(iftrue);
    block->EXP("yyconsume()");
    block->JUMP(code.iftrue);
    return;
  }
  block->CJUMP(block->NAME(sym), code.iftrue, code.iffalse);
}

void GenParser::visit(ast::Code* c) {
  block->CJUMP(block->CODE(c->text, c->line), code.iftrue, code.iffalse);
}

void GenParser::visit(ast::Lookahead* p) {
  auto iftrue = newBasicBlock();
  auto cleanup_iftrue = newBasicBlock();
  auto cleanup_iffalse = newBasicBlock();

  condition(p->head, iftrue, code.iffalse);

  auto target = newTemp();
  place(iftrue);
  block->SAVE(target);
  condition(p->tail, cleanup_iftrue, cleanup_iffalse);

  place(cleanup_iftrue);
  block->RESTORE(target);
  block->JUMP(code.iftrue);

  place(cleanup_iffalse);
  block->RESTORE(target);
  block->JUMP(code.iffalse);
}

void GenParser::visit(ast::And* p) {
  auto iftrue = newBasicBlock();
  condition(p->head, iftrue, code.iffalse);
  place(iftrue);
  condition(p->tail, code.iftrue, code.iffalse);
}

void GenParser::visit(ast::Or* p) {
  auto target = newTemp();
  block->SAVE(target);
  auto iffalse = newBasicBlock();
  condition(p->head, code.iftrue, iffalse);
  place(iffalse);
  block->RESTORE(target);
  condition(p->tail, code.iftrue, code.iffalse);
}

void GenParser::visit(ast::Plus* p) {
  auto more = newBasicBlock();
  condition(p->item, more, code.iffalse);
  place(more);

  auto iftrue = newBasicBlock();
  auto iffalse = newBasicBlock();
  auto target = newTemp();

  block->JUMP(iftrue);
  place(iftrue);
  block->SAVE(target);
  condition(p->item, iftrue, iffalse);
  place(iffalse);
  block->RESTORE(target);
  block->JUMP(code.iftrue);
}

void GenParser::visit(ast::Star* p) {
  auto iftrue = newBasicBlock();
  auto iffalse = newBasicBlock();
  auto target = newTemp();

  block->JUMP(iftrue);
  place(iftrue);
  block->SAVE(target);
  condition(p->item, iftrue, iffalse);
  place(iffalse);
  block->RESTORE(target);
  block->JUMP(code.iftrue);
}

void GenParser::visit(ast::Question* p) {
  auto target = newTemp();
  auto iffalse = newBasicBlock();
  block->SAVE(target);
  condition(p->item, code.iftrue, iffalse);
  place(iffalse);
  block->RESTORE(target);
  block->JUMP(code.iftrue);
}

void GenParser::operator()(ast::Grammar* grammar,
                           const std::vector<Token>& verbatim) {
  grammar_ = grammar;
  verbatim_ = &verbatim;

  grammar->accept(this);
}
