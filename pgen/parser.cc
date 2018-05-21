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

#include "pgen/parser.h"

#include "pgen/classify-symbols.h"
#include "pgen/flags.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

Parser::Parser(std::string yyfilename, token_list tokens)
    : yyfilename(std::move(yyfilename)), yytokens(std::move(tokens)) {
  yytoken = yytokens.cbegin();
}

bool Parser::yymatch(TOK tk) {
  if (yytoken->isNot(tk)) return false;
  ++yytoken;
  return true;
}

Parser::iterator Parser::yyexpect(TOK tk) {
  if (yytoken->isNot(tk)) {
    fmt::print(std::cerr, "{0}:{1}: error: unexpected token '{2}'", yyfilename,
               yytoken->line, *yytoken);
    std::cerr << std::endl;
    exit(EXIT_FAILURE);
    return yytokens.end();
  }
  return yytoken++;
}

Parser::iterator Parser::yyconsume() { return yytoken++; }

ast::Grammar* Parser::operator()() {
  grammar_ = new ast::Grammar();

  parseDirectives();
  parseRules();

  ClassifySymbols classifySymbols;

  classifySymbols(grammar_);

  return grammar_;
}

void Parser::parseDirectives() {
  while (true) {
    if (yymatch(TOK::EXTERN))
      parseExternSymbols();
    else if (yymatch(TOK::TOKEN))
      parseTokens();
    else if (yymatch(TOK::CLASS))
      parseClass();
    else
      break;
  }
}

void Parser::parseTokens() {
  do {
    auto sym = yyexpect(TOK::IDENTIFIER);
    grammar_->terminals.emplace(sym->text);
  } while (yymatch(TOK::COMMA));
}

void Parser::parseClass() {
  auto name = yyexpect(TOK::IDENTIFIER);
  FLAGS_parser_name = name->text;
}

void Parser::parseExternSymbols() {
  do {
    auto sym = yyexpect(TOK::IDENTIFIER);
    std::string extra;
    if (yytoken->is(TOK::EXTRA)) {
      extra = yytoken->text;
      ++yytoken;
    }
    grammar_->externals.emplace(sym->text, std::move(extra));
  } while (yymatch(TOK::COMMA));
}

void Parser::parseRules() {
  while (yytoken->is(TOK::IDENTIFIER)) grammar_->rules.push_back(parseRule());
}

ast::Rule* Parser::parseRule() {
  auto rule = new ast::Rule();
  rule->line = yytoken->line;
  rule->lhs = yyexpect(TOK::IDENTIFIER)->text;
  if (yytoken->is(TOK::EXTRA)) {
    rule->line = yytoken->line;
    rule->extra = yytoken->text;
    ++yytoken;
  }
  if (yytoken->is(TOK::CODE)) {
    rule->init = yytoken->text;
    ++yytoken;
  }
  yyexpect(TOK::COLON);
  if (yytoken->isNot(TOK::SEMICOLON)) rule->def = parseDefinition();
  yyexpect(TOK::SEMICOLON);
  return rule;
}

ast::Node* Parser::parseDefinition() { return parseOr(); }

ast::Node* Parser::parseOr() {
  auto item = parseAnd();
  if (yymatch(TOK::BAR)) item = new ast::Or(item, parseDefinition());
  return item;
}

ast::Node* Parser::parseAnd() {
  auto item = parsePostfix();
  if (lookAtNode()) item = new ast::And(item, parseAnd());
  return item;
}

ast::Node* Parser::parsePostfix() {
  auto item = parsePrimary();
  if (!item) {
    return 0;
  } else if (yymatch(TOK::QUESTION)) {
    return new ast::Question(item);
  } else if (yymatch(TOK::PLUS)) {
    return new ast::Plus(item);
  } else if (yymatch(TOK::STAR)) {
    return new ast::Star(item);
  } else if (yymatch(TOK::SLASH)) {
    auto la = parsePrimary();
    item = new ast::Lookahead(item, la);
  }
  return item;
}

ast::Node* Parser::parseId() {
  if (yytoken->is(TOK::CODE)) {
    auto code = new ast::Code(yytoken->text, yytoken->line);
    ++yytoken;
    return code;
  }
  auto id = yyexpect(TOK::IDENTIFIER);
  std::string extra;
  if (yytoken->is(TOK::EXTRA)) {
    extra = yytoken->text;
    ++yytoken;
  }
  return new ast::Symbol(id->text, extra, id->line);
}

ast::Node* Parser::parsePrimary() {
  if (yymatch(TOK::LPAREN)) {
    auto item = parseOr();
    yyexpect(TOK::RPAREN);
    return item;
  }
  if (yytoken->is(TOK::CHAR_LITERAL)) {
    auto item = new ast::CharLiteral(yytoken->text, yytoken->line);
    ++yytoken;
    return item;
  }
  return parseId();
}

bool Parser::lookAtNode() {
  if (yytoken->is(TOK::EOF_SYMBOL)) return false;
  if (yytoken->is(TOK::SEMICOLON)) return false;
  if (yytoken->is(TOK::BAR)) return false;
  if (yytoken->is(TOK::RPAREN)) return false;
  return true;
}
