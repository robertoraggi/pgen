struct Parser {

#line 48 "pgen/parser.pgen"
std::vector<Token> tokens;
ast::Grammar* grammar_ = nullptr;

Parser(std::vector<Token> tokens)
  : tokens(std::move(tokens))
{
}


#line 59 "pgen/parser.pgen"
  bool parse_grammar(ast::Grammar*& grammar);
#line 71 "pgen/parser.pgen"
  bool parse_directive();
#line 80 "pgen/parser.pgen"
  bool parse_class_symbol();
#line 85 "pgen/parser.pgen"
  bool parse_base_symbol();
#line 90 "pgen/parser.pgen"
  bool parse_token_symbol();
#line 95 "pgen/parser.pgen"
  bool parse_external_symbol();
#line 102 "pgen/parser.pgen"
  bool parse_rule(ast::Rule*& rule);
#line 113 "pgen/parser.pgen"
  bool parse_alternative(ast::Node*& node);
#line 122 "pgen/parser.pgen"
  bool parse_sequence(ast::Node*& node);
#line 131 "pgen/parser.pgen"
  bool parse_postfix(ast::Node*& node);
#line 142 "pgen/parser.pgen"
  bool parse_primary(ast::Node*& node);
#line 160 "pgen/parser.pgen"
  bool parse_lookat_postfix();

  bool yyinvalid = true;
  int yytok{};
  inline int yytoken() { if (yyinvalid) { yytok = yytoken(0); yyinvalid = false; } return yytok; }
  inline void yyconsume() { ++yycursor; yyinvalid = true; }
  inline void yyrewind(int i) { if (yycursor == i) return; yycursor = i; yyinvalid = true; }
  int yytoken(int index);

  unsigned yyparsed = 0;
  int yydepth = -1;
  unsigned yycursor = 0;
};


#line 59 "pgen/parser.pgen"
bool Parser::parse_grammar(ast::Grammar*& grammar) {
	
  ast::Rule* rule = nullptr;
  grammar_ = new ast::Grammar();
  grammar = grammar_;

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	unsigned yy3;
	goto L0;
L0:
L1:	yy0 = yycursor;
	yy1 = yycursor;

#line 65 "pgen/parser.pgen"
	if (parse_directive()) goto L1;
L2:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

L3:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);

L4:
L5:	yy2 = yycursor;
	yy3 = yycursor;

#line 66 "pgen/parser.pgen"
	if (parse_rule(rule)) goto L7;
L6:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);
	goto L8;
L7:
#line 67 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar->rules.push_back(rule); } return true; })()) goto L5;
L8:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

L9:
#line 69 "pgen/parser.pgen"
	if (yytoken() != T_EOF_SYMBOL) goto L11;
L10:	yyconsume();
	goto L12;
L11:	return false;
L12:	return true;
}


#line 71 "pgen/parser.pgen"
bool Parser::parse_directive() {
	unsigned yy0;
	unsigned yy1;
	unsigned yy10;
	unsigned yy11;
	unsigned yy12;
	unsigned yy2;
	unsigned yy3;
	unsigned yy4;
	unsigned yy5;
	unsigned yy6;
	unsigned yy7;
	unsigned yy8;
	unsigned yy9;
	goto L0;
L0:	yy0 = yycursor;
	yy1 = yycursor;
	yy2 = yycursor;
	yy3 = yycursor;
	yy4 = yycursor;

#line 72 "pgen/parser.pgen"
	if (yytoken() != T_TOKEN) goto L10;
L1:	yyconsume();

L2:	yy5 = yycursor;

#line 72 "pgen/parser.pgen"
	if (parse_token_symbol()) goto L4;
L3:	if (yy5 > yyparsed) yyparsed = yy5;
	yyrewind(yy5);
	goto L10;
L4:
L5:	yy6 = yycursor;

#line 72 "pgen/parser.pgen"
	if (yytoken() != T_COMMA) goto L9;
L6:	yyconsume();

L7:	yy7 = yycursor;

#line 72 "pgen/parser.pgen"
	if (parse_token_symbol()) goto L5;
L8:	if (yy7 > yyparsed) yyparsed = yy7;
	yyrewind(yy7);

L9:	if (yy6 > yyparsed) yyparsed = yy6;
	yyrewind(yy6);
	goto L36;
L10:	if (yy4 > yyparsed) yyparsed = yy4;
	yyrewind(yy4);

#line 73 "pgen/parser.pgen"
	if (yytoken() != T_EXTERN) goto L20;
L11:	yyconsume();

L12:	yy8 = yycursor;

#line 73 "pgen/parser.pgen"
	if (parse_external_symbol()) goto L14;
L13:	if (yy8 > yyparsed) yyparsed = yy8;
	yyrewind(yy8);
	goto L20;
L14:
L15:	yy9 = yycursor;

#line 73 "pgen/parser.pgen"
	if (yytoken() != T_COMMA) goto L19;
L16:	yyconsume();

L17:	yy10 = yycursor;

#line 73 "pgen/parser.pgen"
	if (parse_external_symbol()) goto L15;
L18:	if (yy10 > yyparsed) yyparsed = yy10;
	yyrewind(yy10);

L19:	if (yy9 > yyparsed) yyparsed = yy9;
	yyrewind(yy9);
	goto L36;
L20:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);

#line 74 "pgen/parser.pgen"
	if (yytoken() != T_CLASS) goto L24;
L21:	yyconsume();

L22:	yy11 = yycursor;

#line 74 "pgen/parser.pgen"
	if (parse_class_symbol()) goto L36;
L23:	if (yy11 > yyparsed) yyparsed = yy11;
	yyrewind(yy11);

L24:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

#line 75 "pgen/parser.pgen"
	if (yytoken() != T_EXTENDS) goto L28;
L25:	yyconsume();

L26:	yy12 = yycursor;

#line 75 "pgen/parser.pgen"
	if (parse_base_symbol()) goto L36;
L27:	if (yy12 > yyparsed) yyparsed = yy12;
	yyrewind(yy12);

L28:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

#line 76 "pgen/parser.pgen"
	if (yytoken() != T_TOKEN_TYPE) goto L33;
L29:	yyconsume();

L30:
#line 76 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L33;
L31:	yyconsume();

L32:
#line 76 "pgen/parser.pgen"
	if (([&]() -> bool {{ FLAGS_token_type = tokens[yycursor - 1].text; } return true; })()) goto L36;
L33:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);

#line 77 "pgen/parser.pgen"
	if (yytoken() != T_TEXT) goto L35;
L34:	yyconsume();
	goto L36;
L35:	return false;
L36:	return true;
}


#line 80 "pgen/parser.pgen"
bool Parser::parse_class_symbol() {
	goto L0;
L0:
#line 81 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L3;
L1:	yyconsume();

L2:
#line 82 "pgen/parser.pgen"
	if (([&]() -> bool {{ FLAGS_parser_name = tokens[yycursor - 1].text; } return true; })()) goto L4;
L3:	return false;
L4:	return true;
}


#line 85 "pgen/parser.pgen"
bool Parser::parse_base_symbol() {
	goto L0;
L0:
#line 86 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L3;
L1:	yyconsume();

L2:
#line 87 "pgen/parser.pgen"
	if (([&]() -> bool {{ FLAGS_extends = tokens[yycursor - 1].text; } return true; })()) goto L4;
L3:	return false;
L4:	return true;
}


#line 90 "pgen/parser.pgen"
bool Parser::parse_token_symbol() {
	goto L0;
L0:
#line 91 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L3;
L1:	yyconsume();

L2:
#line 92 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar_->terminals.emplace(tokens[yycursor - 1].text); } return true; })()) goto L4;
L3:	return false;
L4:	return true;
}


#line 95 "pgen/parser.pgen"
bool Parser::parse_external_symbol() {
	unsigned yy0;
	goto L0;
L0:	yy0 = yycursor;

#line 96 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L5;
L1:	yyconsume();

L2:
#line 96 "pgen/parser.pgen"
	if (yytoken() != T_EXTRA) goto L5;
L3:	yyconsume();

L4:
#line 97 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar_->externals.emplace(tokens[yycursor - 2].text, tokens[yycursor - 1].text); } return true; })()) goto L9;
L5:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);

#line 98 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L8;
L6:	yyconsume();

L7:
#line 99 "pgen/parser.pgen"
	if (([&]() -> bool {{ grammar_->externals.emplace(tokens[yycursor - 1].text, std::string()); } return true; })()) goto L9;
L8:	return false;
L9:	return true;
}


#line 102 "pgen/parser.pgen"
bool Parser::parse_rule(ast::Rule*& rule) {
	
  ast::Node* def = nullptr;
  rule = new ast::Rule();

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	goto L0;
L0:
#line 107 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L17;
L1:	yyconsume();

L2:
#line 107 "pgen/parser.pgen"
	if (!([&]() -> bool {{ rule->lhs = tokens[yycursor - 1].text; rule->line = tokens[yycursor - 1].line; } return true; })()) goto L17;
L3:	yy0 = yycursor;

#line 108 "pgen/parser.pgen"
	if (yytoken() != T_EXTRA) goto L6;
L4:	yyconsume();

L5:
#line 108 "pgen/parser.pgen"
	if (([&]() -> bool {{ rule->extra = tokens[yycursor - 1].text; } return true; })()) goto L7;
L6:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);

L7:	yy1 = yycursor;

#line 109 "pgen/parser.pgen"
	if (yytoken() != T_CODE) goto L10;
L8:	yyconsume();

L9:
#line 109 "pgen/parser.pgen"
	if (([&]() -> bool {{ rule->init = tokens[yycursor - 1].text; } return true; })()) goto L11;
L10:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

L11:
#line 110 "pgen/parser.pgen"
	if (yytoken() != T_COLON) goto L17;
L12:	yyconsume();

L13:	yy2 = yycursor;

#line 110 "pgen/parser.pgen"
	if (parse_alternative(rule->def)) goto L15;
L14:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);
	goto L17;
L15:
#line 110 "pgen/parser.pgen"
	if (yytoken() != T_SEMICOLON) goto L17;
L16:	yyconsume();
	goto L18;
L17:	return false;
L18:	return true;
}


#line 113 "pgen/parser.pgen"
bool Parser::parse_alternative(ast::Node*& node) {
	
    ast::Node* tail = nullptr;

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	goto L0;
L0:	yy0 = yycursor;

#line 117 "pgen/parser.pgen"
	if (parse_sequence(node)) goto L2;
L1:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);
	goto L9;
L2:
L3:	yy1 = yycursor;

#line 118 "pgen/parser.pgen"
	if (yytoken() != T_BAR) goto L8;
L4:	yyconsume();

L5:	yy2 = yycursor;

#line 118 "pgen/parser.pgen"
	if (parse_sequence(tail)) goto L7;
L6:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);
	goto L8;
L7:
#line 119 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Or(node, tail); } return true; })()) goto L3;
L8:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);
	goto L10;
L9:	return false;
L10:	return true;
}


#line 122 "pgen/parser.pgen"
bool Parser::parse_sequence(ast::Node*& node) {
	
  ast::Node* tail = nullptr;

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	unsigned yy3;
	goto L0;
L0:	yy0 = yycursor;

#line 126 "pgen/parser.pgen"
	if (parse_postfix(node)) goto L2;
L1:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);
	goto L9;
L2:
L3:	yy1 = yycursor;
	yy2 = yycursor;

#line 127 "pgen/parser.pgen"
	if (parse_lookat_postfix()) goto L5;
L4:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);
	goto L8;
L5:	yy3 = yycursor;

#line 127 "pgen/parser.pgen"
	if (parse_postfix(tail)) goto L7;
L6:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);
	goto L8;
L7:
#line 128 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::And(node, tail); } return true; })()) goto L3;
L8:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);
	goto L10;
L9:	return false;
L10:	return true;
}


#line 131 "pgen/parser.pgen"
bool Parser::parse_postfix(ast::Node*& node) {
	
    ast::Node* la = nullptr;

	unsigned yy0;
	unsigned yy1;
	unsigned yy2;
	unsigned yy3;
	unsigned yy4;
	unsigned yy5;
	goto L0;
L0:	yy0 = yycursor;

#line 135 "pgen/parser.pgen"
	if (parse_primary(node)) goto L2;
L1:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);
	goto L17;
L2:	yy1 = yycursor;
	yy2 = yycursor;
	yy3 = yycursor;
	yy4 = yycursor;

#line 136 "pgen/parser.pgen"
	if (yytoken() != T_QUESTION) goto L5;
L3:	yyconsume();

L4:
#line 136 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Question(node); } return true; })()) goto L18;
L5:	if (yy4 > yyparsed) yyparsed = yy4;
	yyrewind(yy4);

#line 137 "pgen/parser.pgen"
	if (yytoken() != T_STAR) goto L8;
L6:	yyconsume();

L7:
#line 137 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Star(node); } return true; })()) goto L18;
L8:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);

#line 138 "pgen/parser.pgen"
	if (yytoken() != T_PLUS) goto L11;
L9:	yyconsume();

L10:
#line 138 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Plus(node); } return true; })()) goto L18;
L11:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

#line 139 "pgen/parser.pgen"
	if (yytoken() != T_SLASH) goto L16;
L12:	yyconsume();

L13:	yy5 = yycursor;

#line 139 "pgen/parser.pgen"
	if (parse_primary(la)) goto L15;
L14:	if (yy5 > yyparsed) yyparsed = yy5;
	yyrewind(yy5);
	goto L16;
L15:
#line 139 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Lookahead(node, la); } return true; })()) goto L18;
L16:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);
	goto L18;
L17:	return false;
L18:	return true;
}


#line 142 "pgen/parser.pgen"
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

#line 143 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L5;
L1:	yyconsume();

L2:
#line 143 "pgen/parser.pgen"
	if (yytoken() != T_EXTRA) goto L5;
L3:	yyconsume();

L4:
#line 144 "pgen/parser.pgen"
	if (([&]() -> bool {{
      auto sym = new ast::Symbol(tokens[yycursor - 2].text, tokens[yycursor - 1].text, tokens[yycursor - 1].line);
      sym->isTerminal = grammar_->terminals.find(sym->name) != grammar_->terminals.end();
      node = sym;
    } return true; })()) goto L21;
L5:	if (yy3 > yyparsed) yyparsed = yy3;
	yyrewind(yy3);

#line 149 "pgen/parser.pgen"
	if (yytoken() != T_IDENTIFIER) goto L8;
L6:	yyconsume();

L7:
#line 150 "pgen/parser.pgen"
	if (([&]() -> bool {{
      auto sym = new ast::Symbol(tokens[yycursor - 1].text, std::string(), tokens[yycursor - 1].line);
      sym->isTerminal = grammar_->terminals.find(sym->name) != grammar_->terminals.end();
      node = sym;
    } return true; })()) goto L21;
L8:	if (yy2 > yyparsed) yyparsed = yy2;
	yyrewind(yy2);

#line 155 "pgen/parser.pgen"
	if (yytoken() != T_CHAR_LITERAL) goto L11;
L9:	yyconsume();

L10:
#line 155 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::CharLiteral(tokens[yycursor - 1].text, tokens[yycursor - 1].line); } return true; })()) goto L21;
L11:	if (yy1 > yyparsed) yyparsed = yy1;
	yyrewind(yy1);

#line 156 "pgen/parser.pgen"
	if (yytoken() != T_CODE) goto L14;
L12:	yyconsume();

L13:
#line 156 "pgen/parser.pgen"
	if (([&]() -> bool {{ node = new ast::Code(tokens[yycursor - 1].text, tokens[yycursor - 1].line); } return true; })()) goto L21;
L14:	if (yy0 > yyparsed) yyparsed = yy0;
	yyrewind(yy0);

#line 157 "pgen/parser.pgen"
	if (yytoken() != T_LPAREN) goto L20;
L15:	yyconsume();

L16:	yy4 = yycursor;

#line 157 "pgen/parser.pgen"
	if (parse_alternative(node)) goto L18;
L17:	if (yy4 > yyparsed) yyparsed = yy4;
	yyrewind(yy4);
	goto L20;
L18:
#line 157 "pgen/parser.pgen"
	if (yytoken() != T_RPAREN) goto L20;
L19:	yyconsume();
	goto L21;
L20:	return false;
L21:	return true;
}


#line 160 "pgen/parser.pgen"
bool Parser::parse_lookat_postfix() {
	goto L0;
L0:
#line 160 "pgen/parser.pgen"
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
L1:	return false;
L2:	return true;
}
