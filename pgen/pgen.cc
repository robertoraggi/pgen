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

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <gflags/gflags.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "pgen/dispose-ast.h"
#include "pgen/flags.h"
#include "pgen/gen-parser.h"
#include "pgen/lexer.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4102)
#endif

#include "pgen/parser-pgen.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

int Parser::yytoken(int n) { return tokens[yycursor + n].kind; }

namespace {

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

  Parser parser(tokens);

  ast::Grammar* grammar = nullptr;
  if (!parser.parse_grammar(grammar)) {
    fmt::print(std::cerr, "{0}:{1}: error: unexpected token", FLAGS_input,
               tokens[parser.yyparsed].line);
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }

  if (!grammar->undef.empty()) {
    return EXIT_FAILURE;
  }

  std::ofstream out(FLAGS_o);

  GenParser gen(out);
  gen(grammar, verbatim);

  ast::DisposeAST disposeAST;
  disposeAST(grammar);

  return EXIT_SUCCESS;
}
