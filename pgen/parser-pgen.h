struct Parser {

#line 46 "pgen/parser.pgen"
std::vector<Token> tokens;
ast::Grammar* grammar_ = nullptr;

Parser(std::vector<Token> tokens)
  : tokens(std::move(tokens))
{
}


#line 57 "pgen/parser.pgen"
  bool parse_grammar(ast::Grammar*& grammar);
#line 69 "pgen/parser.pgen"
  bool parse_directive();
#line 76 "pgen/parser.pgen"
  bool parse_class_symbol();
#line 81 "pgen/parser.pgen"
  bool parse_token_symbol();
#line 86 "pgen/parser.pgen"
  bool parse_external_symbol();
#line 93 "pgen/parser.pgen"
  bool parse_rule(ast::Rule*& rule);
#line 104 "pgen/parser.pgen"
  bool parse_alternative(ast::Node*& node);
#line 113 "pgen/parser.pgen"
  bool parse_sequence(ast::Node*& node);
#line 122 "pgen/parser.pgen"
  bool parse_postfix(ast::Node*& node);
#line 133 "pgen/parser.pgen"
  bool parse_primary(ast::Node*& node);
#line 151 "pgen/parser.pgen"
  bool parse_lookat_postfix();

  bool yyinvalid = true;
  int yytok = 0;
  inline int yytoken() { if (yyinvalid) { yytok = yytoken(0); yyinvalid = false; } return yytok; }
  inline void yyconsume() { ++yycursor; yyinvalid = true; }
  inline void yyrewind(int i) { if (yycursor == i) return; yycursor = i; yyinvalid = true; }
  int yytoken(int index);

  unsigned yyparsed = 0;
  int yydepth = -1;
  unsigned yycursor = 0;
};


#line 57 "pgen/parser.pgen"
bool Parser::parse_grammar(ast::Grammar*& grammar) {
	
  ast::Rule* rule = nullptr;
  grammar_ = new ast::Grammar();
  grammar = grammar_;

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	goto L0;
L0:	yy0 = yycursor;

L1:	yy1 = yycursor;

#line 63 "pgen/parser.pgen"
	if (parse_directive()) goto L1;
L2:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

L3:
L4:	yy2 = yycursor;

#line 64 "pgen/parser.pgen"
	if (!parse_rule(rule)) goto L6;
L5:
#line 65 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar->rules.push_back(rule); } return true; })()) goto L4;
L6:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

L7:
#line 67 "pgen/parser.pgen"
	if (yytoken() != T_EOF_SYMBOL) goto L9;
L8:	yyconsume();
	goto L10;
L9:	yyrewind(yyparsed);
	return false;
L10:	return true;
}


#line 69 "pgen/parser.pgen"
bool Parser::parse_directive() {
	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	unsigned yy3;
	unsigned yy4;
	unsigned yy5;
	goto L0;
L0:	yy0 = yycursor;
	yy1 = yycursor;
	yy2 = yycursor;
	yy3 = yycursor;

#line 70 "pgen/parser.pgen"
	if (yytoken() != T_TOKEN) goto L8;
L1:	yyconsume();

L2:
#line 70 "pgen/parser.pgen"
	if (!parse_token_symbol()) goto L8;
L3:
L4:	yy4 = yycursor;

#line 70 "pgen/parser.pgen"
	if (yytoken() != T_COMMA) goto L7;
L5:	yyconsume();

L6:
#line 70 "pgen/parser.pgen"
	if (parse_token_symbol()) goto L4;
L7:	if (yy4 > yyparsed) yyparsed = yy4;
	yyrewind(yy4);
	goto L22;
L8:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);

#line 71 "pgen/parser.pgen"
	if (yytoken() != T_EXTERN) goto L16;
L9:	yyconsume();

L10:
#line 71 "pgen/parser.pgen"
	if (!parse_external_symbol()) goto L16;
L11:
L12:	yy5 = yycursor;

#line 71 "pgen/parser.pgen"
	if (yytoken() != T_COMMA) goto L15;
L13:	yyconsume();

L14:
#line 71 "pgen/parser.pgen"
	if (parse_external_symbol()) goto L12;
L15:	if (yy5 > yyparsed) yyparsed = yy5;
	yyrewind(yy5);
	goto L22;
L16:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

#line 72 "pgen/parser.pgen"
	if (yytoken() != T_CLASS) goto L19;
L17:	yyconsume();

L18:
#line 72 "pgen/parser.pgen"
	if (parse_class_symbol()) goto L22;
L19:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

#line 73 "pgen/parser.pgen"
	if (yytoken() != T_TEXT) goto L21;
L20:	yyconsume();
	goto L22;
L21:	yyrewind(yyparsed);
	return false;
L22:	return true;
}


#line 76 "pgen/parser.pgen"
bool Parser::parse_class_symbol() {
	unsigned yy0;
	goto L0;
L0:	yy0 = yycursor;

#line 77 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L3;
L1:	yyconsume();

L2:
#line 78 "pgen/parser.pgen"
	if (([&]() -> bool {{ FLAGS_parser_name = tokens[yycursor - 1].text; } return true; })()) goto L4;
L3:	yyrewind(yyparsed);
	return false;
L4:	return true;
}


#line 81 "pgen/parser.pgen"
bool Parser::parse_token_symbol() {
	unsigned yy0;
	goto L0;
L0:	yy0 = yycursor;

#line 82 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L3;
L1:	yyconsume();

L2:
#line 83 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar_->terminals.emplace(tokens[yycursor - 1].text); } return true; })()) goto L4;
L3:	yyrewind(yyparsed);
	return false;
L4:	return true;
}


#line 86 "pgen/parser.pgen"
bool Parser::parse_external_symbol() {
	unsigned yy0;
	unsigned yy1;
	goto L0;
L0:	yy0 = yycursor;
	yy1 = yycursor;

#line 87 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L5;
L1:	yyconsume();

L2:
#line 87 "pgen/parser.pgen"
	if (yytoken() != T_EXTRA) goto L5;
L3:	yyconsume();

L4:
#line 88 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar_->externals.emplace(tokens[yycursor - 2].text, tokens[yycursor - 1].text); } return true; })()) goto L9;
L5:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

#line 89 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L8;
L6:	yyconsume();

L7:
#line 90 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar_->externals.emplace(tokens[yycursor - 1].text, std::string()); } return true; })()) goto L9;
L8:	yyrewind(yyparsed);
	return false;
L9:	return true;
}


#line 93 "pgen/parser.pgen"
bool Parser::parse_rule(ast::Rule*& rule) {
	
  ast::Node* def = nullptr;
  rule = new ast::Rule();

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	goto L0;
L0:	yy0 = yycursor;

#line 98 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L16;
L1:	yyconsume();

L2:
#line 98 "pgen/parser.pgen"
	if (!([&]() -> bool {{ rule->lhs = tokens[yycursor - 1].text; rule->line = tokens[yycursor - 1].line; } return true; })()) goto L16;
L3:	yy1 = yycursor;

#line 99 "pgen/parser.pgen"
	if (yytoken() != T_EXTRA) goto L6;
L4:	yyconsume();

L5:
#line 99 "pgen/parser.pgen"
	if (([&]() -> bool {{ rule->extra = tokens[yycursor - 1].text; } return true; })()) goto L7;
L6:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

L7:	yy2 = yycursor;

#line 100 "pgen/parser.pgen"
	if (yytoken() != T_CODE) goto L10;
L8:	yyconsume();

L9:
#line 100 "pgen/parser.pgen"
	if (([&]() -> bool {{ rule->init = tokens[yycursor - 1].text; } return true; })()) goto L11;
L10:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

L11:
#line 101 "pgen/parser.pgen"
	if (yytoken() != T_COLON) goto L16;
L12:	yyconsume();

L13:
#line 101 "pgen/parser.pgen"
	if (!parse_alternative(rule->def)) goto L16;
L14:
#line 101 "pgen/parser.pgen"
	if (yytoken() != T_SEMICOLON) goto L16;
L15:	yyconsume();
	goto L17;
L16:	yyrewind(yyparsed);
	return false;
L17:	return true;
}


#line 104 "pgen/parser.pgen"
bool Parser::parse_alternative(ast::Node*& node) {
	
    ast::Node* tail = nullptr;

	unsigned yy0;
	unsigned yy1;
	goto L0;
L0:	yy0 = yycursor;

#line 108 "pgen/parser.pgen"
	if (!parse_sequence(node)) goto L7;
L1:
L2:	yy1 = yycursor;

#line 109 "pgen/parser.pgen"
	if (yytoken() != T_BAR) goto L6;
L3:	yyconsume();

L4:
#line 109 "pgen/parser.pgen"
	if (!parse_sequence(tail)) goto L6;
L5:
#line 110 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Or(node, tail); } return true; })()) goto L2;
L6:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);
	goto L8;
L7:	yyrewind(yyparsed);
	return false;
L8:	return true;
}


#line 113 "pgen/parser.pgen"
bool Parser::parse_sequence(ast::Node*& node) {
	
  ast::Node* tail = nullptr;

	unsigned yy0;
	unsigned yy1;
	goto L0;
L0:	yy0 = yycursor;

#line 117 "pgen/parser.pgen"
	if (!parse_postfix(node)) goto L6;
L1:
L2:	yy1 = yycursor;

#line 118 "pgen/parser.pgen"
	if (!parse_lookat_postfix()) goto L5;
L3:
#line 118 "pgen/parser.pgen"
	if (!parse_postfix(tail)) goto L5;
L4:
#line 119 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::And(node, tail); } return true; })()) goto L2;
L5:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);
	goto L7;
L6:	yyrewind(yyparsed);
	return false;
L7:	return true;
}


#line 122 "pgen/parser.pgen"
bool Parser::parse_postfix(ast::Node*& node) {
	
    ast::Node* la = nullptr;

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	unsigned yy3;
	unsigned yy4;
	goto L0;
L0:	yy0 = yycursor;

#line 126 "pgen/parser.pgen"
	if (!parse_primary(node)) goto L15;
L1:	yy1 = yycursor;
	yy2 = yycursor;
	yy3 = yycursor;
	yy4 = yycursor;

#line 127 "pgen/parser.pgen"
	if (yytoken() != T_QUESTION) goto L4;
L2:	yyconsume();

L3:
#line 127 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Question(node); } return true; })()) goto L16;
L4:	if (yy4 > yyparsed) yyparsed = yy4;
	yyrewind(yy4);

#line 128 "pgen/parser.pgen"
	if (yytoken() != T_STAR) goto L7;
L5:	yyconsume();

L6:
#line 128 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Star(node); } return true; })()) goto L16;
L7:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);

#line 129 "pgen/parser.pgen"
	if (yytoken() != T_PLUS) goto L10;
L8:	yyconsume();

L9:
#line 129 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Plus(node); } return true; })()) goto L16;
L10:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

#line 130 "pgen/parser.pgen"
	if (yytoken() != T_SLASH) goto L14;
L11:	yyconsume();

L12:
#line 130 "pgen/parser.pgen"
	if (!parse_primary(la)) goto L14;
L13:
#line 130 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Lookahead(node, la); } return true; })()) goto L16;
L14:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);
	goto L16;
L15:	yyrewind(yyparsed);
	return false;
L16:	return true;
}


#line 133 "pgen/parser.pgen"
bool Parser::parse_primary(ast::Node*& node) {
	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	unsigned yy3;
	unsigned yy4;
	goto L0;
L0:	yy0 = yycursor;
	yy1 = yycursor;
	yy2 = yycursor;
	yy3 = yycursor;
	yy4 = yycursor;

#line 134 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L5;
L1:	yyconsume();

L2:
#line 134 "pgen/parser.pgen"
	if (yytoken() != T_EXTRA) goto L5;
L3:	yyconsume();

L4:
#line 135 "pgen/parser.pgen"
	if (([&]() -> bool {{
      auto sym = new ast::Symbol(tokens[yycursor - 2].text, tokens[yycursor - 1].text, tokens[yycursor - 1].line);
      sym->isTerminal = grammar_->terminals.find(sym->name) != grammar_->terminals.end();
      node = sym;
    } return true; })()) goto L20;
L5:	if (yy4 > yyparsed) yyparsed = yy4;
	yyrewind(yy4);

#line 140 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L8;
L6:	yyconsume();

L7:
#line 141 "pgen/parser.pgen"
	if (([&]() -> bool {{
      auto sym = new ast::Symbol(tokens[yycursor - 1].text, std::string(), tokens[yycursor - 1].line);
      sym->isTerminal = grammar_->terminals.find(sym->name) != grammar_->terminals.end();
      node = sym;
    } return true; })()) goto L20;
L8:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);

#line 146 "pgen/parser.pgen"
	if (yytoken() != T_CHAR_LITERAL) goto L11;
L9:	yyconsume();

L10:
#line 146 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::CharLiteral(tokens[yycursor - 1].text, tokens[yycursor - 1].line); } return true; })()) goto L20;
L11:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

#line 147 "pgen/parser.pgen"
	if (yytoken() != T_CODE) goto L14;
L12:	yyconsume();

L13:
#line 147 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Code(tokens[yycursor - 1].text, tokens[yycursor - 1].line); } return true; })()) goto L20;
L14:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

#line 148 "pgen/parser.pgen"
	if (yytoken() != T_LPAREN) goto L19;
L15:	yyconsume();

L16:
#line 148 "pgen/parser.pgen"
	if (!parse_alternative(node)) goto L19;
L17:
#line 148 "pgen/parser.pgen"
	if (yytoken() != T_RPAREN) goto L19;
L18:	yyconsume();
	goto L20;
L19:	yyrewind(yyparsed);
	return false;
L20:	return true;
}


#line 151 "pgen/parser.pgen"
bool Parser::parse_lookat_postfix() {
	unsigned yy0;
	goto L0;
L0:	yy0 = yycursor;

#line 151 "pgen/parser.pgen"
	if (([&]() -> bool {{
  switch (yytoken()) {
  case T_IDENTIFIER:
  case T_CHAR_LITERAL:
  case T_CODE:
  case T_LPAREN:
    return true;
  default:
    return false;
  } // switch
} return true; })()) goto L2;
L1:	yyrewind(yyparsed);
	return false;
L2:	return true;
}
