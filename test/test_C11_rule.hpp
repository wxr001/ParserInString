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

#ifndef PARSER_IN_STRING_TEST_C11_RULE
#define PARSER_IN_STRING_TEST_C11_RULE

#include "pis/Common/Tstring.hpp"
#include "pis/Lexer/Lexer.hpp"
#include "pis/Matcher/LL.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include "pis/Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <doctest.h>
#include <type_traits>
#include "test_C11.hpp"
namespace pis
{
    using c11 = code_to_rules_t<identifier_rule,
                                constant_rule,
                                integer_constant_rule,
                                floating_constant_rule,
                                enumeration_constant_rule,
                                character_constant_rule,
                                string_literal_rule,
                                primary_rule,
                                generic_selection_rule,
                                generic_assoc_list_rule,
                                generic_association_rule,
                                postfix_rule,
                                argument_expression_list_rule,
                                unary_expression_rule,
                                unary_operator_rule,
                                cast_expression_rule,
                                multiplicative_rule,
                                additive_rule,
                                shift_rule,
                                relational_rule,
                                equality_expression_rule,
                                and_expression_rule,
                                exclusive_rule,
                                inclusive_rule,
                                logical_or_rule,
                                logical_and_rule,
                                conditional_rule,
                                assignment_expression_rule,
                                assignment_operator_rule,
                                expression_rule,
                                constant_expression_rule,
                                declaration_rule,
                                declaration_specifiers_rule,
                                init_declarator_list_rule,
                                init_declarator_rule,
                                storage_class_specifier_rule,
                                type_specifier_rule,
                                struct_or_union_specifier_rule,
                                struct_or_union_rule,
                                struct_declaration_list_rule,
                                struct_declaration_rule,
                                specifier_qualifier_list_rule,
                                struct_declarator_list_rule,
                                struct_declarator_rule,
                                enum_specifier_rule,
                                enumerator_list_rule,
                                enumerator_rule,
                                atomic_type_specifier_rule,
                                type_qualifier_rule,
                                function_specifier_rule,
                                alignment_specifier_rule,
                                declarator_rule,
                                direct_declarator_rule,
                                pointer_rule,
                                type_qualifier_list_rule,
                                parameter_type_list_rule,
                                parameter_list_rule,
                                parameter_declaration_rule,
                                identifier_list_rule,
                                type_name_rule,
                                abstract_declarator_rule,
                                direct_abstract_declarator_rule,
                                direct_abstract_declarator_ext_rule,
                                typedef_name_rule,
                                initializer_rule,
                                initializer_list_rule,
                                designation_rule,
                                designator_list_rule,
                                designator_rule,
                                static_assert_declaration_rule,
                                statement_rule,
                                labeled_statement_rule,
                                compound_statement_rule,
                                block_item_list_rule,
                                block_item_rule,
                                expression_statement_rule,
                                selection_statement_rule,
                                iteration_statement_rule,
                                jump_statement_rule,
                                translation_unit_rule,
                                external_declaration_rule,
                                function_definition_rule,
                                declaration_list_rule>;
} // namespace pis

#endif
