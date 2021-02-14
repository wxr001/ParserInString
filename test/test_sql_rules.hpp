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

// Grammar rules originally from https://github.com/cviebig/lib-sql-text
#ifndef PARSER_IN_STRING_TESTSQLRULES
#define PARSER_IN_STRING_TESTSQLRULES

#include "TAVL.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Lexer/Lexer.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include "test_common.hpp"

namespace pis
{
    // common.hpp
    DECL_RULE(direction_type, R"(
    direction type =
        ?lexer asc?, ?@[Aa][Ss][Cc]? |
        ?lexer desc?, ?@[Dd][Ee][Ss][Cc]? |
        ;)");
    // create_table_stmt.hpp
    DECL_RULE(type, R"(
    type = ?lexer type?, ?alpha?, {?alpha?};)");
    DECL_RULE(column_def, R"(
    column def = identifier, type, ["(",
        [numeric literal, ","], numeric literal, ')'];)");
    DECL_RULE(column_def_list, R"(
    column def list = column def, {',', column def};)");
    DECL_RULE(indexed_column, R"(
    indexed column = identifier, direction type;)");
    DECL_RULE(indexed_column_list, R"(
    indexed column list = indexed column, {',', indexed column};)");
    DECL_RULE(table_constraint_type, R"(
    table constraint type =
        "PRIMARY", "KEY", "(", indexed column list, ')' |
        "UNIQUE", '(', indexed column list, ')' |
        "FOREIGN", "KEY", '(', identifier list, ')',
            "REFERENCES", identifier,
            '(', identifier list, ')';)");
    DECL_RULE(table_constraint, R"(
    table constraint =
        ["CONSTRAINT", identifier], table constraint type;)");
    DECL_RULE(table_constraint_list, R"(
    table constraint list =
        table constraint, [',', table constraint];)");
    DECL_RULE(create_table_stmt_definition, R"(
    create table stmt definition =
        '(', column def list,
            [',', table constraint list],
        ')',
        ["WITHOUT", "ROWID"] |
        'AS', select stmt;)");
    DECL_RULE(create_table_stmt, R"(
    create table stmt =
        "CREATE", ["TEMPORARY" | "TEMP"],
        "TABLE",
        ["IF", "NOT", "EXISTS"],
        table identifier,
        create table stmt definition;)");
    // expr.hpp
    DECL_RULE(function_params, R"(
    function params =
        ["DISTINCT"], expr list |
        '*' |
        ;)");
    DECL_RULE(function_call, R"(
    function call =
        identifier, '(', function params, ')';)");
    DECL_RULE(p1, "p1 = '||';");
    DECL_RULE(p2, "p2 = '*' | '/' | '%';");
    DECL_RULE(p3, "p3 = '+' | '-';");
    DECL_RULE(p4, "p4 = '>>' | '<<' | '&' | '|';");
    DECL_RULE(p5, R"(
    p5 = '<=' | '<' | '>=' | '>' |
        '=' | '==' | '!=' | '<>' |
        'IS NOT';)");
    DECL_RULE(
        p6,
        "p6 = 'IS ' | 'IN ' | 'LIKE ' | 'GLOB ' | 'MATCH ' | 'REGEXPR ';");
    DECL_RULE(p7, "p7 = 'AND ';");
    DECL_RULE(p8, "p8 = 'OR ';");
    DECL_RULE(expr_left, R"(
    expr left =
        '(', expr, ')' |
        literal value |
        '?' |
        identifier, ['.', identifier, ['.', identifier]] |
        function call;)");
    DECL_RULE(bo_lhs_p1, "bo lhs p1 = expr left;");
    DECL_RULE(bo_lhs_p2, "bo lhs p2 = bo p1 | expr left;");
    DECL_RULE(bo_lhs_p3, "bo lhs p3 = bo p2 | expr left;");
    DECL_RULE(bo_lhs_p4, "bo lhs p4 = bo p3 | expr left;");
    DECL_RULE(bo_lhs_p5, "bo lhs p5 = bo p4 | expr left;");
    DECL_RULE(bo_lhs_p6, "bo lhs p6 = bo p5 | expr left;");
    DECL_RULE(bo_lhs_p7, "bo lhs p7 = bo p6 | expr left;");
    DECL_RULE(bo_lhs_p8, "bo lhs p8 = bo p7 | expr left;");
    DECL_RULE(bo_rhs_p1, "bo rhs p1 = bo lhs p1;");
    DECL_RULE(bo_rhs_p2, "bo rhs p2 = bo p2 | bo lhs p2;");
    DECL_RULE(bo_rhs_p3, "bo rhs p3 = bo p3 | bo lhs p3;");
    DECL_RULE(bo_rhs_p4, "bo rhs p4 = bo p4 | bo lhs p4;");
    DECL_RULE(bo_rhs_p5, "bo rhs p5 = bo p5 | bo lhs p5;");
    DECL_RULE(bo_rhs_p6, "bo rhs p6 = bo p6 | bo lhs p6;");
    DECL_RULE(bo_rhs_p7, "bo rhs p7 = bo p7 | bo lhs p7;");
    DECL_RULE(bo_rhs_p8, "bo rhs p8 = bo p8 | bo lhs p8;");
    DECL_RULE(bo_p1, "bo p1 = bo lhs p1, p1, bo rhs p1;");
    DECL_RULE(bo_p2, "bo p2 = bo lhs p2, p2, bo rhs p2 | bo p1;");
    DECL_RULE(bo_p3, "bo p3 = bo lhs p3, p3, bo rhs p3 | bo p2;");
    DECL_RULE(bo_p4, "bo p4 = bo lhs p4, p4, bo rhs p4 | bo p3;");
    DECL_RULE(bo_p5, "bo p5 = bo lhs p5, p5, bo rhs p5 | bo p4;");
    DECL_RULE(bo_p6, "bo p6 = bo lhs p6, p6, bo rhs p6 | bo p5;");
    DECL_RULE(bo_p7, "bo p7 = bo lhs p7, p7, bo rhs p7 | bo p6;");
    DECL_RULE(bo_p8, "bo p8 = bo lhs p8, p8, bo rhs p8 | bo p7;");
    DECL_RULE(binary_operation, "binary operation = bo p8;");
    DECL_RULE(expr, R"(
    expr = binary operation | expr left;)");
    DECL_RULE(expr_list, R"(
    expr list = expr, {',', expr};)");
    // identifier.hpp
    DECL_RULE(identifier, R"(
    identifier = ?lexer identifier?, ?@[a-zA-Z][a-zA-Z0-9_]*? |
                ?lexer double quote?, ?@"[a-zA-Z0-9_][a-zA-Z0-9_]*"? |
                ?lexer squared?, ?@\[[a-zA-Z0-9_][a-zA-Z0-9_]*\]? |
                ?lexer backtick?, ?@`[a-zA-Z0-9_][a-zA-Z0-9_]*`?;)");
    DECL_RULE(identifier_list, R"(
    identifier list = identifier, {',', identifier};)");
    DECL_RULE(table_identifier, R"(
    table identifier = identifier, ['.', identifier];)");
    DECL_RULE(aliased_table_identifier, R"(
    aliased table identifier = table identifier, [["AS"], identifier];)");
    // import_table_stmt.hpp
    DECL_RULE(import_table_file, R"(
    import table file =
        ?lexer file?,
        ?@[a-zA-Z0-9._\-/\\:][a-zA-Z0-9._\-/\\:]*?;)");
    DECL_RULE(import_table_stmt, R"(
    import table stmt =
        "IMPORT", "TABLE",
        table identifier,
        "FROM",
        "CSV", "FILE", import table file, "DELIMITED", "BY", string literal;)");
    // join_clause.hpp
    DECL_RULE(join_operator, R"(
    join operator =
        ["NATURAL"],
        ("LEFT", ["OUTER"] |
        "INNER" |
        "CROSS" |
        ),
        "JOIN";)");
    DECL_RULE(join_constraint, R"(
    join constraint =
        "ON", expr |
        "USING", '(', identifier list, ')' |
        ;)");
    DECL_RULE(join, R"(
    join = join operator, table or subquery, join constraint;)");
    DECL_RULE(join_list, "join list = {join};");
    DECL_RULE(join_clause, "join clause = table or subquery, join list;");
    // literal_value.hpp
    // modified from C11
    DECL_RULE(numeric_literal, R"(
    numeric literal = 
        ?lexer floating literal?,
            (?@[0-9][0-9]*\.? | ?@[0-9]*\.[0-9][0-9]*?) | 
        ?lexer integer literal?,
            (?@[1-9][0-9]*? | '0');)");
    DECL_RULE(string_literal, R"(
    string literal =
        ?lexer string literal?,
        [?@[LuU]?], "'", (
            ?@[!-&(-[\]-~\a\b\f\r\t\v]? |
            '\\', (?@['"\\abfnrtv]? | '?') |
            ('\\', ?oct?, [?oct?, [?oct?]]) |
            ?@\x[0-9a-fA-F][0-9a-fA-F]*? ),
        "'";)");
    DECL_RULE(literal_value, R"(
    literal value =
                    numeric literal |
                    string literal |
                    "NULL" |
                    "CURRENT_DATE" |
                    "CURRENT_TIMESTAMP" |
                    "CURRENT_TIME";)");
    // result_column.hpp
    DECL_RULE(result_column, R"(
    result column =
        "*" |
        table identifier, ".", "*" |
        expr, [["AS"], identifier];)");
    DECL_RULE(result_column_list, R"(
    result column list = result column, {',', result column};)");
    // select_core.hpp
    DECL_RULE(select_type, R"(select type = "DISTINCT" | "ALL" | ;)");
    DECL_RULE(select_core_select, R"(
    select core select = "SELECT", select type, result column list,
        ["FROM", (
            table or subquery,
                join operator, table or subquery, join constraint,
                join clause |
            table or subquery list
        )],
        ["WHERE", expr],
        ["GROUP", "BY", expr list,
            ["HAVING", expr]];)");
    DECL_RULE(select_core_values, R"(
    select core values = '(', expr list, ')',
        {',', '(', expr list, ')'};)");
    DECL_RULE(select_core, R"(
    select core = select core select | select core values;)");
    // select_stmt.hpp
    DECL_RULE(ordering_term, "ordering term = expr, direction type;");
    DECL_RULE(select_stmt, R"(
    select stmt = select core,
        ["ORDER", "BY", ordering term, {',', ordering term}],
        ["LIMIT", expr,
            [',' | "OFFSET"],
            expr];)");
    // sql.hpp
    DECL_RULE(sql, "sql = sql stmt list;");
    // sql_stmt.hpp
    DECL_RULE(sql_stmt, R"(
    sql stmt = ["EXPLAIN", ["QUERY", "PLAN"]], (
                select stmt |
                create table stmt |
                import table stmt);)");
    DECL_RULE(sql_stmt_list, R"(
    sql stmt list = sql stmt, {';', sql stmt};)");
    // table_or_subquery.hpp
    DECL_RULE(table_or_subquery_index, R"(
    table or subquery index =
        "INDEXED", "BY", identifier |
        "NOT", "INDEXED" |
        ;)");
    DECL_RULE(table_or_subquery_table, R"(
    table or subquery table =
        aliased table identifier, table or subquery index;)");
    DECL_RULE(table_or_subquery_sublist, R"(
    table or subquery sublist =
        '(', table or subquery list, ')';)");
    DECL_RULE(table_or_subquery_join, R"(
    table or subquery join =
        '(', join clause, ')';)");
    DECL_RULE(table_or_subquery_select, R"(
    table or subquery select =
        '(', select stmt, ')',
        [["AS"], identifier];)");
    DECL_RULE(table_or_subquery, R"(
    table or subquery =
        aliased table identifier, table or subquery index |
        '(', (
            table or subquery list |
            join clause |
            select stmt), ')';)");
    DECL_RULE(table_or_subquery_list, R"(
    table or subquery list =
        table or subquery, {',', table or subquery};)");
    using sql_rules = code_to_rules_t<direction_type_rule,
                                      type_rule,
                                      column_def_rule,
                                      column_def_list_rule,
                                      indexed_column_rule,
                                      indexed_column_list_rule,
                                      table_constraint_type_rule,
                                      table_constraint_rule,
                                      table_constraint_list_rule,
                                      create_table_stmt_definition_rule,
                                      create_table_stmt_rule,
                                      function_params_rule,
                                      function_call_rule,
                                      p1_rule,
                                      p2_rule,
                                      p3_rule,
                                      p4_rule,
                                      p5_rule,
                                      p6_rule,
                                      p7_rule,
                                      p8_rule,
                                      expr_left_rule,
                                      bo_lhs_p1_rule,
                                      bo_lhs_p2_rule,
                                      bo_lhs_p3_rule,
                                      bo_lhs_p4_rule,
                                      bo_lhs_p5_rule,
                                      bo_lhs_p6_rule,
                                      bo_lhs_p7_rule,
                                      bo_lhs_p8_rule,
                                      bo_rhs_p1_rule,
                                      bo_rhs_p2_rule,
                                      bo_rhs_p3_rule,
                                      bo_rhs_p4_rule,
                                      bo_rhs_p5_rule,
                                      bo_rhs_p6_rule,
                                      bo_rhs_p7_rule,
                                      bo_rhs_p8_rule,
                                      bo_p1_rule,
                                      bo_p2_rule,
                                      bo_p3_rule,
                                      bo_p4_rule,
                                      bo_p5_rule,
                                      bo_p6_rule,
                                      bo_p7_rule,
                                      bo_p8_rule,
                                      binary_operation_rule,
                                      expr_rule,
                                      expr_list_rule,
                                      identifier_rule,
                                      identifier_list_rule,
                                      table_identifier_rule,
                                      aliased_table_identifier_rule,
                                      import_table_file_rule,
                                      import_table_stmt_rule,
                                      join_operator_rule,
                                      join_constraint_rule,
                                      join_rule,
                                      join_clause_rule,
                                      join_list_rule,
                                      numeric_literal_rule,
                                      string_literal_rule,
                                      literal_value_rule,
                                      result_column_rule,
                                      result_column_list_rule,
                                      select_type_rule,
                                      select_core_select_rule,
                                      select_core_values_rule,
                                      select_core_rule,
                                      ordering_term_rule,
                                      select_stmt_rule,
                                      sql_rule,
                                      sql_stmt_rule,
                                      sql_stmt_list_rule,
                                      table_or_subquery_rule,
                                      table_or_subquery_index_rule,
                                      table_or_subquery_list_rule,
                                      table_or_subquery_join_rule,
                                      table_or_subquery_select_rule,
                                      table_or_subquery_sublist_rule,
                                      table_or_subquery_table_rule>;
} // namespace pis
#endif
