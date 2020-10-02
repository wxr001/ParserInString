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

#ifndef PARSER_IN_STRING_TEST_TESTLUA
#define PARSER_IN_STRING_TEST_TESTLUA

#include "Common/Tstring.hpp"
#include "Lexer/Lexer.hpp"
#include "Matcher/LL.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <doctest.h>
#include <type_traits>
namespace Compiler
{
    DECL_RULE(chunk, "chunk = block;");
    DECL_RULE(block, "block = {stat}, [retstat];");
    DECL_RULE(stat12, "stat = ; | varlist, '=', explist;");
    DECL_RULE(stat3_6, "stat = functioncall | label | 'break'| 'goto', Name;");
    DECL_RULE(
        stat7_9,
        "stat = 'do', block, 'end' | 'while', exp, 'do', block, 'end' | 'repeat', block, 'until', exp;");
    DECL_RULE(
        stat_10,
        "stat = 'if', exp, 'then', block, {'elseif', exp, 'then', block}, ['else', block], 'end';");
    DECL_RULE(stat11_12, R"(
    stat =   'for', Name, '=', exp, ',', exp, [',', exp], 'do', block, 'end' | 
    		 'for', namelist, 'in', explist, 'do', block, 'end';)");
    DECL_RULE(stat_last, R"(
    stat =   'function', funcname, funcbody | 
    		 'local', 'function', Name, funcbody | 
    		 'local', namelist, ['=', explist] ;)");
    DECL_RULE(retstat, "retstat = 'return', [explist], [';'];");
    DECL_RULE(label, "label = '::', Name, '::';");
    DECL_RULE(funcname, "funcname = Name, {'.', Name}, [':', Name];");
    DECL_RULE(varlist, "varlist = var, {',', var};");
    DECL_RULE(
        var,
        "var =  Name | prefixexp, '[', exp, ']' | prefixexp, '.', Name; ");
    DECL_RULE(namelist, "namelist = Name, {',', Name};");
    DECL_RULE(explist, "explist = exp, {',', exp};");
    DECL_RULE(exp, R"(
    exp =  'nil' | 'false' | 'true' | Numeral | LiteralString | '...' | functiondef | 
    		 prefixexp | tableconstructor | exp, binop, exp | unop, exp;)");
    DECL_RULE(prefixexp, "prefixexp = var | functioncall | '(', exp, ')';");
    DECL_RULE(functioncall,
              "functioncall =  prefixexp, args | prefixexp, ':', Name, args; ");
    DECL_RULE(
        args,
        "args =  '(', [explist], ')' | tableconstructor | LiteralString; ");
    DECL_RULE(functiondef, "functiondef = 'function', funcbody;");
    DECL_RULE(funcbody, "funcbody = '(', [parlist], ')', block, 'end';");
    DECL_RULE(parlist, "parlist = namelist, [',', '...'] | '...';");
    DECL_RULE(tableconstructor, "tableconstructor = '{', [fieldlist], '}';");
    DECL_RULE(fieldlist, "fieldlist = field, {fieldsep, field}, [fieldsep];");
    DECL_RULE(field, "field = '[', exp, ']', '=', exp | Name, '=', exp | exp;");
    DECL_RULE(fieldsep, "fieldsep = ',' | ';';");
    DECL_RULE(binop, R"(
    binop =  '+' | '-' | '*' | '/' | '//' | '^' | '%' | 
    		 '&' | '~' | '|' | '>>' | '<<' | '..' | 
    		 '<' | '<=' | '>' | '>=' | '==' | '~=' | 
    		 'and' | 'or';)");
    DECL_RULE(unop, "unop = '-' | 'not' | '#' | '~';");
    DECL_RULE(literal_string, "LiteralString = ?literal string?;");
    DECL_RULE(numeral, R"(
    Numeral = ?lexer number?, ['+' | '-'], {?number?}, ['.', {?number?}], ['E', ['+' | '-'], {?number?}];)");
    DECL_RULE(identifier,
              "Name = ?lexer identifier?, (?alpha? | '_'), {?alnum? | '_'};");
    using lua = code_to_rules_t<chunk_rule,
                                block_rule,
                                stat12_rule,
                                stat3_6_rule,
                                stat7_9_rule,
                                stat_10_rule,
                                stat11_12_rule,
                                stat_last_rule,
                                retstat_rule,
                                label_rule,
                                funcname_rule,
                                varlist_rule,
                                var_rule,
                                namelist_rule,
                                explist_rule,
                                exp_rule,
                                prefixexp_rule,
                                functioncall_rule,
                                args_rule,
                                functiondef_rule,
                                funcbody_rule,
                                parlist_rule,
                                tableconstructor_rule,
                                fieldlist_rule,
                                field_rule,
                                fieldsep_rule,
                                binop_rule,
                                unop_rule,
                                literal_string_rule,
                                numeral_rule,
                                identifier_rule>;
    // not LL(k)
    // using lua_ll = ll_parsing<lua, _TSTR("chunk"), 2>;
} // namespace Compiler
TEST_SUITE("lua") {}
#endif
