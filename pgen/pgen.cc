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

#include <gflags/gflags.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "pgen/flags.h"
#include "pgen/gen-parser.h"
#include "pgen/lexer.h"
#include "pgen/parser.h"

namespace {

class DisposeAST final : ast::Visitor {
 public:
  void operator()(ast::Node* ast) {
    if (ast) ast->accept(this);
  }

 private:
  void visit(ast::Grammar* ast) override {
    for (auto rule : ast->rules) {
      rule->accept(this);
    }
    delete ast;
  }
  void visit(ast::Rule* ast) override {
    ast->def->accept(this);
    delete ast;
  }
  void visit(ast::CharLiteral* ast) override { delete ast; }
  void visit(ast::Symbol* ast) override { delete ast; }
  void visit(ast::Code* ast) override { delete ast; }
  void visit(ast::And* ast) override {
    ast->head->accept(this);
    ast->tail->accept(this);
    delete ast;
  }
  void visit(ast::Or* ast) override {
    ast->head->accept(this);
    ast->tail->accept(this);
    delete ast;
  }
  void visit(ast::Lookahead* ast) override {
    ast->head->accept(this);
    ast->tail->accept(this);
    delete ast;
  }
  void visit(ast::Plus* ast) override {
    ast->item->accept(this);
    delete ast;
  }
  void visit(ast::Star* ast) override {
    ast->item->accept(this);
    delete ast;
  }
  void visit(ast::Question* ast) override {
    ast->item->accept(this);
    delete ast;
  }
};

std::string ReadFile(const std::string& filename) {
  std::ifstream in(filename);
  std::ostringstream out;
  out << in.rdbuf();
  in.close();
  return out.str();
}
}  // namespace

struct FlagsScope {
  ~FlagsScope() { gflags::ShutDownCommandLineFlags(); }
};

int main(int argc, char* argv[]) {
  FlagsScope flagsScope;

  gflags::SetVersionString("1.0");
  gflags::SetUsageMessage("pgen [options] file");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_input.empty() && argc > 1) FLAGS_input = argv[1];

  if (FLAGS_input.empty()) {
    std::cerr << "pgen: no input file" << std::endl;
    return EXIT_FAILURE;
  }

  if (FLAGS_o.empty()) {
    std::cerr << "pgen: no output file" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<Token> verbatim;

  std::vector<Token> tokens = Lexer::tokenize(ReadFile(FLAGS_input), &verbatim);

  Parser parse(FLAGS_input, std::move(tokens));

  auto grammar = parse();

  if (!grammar->undef.empty()) {
    exit(EXIT_FAILURE);
  }

  std::ofstream out(FLAGS_o);

  GenParser gen(out);
  gen(grammar, verbatim);

  DisposeAST disposeAST;
  disposeAST(grammar);

  return EXIT_SUCCESS;
}
