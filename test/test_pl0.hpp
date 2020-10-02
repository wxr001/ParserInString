// ParserInString -*- C++ -*-

// Copyright (C) 2020  Scott Brown

// This file is part of the UCL library.
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PARSER_IN_STRING_TEST_PL0
#define PARSER_IN_STRING_TEST_PL0

#include "Common/Tstring.hpp"
#include "Matcher/LL.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include "doctest.h"
#include <type_traits>
#include <iostream>
namespace Compiler
{
    DECL_RULE(program, "program = block, ';';");
    DECL_RULE(block, R"(
    block = [consts], [vars], {procedure}, statement;)");
    DECL_RULE(consts, R"(
    consts = 'const', ident, '=', number, {',', ident, '=', number}, ';';)");
    DECL_RULE(vars, R"(
    vars = 'var', ident, {',', ident}, ';';)");
    DECL_RULE(procedure, R"(
    procedure = 'procedure', ident, ';', block, ';';)");
    DECL_RULE(statement, R"(
    statement = [
        assignstmt |
        callstmt |
        writestmt |
        qstmt |
        bangstmt |
        beginstmt |
        ifstmt |
        whilestmt
    ];)");
    DECL_RULE(assignstmt, R"(
    assignstmt = ident, ':=', expression;)");
    DECL_RULE(callstmt, R"(
    callstmt = 'call', ident;)");
    DECL_RULE(writestmt, R"(
    writestmt = 'write', ident;)");
    DECL_RULE(qstmt, "qstmt = '?', ident;");
    DECL_RULE(bangstmt, "bangstmt = '!', expression;");
    DECL_RULE(beginstmt, R"(
    beginstmt = 'begin', statement, {';', statement}, 'end';)");
    DECL_RULE(ifstmt, R"(
    ifstmt = 'if', condition, 'then', statement;)");
    DECL_RULE(whilestmt, R"(
    whilestmt = 'while', condition, 'do', statement;)");
    DECL_RULE(condition, R"(
    condition = 'odd', expression |
        expression, ('=' | '#' | '<' | '<=' | '>' | '>='), expression;)");
    DECL_RULE(expression, R"(
    expression =
        ['+' | '-'], term, {['+' | '-'], term};)");
    DECL_RULE(term, "term = factor, {('*' | '/'), factor};");
    DECL_RULE(factor, "factor = ident | number | '(', expression, ')';");
    DECL_RULE(ident, R"(
    ident =
        ?lexer ident?,
        ?@[a-zA-Z][a-zA-Z]*?;)");
    DECL_RULE(number, R"(
    number =
        ?lexer number?,
        ?@[0-9]+?;)");
    using pl0_rules = code_to_rules_t<program_rule,
                                      block_rule,
                                      consts_rule,
                                      vars_rule,
                                      procedure_rule,
                                      statement_rule,
                                      assignstmt_rule,
                                      callstmt_rule,
                                      writestmt_rule,
                                      qstmt_rule,
                                      bangstmt_rule,
                                      beginstmt_rule,
                                      ifstmt_rule,
                                      whilestmt_rule,
                                      condition_rule,
                                      expression_rule,
                                      term_rule,
                                      factor_rule,
                                      ident_rule,
                                      number_rule>;
    using pl0_ll    = ll_parsing<pl0_rules, _TSTR("program"), 1>;
} // namespace Compiler
TEST_CASE("ll(1)")
{
    typename Compiler::pl0_ll::parsing_table::state_map          _s;
    typename Compiler::pl0_ll::parsing_table::parsing_table      _p;
    std::unordered_map<std::string, Compiler::parser_listener_t> _m;
    Compiler::pl0_ll::parsing_table table{_s, _p, _m};
    std::cerr<<table.dump()<<std::endl;
}

#endif
