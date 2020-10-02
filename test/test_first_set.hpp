// ParserInString -*- C++ -*-

// Copyright (C) 2019 Scott Brown

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

#ifndef PARSER_IN_STRING_MATCHER_TESTFIRSTSET
#define PARSER_IN_STRING_MATCHER_TESTFIRSTSET

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Matcher/FirstFollow.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <type_traits>

static constexpr const char empty_rules[] = " ";
using empty_rules_result                  = tavl::empty_node;
static constexpr const char single_rule[] = R"(abc = "abc", "def";)";
using single_rule_1_result                = tavl::tavl_node<
    tavl::empty_node,
    tavl::empty_node,
    0,
    std::integer_sequence<char, 'a', 'b', 'c'>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>>,
        std::integral_constant<bool, true>>>;
using single_rule_2_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::empty_node,
    0,
    std::integer_sequence<char, 'a', 'b', 'c'>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>,
                            std::integer_sequence<char, 'd', 'e', 'f'>>,
        std::integral_constant<bool, true>>>;
static constexpr const char multi_rules_simple[] = R"(
a = "abc", "def";
b = "def", "ghi", "jkl";
c = a;
)";
using multi_rules_simple_1_result                = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>>,
            std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'c'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>>,
            std::integral_constant<bool, true>>>,
    1,
    std::integer_sequence<char, 'b'>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        Compiler::type_list<std::integer_sequence<char, 'd', 'e', 'f'>>,
        std::integral_constant<bool, true>>>;
using multi_rules_simple_2_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'c'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            std::integral_constant<bool, true>>>,
    1,
    std::integer_sequence<char, 'b'>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        Compiler::type_list<std::integer_sequence<char, 'd', 'e', 'f'>,
                            std::integer_sequence<char, 'g', 'h', 'i'>>,
        std::integral_constant<bool, true>>>;
using multi_rules_simple_3_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'c'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            std::integral_constant<bool, true>>>,
    1,
    std::integer_sequence<char, 'b'>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        Compiler::type_list<std::integer_sequence<char, 'd', 'e', 'f'>,
                            std::integer_sequence<char, 'g', 'h', 'i'>,
                            std::integer_sequence<char, 'j', 'k', 'l'>>,
        std::integral_constant<bool, true>>>;
using multi_rules_simple_4_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'c'>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            Compiler::type_list<std::integer_sequence<char, 'a', 'b', 'c'>,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            std::integral_constant<bool, true>>>,
    1,
    std::integer_sequence<char, 'b'>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        Compiler::type_list<std::integer_sequence<char, 'd', 'e', 'f'>,
                            std::integer_sequence<char, 'g', 'h', 'i'>,
                            std::integer_sequence<char, 'j', 'k', 'l'>>,
        std::integral_constant<bool, true>>>;
static constexpr const char multi_rules_complex[] = R"(
Expr = Expr, '+', Multi |
        Expr, '-', Multi |
        Multi;
Multi = Multi, "*", Const |
        Multi, '/', Const |
        Const;
Const = "i" |
        '(', Expr, ')';
)";
using multi_rules_complex_1_result                = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::empty_node,
                        1,
                        Compiler::type_list<std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::empty_node,
                        1,
                        Compiler::type_list<std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>>,
    1,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    tavl::tavl_node<
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
        tavl::empty_node,
        1,
        Compiler::type_list<std::integer_sequence<char, 'i'>>,
        std::integral_constant<bool, true>>>;
using multi_rules_complex_2_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, 'i'>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::empty_node,
                tavl::empty_node,
                0,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '/'>>,
                std::integral_constant<bool, true>>,
            2,
            Compiler::type_list<std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '*'>>,
            std::integral_constant<bool, true>>>,
    1,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    tavl::tavl_node<
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::empty_node,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::empty_node,
                tavl::empty_node,
                0,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '*'>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::empty_node,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '-'>>,
                std::integral_constant<bool, true>>,
            2,
            Compiler::type_list<std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '+'>>,
            std::integral_constant<bool, true>>,
        3,
        Compiler::type_list<std::integer_sequence<char, 'i'>>,
        std::integral_constant<bool, true>>>;
using multi_rules_complex_3_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, ')'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '+'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>>,
                        std::integral_constant<bool, true>>,
                    tavl::empty_node,
                    1,
                    Compiler::type_list<std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '-'>>,
                std::integral_constant<bool, true>>,
            3,
            Compiler::type_list<std::integer_sequence<char, '('>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '*'>>,
            std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
                    tavl::empty_node,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '*'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, ')'>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '-'>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '*'>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '*'>,
                                    std::integer_sequence<char, '('>>,
                std::integral_constant<bool, true>>,
            3,
            Compiler::type_list<std::integer_sequence<char, '('>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '+'>>,
            std::integral_constant<bool, true>>>,
    1,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::empty_node,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '+'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '*'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '-'>>,
                std::integral_constant<bool, true>>,
            3,
            Compiler::type_list<std::integer_sequence<char, '('>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, ')'>>,
            std::integral_constant<bool, true>>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '*'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '+'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '+'>,
                                    std::integer_sequence<char, '('>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '-'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '/'>,
                                    std::integer_sequence<char, '('>>,
                std::integral_constant<bool, true>>,
            2,
            Compiler::type_list<std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '-'>,
                                std::integer_sequence<char, '('>>,
            std::integral_constant<bool, true>>,
        4,
        Compiler::type_list<std::integer_sequence<char, 'i'>,
                            std::integer_sequence<char, '*'>,
                            std::integer_sequence<char, '('>>,
        std::integral_constant<bool, true>>>;
using empty_definition_test_result = tavl::empty_node;
using test_definitions             = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        Compiler::type_list<
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, 'i'>>>,
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '('>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        Compiler::type_list<
            Compiler::type_list<
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '*'>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>>>,
            Compiler::type_list<
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '/'>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>>>,
            Compiler::type_list<Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>>>>>,
    1,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    Compiler::type_list<
        Compiler::type_list<
            Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
            Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                std::integer_sequence<char, '+'>>,
            Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>>,
        Compiler::type_list<
            Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
            Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                std::integer_sequence<char, '-'>>,
            Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>>,
        Compiler::type_list<Compiler::type_pair<
            Compiler::Impl::meta_identifier_flag,
            std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>>>>;
using terminal_1_result =
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>;
using nonterminal_1_result = tavl::tavl_node<
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
    tavl::empty_node,
    1,
    Compiler::type_list<std::integer_sequence<char, 'i'>>,
    std::integral_constant<bool, true>>;
using terminal_nonterminal_2_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
    1,
    Compiler::type_list<std::integer_sequence<char, '('>,
                        std::integer_sequence<char, '('>>,
    std::integral_constant<bool, true>>;
using nonterminal_terminal_2_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, ')'>>,
                                        std::integral_constant<bool, true>>,
                        tavl::empty_node,
                        1,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '*'>>,
                        std::integral_constant<bool, true>>,
        2,
        Compiler::type_list<std::integer_sequence<char, '('>,
                            std::integer_sequence<char, 'i'>>,
        std::integral_constant<bool, true>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>>,
                        std::integral_constant<bool, true>>,
        1,
        Compiler::type_list<std::integer_sequence<char, 'i'>,
                            std::integer_sequence<char, '-'>>,
        std::integral_constant<bool, true>>,
    3,
    Compiler::type_list<std::integer_sequence<char, 'i'>,
                        std::integer_sequence<char, '+'>>,
    std::integral_constant<bool, true>>;
using nonterminal_nonterminal_2_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::empty_node,
                        1,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '*'>>,
                        std::integral_constant<bool, true>>,
        2,
        Compiler::type_list<std::integer_sequence<char, '('>,
                            std::integer_sequence<char, 'i'>>,
        std::integral_constant<bool, true>>,
    tavl::tavl_node<
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '-'>>,
                        std::integral_constant<bool, true>>,
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
        1,
        Compiler::type_list<std::integer_sequence<char, 'i'>,
                            std::integer_sequence<char, '/'>>,
        std::integral_constant<bool, true>>,
    3,
    Compiler::type_list<std::integer_sequence<char, 'i'>,
                        std::integer_sequence<char, '+'>>,
    std::integral_constant<bool, true>>;
using terminal_terminal_2_result =
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, ')'>>,
                    std::integral_constant<bool, true>>;
static constexpr const char multi_rules_complex_2[] = R"(
Expr = Multi, Expr1;
Expr1 = '+', Multi, Expr1 |
        '-', Multi, Expr1 |
        ;
Multi = Const, Multi1;
Multi1 = '*', Const, Multi1 |
         '/', Const, Multi1 |
         ;
Const = "i" |
        '(', Expr, ')';
)";
using multi_rules_complex_2_1_result                = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::empty_node,
                        1,
                        Compiler::type_list<std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'E', 'x', 'p', 'r', '1'>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '+'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<tavl::empty_node,
                                tavl::empty_node,
                                0,
                                Compiler::type_list<std::integer_sequence<char,
                                                                          '@',
                                                                          'E',
                                                                          'M',
                                                                          'P',
                                                                          'T',
                                                                          'Y'>>,
                                std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '-'>>,
                std::integral_constant<bool, true>>>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'M', 'u', 'l', 't', 'i', '1'>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '*'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<tavl::empty_node,
                                tavl::empty_node,
                                0,
                                Compiler::type_list<std::integer_sequence<char,
                                                                          '@',
                                                                          'E',
                                                                          'M',
                                                                          'P',
                                                                          'T',
                                                                          'Y'>>,
                                std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '/'>>,
                std::integral_constant<bool, true>>>,
        1,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::empty_node,
                        1,
                        Compiler::type_list<std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>>,
    2,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    tavl::tavl_node<
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
        tavl::empty_node,
        1,
        Compiler::type_list<std::integer_sequence<char, 'i'>>,
        std::integral_constant<bool, true>>>;
using multi_rules_complex_2_2_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, 'i'>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'E', 'x', 'p', 'r', '1'>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '+'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '-'>,
                                            std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char,
                                                                  '@',
                                                                  'E',
                                                                  'M',
                                                                  'P',
                                                                  'T',
                                                                  'Y'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '-'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '+'>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'M', 'u', 'l', 't', 'i', '1'>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '*'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '/'>,
                                            std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char,
                                                                  '@',
                                                                  'E',
                                                                  'M',
                                                                  'P',
                                                                  'T',
                                                                  'Y'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '/'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '*'>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>>,
        1,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::empty_node,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::empty_node,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '*'>>,
                std::integral_constant<bool, true>>,
            2,
            Compiler::type_list<std::integer_sequence<char, 'i'>>,
            std::integral_constant<bool, true>>>,
    2,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    tavl::tavl_node<
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, 'i'>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
        tavl::tavl_node<tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '+'>>,
                                        std::integral_constant<bool, true>>,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '-'>>,
                        std::integral_constant<bool, true>>,
        2,
        Compiler::type_list<std::integer_sequence<char, 'i'>,
                            std::integer_sequence<char, '*'>>,
        std::integral_constant<bool, true>>>;
using multi_rules_complex_2_3_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, ')'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '+'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>>,
                        std::integral_constant<bool, true>>,
                    tavl::empty_node,
                    1,
                    Compiler::type_list<std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '-'>>,
                std::integral_constant<bool, true>>,
            3,
            Compiler::type_list<std::integer_sequence<char, '('>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '*'>>,
            std::integral_constant<bool, true>>>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'E', 'x', 'p', 'r', '1'>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '+'>,
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '+'>,
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '+'>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '*'>>,
                        std::integral_constant<bool, true>>,
                    2,
                    Compiler::type_list<std::integer_sequence<char, '+'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '+'>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '-'>>,
                                        std::integral_constant<bool, true>>,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '-'>,
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '+'>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::tavl_node<
                            tavl::tavl_node<
                                tavl::empty_node,
                                tavl::empty_node,
                                0,
                                Compiler::type_list<
                                    std::integer_sequence<char, '-'>,
                                    std::integer_sequence<char, 'i'>>,
                                std::integral_constant<bool, true>>,
                            tavl::tavl_node<
                                tavl::empty_node,
                                tavl::empty_node,
                                0,
                                Compiler::type_list<
                                    std::integer_sequence<char, '-'>,
                                    std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '+'>>,
                                std::integral_constant<bool, true>>,
                            1,
                            Compiler::type_list<
                                std::integer_sequence<char, '-'>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '*'>>,
                            std::integral_constant<bool, true>>,
                        tavl::tavl_node<
                            tavl::empty_node,
                            tavl::tavl_node<
                                tavl::empty_node,
                                tavl::empty_node,
                                0,
                                Compiler::type_list<std::integer_sequence<char,
                                                                          '@',
                                                                          'E',
                                                                          'M',
                                                                          'P',
                                                                          'T',
                                                                          'Y'>>,
                                std::integral_constant<bool, true>>,
                            1,
                            Compiler::type_list<
                                std::integer_sequence<char, '-'>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '/'>>,
                            std::integral_constant<bool, true>>,
                        2,
                        Compiler::type_list<std::integer_sequence<char, '-'>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '-'>>,
                        std::integral_constant<bool, true>>,
                    3,
                    Compiler::type_list<std::integer_sequence<char, '-'>,
                                        std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                4,
                Compiler::type_list<std::integer_sequence<char, '+'>,
                                    std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '+'>>,
                std::integral_constant<bool, true>>>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'M', 'u', 'l', 't', 'i', '1'>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '*'>,
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '*'>,
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '*'>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '*'>>,
                                        std::integral_constant<bool, true>>,
                        tavl::tavl_node<tavl::empty_node,
                                        tavl::empty_node,
                                        0,
                                        Compiler::type_list<
                                            std::integer_sequence<char, '/'>,
                                            std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, '('>>,
                                        std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '*'>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>>,
                        std::integral_constant<bool, true>>,
                    2,
                    Compiler::type_list<std::integer_sequence<char, '*'>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '/'>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::tavl_node<
                            tavl::empty_node,
                            tavl::empty_node,
                            0,
                            Compiler::type_list<std::integer_sequence<char,
                                                                      '@',
                                                                      'E',
                                                                      'M',
                                                                      'P',
                                                                      'T',
                                                                      'Y'>>,
                            std::integral_constant<bool, true>>,
                        1,
                        Compiler::type_list<std::integer_sequence<char, '/'>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>>,
                        std::integral_constant<bool, true>>,
                    2,
                    Compiler::type_list<std::integer_sequence<char, '/'>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '*'>>,
                    std::integral_constant<bool, true>>,
                3,
                Compiler::type_list<std::integer_sequence<char, '/'>,
                                    std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>>,
        1,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '*'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, ')'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '-'>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '*'>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '*'>,
                                    std::integer_sequence<char, '('>>,
                std::integral_constant<bool, true>>,
            3,
            Compiler::type_list<std::integer_sequence<char, '('>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '+'>>,
            std::integral_constant<bool, true>>>,
    2,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::empty_node,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, '('>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, '('>,
                                            std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '+'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '*'>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, '('>,
                                        std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, '('>,
                                    std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '-'>>,
                std::integral_constant<bool, true>>,
            3,
            Compiler::type_list<std::integer_sequence<char, '('>,
                                std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, ')'>>,
            std::integral_constant<bool, true>>,
        tavl::tavl_node<
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '*'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '+'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                1,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '*'>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>,
            tavl::tavl_node<
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::empty_node,
                    0,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '-'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                tavl::tavl_node<
                    tavl::empty_node,
                    tavl::tavl_node<
                        tavl::empty_node,
                        tavl::empty_node,
                        0,
                        Compiler::type_list<std::integer_sequence<char, 'i'>,
                                            std::integer_sequence<char, '/'>,
                                            std::integer_sequence<char, 'i'>>,
                        std::integral_constant<bool, true>>,
                    1,
                    Compiler::type_list<std::integer_sequence<char, 'i'>,
                                        std::integer_sequence<char, '/'>,
                                        std::integer_sequence<char, '('>>,
                    std::integral_constant<bool, true>>,
                2,
                Compiler::type_list<std::integer_sequence<char, 'i'>,
                                    std::integer_sequence<char, '-'>,
                                    std::integer_sequence<char, 'i'>>,
                std::integral_constant<bool, true>>,
            3,
            Compiler::type_list<std::integer_sequence<char, 'i'>,
                                std::integer_sequence<char, '+'>,
                                std::integer_sequence<char, 'i'>>,
            std::integral_constant<bool, true>>,
        4,
        Compiler::type_list<std::integer_sequence<char, 'i'>>,
        std::integral_constant<bool, true>>>;
namespace Compiler
{
    namespace FirstSetGenerationTest
    {
        using test_empty_rules =
            typename first_set<code_to_rules_t<empty_rules>, 1>::type;
        static_assert(
            tavl::tavl_is_same_v<test_empty_rules, empty_rules_result>,
            "test for first-set for empty rules");
        using test_single_rule_1 =
            typename first_set<code_to_rules_t<single_rule>, 1>::type;
        static_assert(
            tavl::tavl_is_same_v<test_single_rule_1, single_rule_1_result>,
            "test for initial sets(K=1)");
        using test_single_rule_2 =
            typename first_set<code_to_rules_t<single_rule>, 2>::type;
        static_assert(std::is_same_v<test_single_rule_2, single_rule_2_result>,
                      "test for initial sets(K=2)");
        using test_single_rule_3 =
            typename first_set<code_to_rules_t<single_rule>, 3>::type;
        static_assert(
            tavl::tavl_is_same_v<test_single_rule_3, single_rule_2_result>,
            "test for initial sets(K>{max len})");
        using test_multi_rules_simple_1 =
            typename first_set<code_to_rules_t<multi_rules_simple>, 1>::type;
        static_assert(tavl::tavl_is_same_v<test_multi_rules_simple_1,
                                           multi_rules_simple_1_result>,
                      "test for multi-rules with only one transformation(K=1)");
        using test_multi_rules_simple_2 =
            typename first_set<code_to_rules_t<multi_rules_simple>, 2>::type;
        static_assert(tavl::tavl_is_same_v<test_multi_rules_simple_2,
                                           multi_rules_simple_2_result>,
                      "test for multi-rules with only one transformation(K=2)");
        using test_multi_rules_simple_3 =
            typename first_set<code_to_rules_t<multi_rules_simple>, 3>::type;
        static_assert(tavl::tavl_is_same_v<test_multi_rules_simple_3,
                                           multi_rules_simple_3_result>,
                      "test for multi-rules with only one transformation(K=3)");
        using test_multi_rules_simple_4 =
            typename first_set<code_to_rules_t<multi_rules_simple>, 4>::type;
        static_assert(
            tavl::tavl_is_same_v<test_multi_rules_simple_4,
                                 multi_rules_simple_3_result>,
            "test for multi-rules with only one transformation(K>{max len})");
        using test_multi_rules_complex_1 =
            first_set_k<code_to_rules_t<multi_rules_complex>, 1>;
        static_assert(tavl::tavl_is_same_v<test_multi_rules_complex_1,
                                           multi_rules_complex_1_result>,
                      "test using calculator grammars");

        using test_multi_rules_complex_2 =
            first_set_k<code_to_rules_t<multi_rules_complex>, 2>;
        static_assert(tavl::tavl_is_same_v<test_multi_rules_complex_2,
                                           multi_rules_complex_2_result>,
                      "test using calculator grammars(K=2)");
        using test_multi_rules_complex_3 =
            first_set_k<code_to_rules_t<multi_rules_complex>, 3>;
        static_assert(tavl::tavl_is_same_v<test_multi_rules_complex_3,
                                           multi_rules_complex_3_result>,
                      "test using calculator grammars(K=3)");
        using test_multi_rules_complex_2_1 =
            first_set_k<code_to_rules_t<multi_rules_complex_2>, 1>;
        static_assert(tavl::tavl_is_same_v<test_multi_rules_complex_2_1,
                                           multi_rules_complex_2_1_result>,
                      "test using calculator grammars with empty definition");
        using test_multi_rules_complex_2_2 =
            first_set_k<code_to_rules_t<multi_rules_complex_2>, 2>;
        static_assert(
            tavl::tavl_is_same_v<test_multi_rules_complex_2_2,
                                 multi_rules_complex_2_2_result>,
            "test using calculator grammars with empty definition(K=2)");
        using test_multi_rules_complex_2_3 =
            first_set_k<code_to_rules_t<multi_rules_complex_2>, 3>;
        static_assert(
            tavl::tavl_is_same_v<test_multi_rules_complex_2_3,
                                 multi_rules_complex_2_3_result>,
            "test using calculator grammars with empty definition(K=3)");
    } // namespace FirstSetGenerationTest
    namespace FirstSetForDefinitonTest
    {
        using test_empty_definition =
            first_set_for_definition_t<tavl::empty_node, type_list<>, 1>;
        static_assert(tavl::tavl_is_same_v<test_empty_definition,
                                           empty_definition_test_result>,
                      "test for first-set of empty definition");
        using test_terminal_1 = first_set_for_definition_t<
            multi_rules_complex_1_result,
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '('>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>>,
            1>;
        static_assert(
            tavl::tavl_is_same_v<test_terminal_1, terminal_1_result>,
            "test for first-set of definition starting with terminal symbols");
        using test_nonterminal_1 = first_set_for_definition_t<
            multi_rules_complex_1_result,
            Compiler::type_list<Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'E', 'x', 'p', 'r'>>>,
            1>;
        static_assert(
            tavl::tavl_is_same_v<test_nonterminal_1, nonterminal_1_result>,
            "test for first-set of definition starting with nonterminal symbols");
        using test_terminal_nonterminal_2 = first_set_for_definition_t<
            multi_rules_complex_2_result,
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '('>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>>,
            2>;
        static_assert(
            tavl::tavl_is_same_v<test_terminal_nonterminal_2,
                                 terminal_nonterminal_2_result>,
            "test for first-set of definition starting with terminal+nonterminal(k=2)");
        using test_nonterminal_terminal_2 = first_set_for_definition_t<
            multi_rules_complex_2_result,
            Compiler::type_list<
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>>,
            2>;
        static_assert(
            tavl::tavl_is_same_v<test_nonterminal_terminal_2,
                                 nonterminal_terminal_2_result>,
            "test for first-set of definition starting with nonterminal+terminal(k=2)");
        using test_nonterminal_nonterminal_2 = first_set_for_definition_t<
            multi_rules_complex_2_result,
            Compiler::type_list<
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>>,
            2>;
        static_assert(
            tavl::tavl_is_same_v<test_nonterminal_nonterminal_2,
                                 nonterminal_nonterminal_2_result>,
            "test for first-set of definition starting with 2 nonterminals(k=2)");
        using test_terminal_terminal_2 = first_set_for_definition_t<
            multi_rules_complex_2_result,
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '('>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'E', 'x', 'p', 'r'>>>,
            2>;
        static_assert(
            tavl::tavl_is_same_v<test_terminal_terminal_2,
                                 terminal_terminal_2_result>,
            "test for first-set of definition starting with 2 terminals(k=2)");
    } // namespace FirstSetForDefinitonTest
} // namespace Compiler
#endif
