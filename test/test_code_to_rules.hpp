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

#ifndef PARSER_IN_STRING_MATCHER_TESTCODETORULES
#define PARSER_IN_STRING_MATCHER_TESTCODETORULES
#include "pis/Common/Identity.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include "TAVL.hpp"
#include <type_traits>
static constexpr const char empty_rules[]    = " ";
using empty_rules_result                     = tavl::empty_node;
static constexpr const char failed_rules[]   = R"(
R1 = {[R2, R2, , "abc"]} | R1 |;)";
static constexpr const char unmerged_rules[] = R"(
R1 = ("a", "+"), R2 | R2;
R2 = "(", R1, ')' | "i"; )";
using unmerged_rules_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, '@', '0'>,
        pis::type_list<pis::type_list<
            pis::type_pair<pis::Impl::terminal_string_flag,
                                std::integer_sequence<char, 'a'>>,
            pis::type_pair<pis::Impl::terminal_string_flag,
                                std::integer_sequence<char, '+'>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'R', '2'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '('>>,
                pis::type_pair<pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'R', '1'>>,
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>>,
            pis::type_list<
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, 'i'>>>>>,
    1,
    std::integer_sequence<char, 'R', '1'>,
    pis::type_list<
        pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '0'>>,
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'R', '2'>>>,
        pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'R', '2'>>>>>;
static constexpr const char lexer_rules[] = R"(
R1 = [R2], R3 | [R2];
R2 = {R2}, R3 | R3;
R3 = ?lexer abc?, {R3} | ;)";
using lexer_rules_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, '@', '0'>,
            pis::type_list<pis::type_list<pis::type_pair<
                                    pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'R', '2'>>>,
                                pis::type_list<pis::type_pair<
                                    pis::Impl::empty_sequence_flag,
                                    std::integer_sequence<char>>>>>,
        tavl::empty_node,
        1,
        std::integer_sequence<char, '@', '1'>,
        pis::type_list<pis::type_list<pis::type_pair<
                                pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'R', '2'>>>,
                            pis::type_list<pis::type_pair<
                                pis::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'R', '1'>,
            pis::type_list<
                pis::type_list<
                    pis::type_pair<pis::Impl::meta_identifier_flag,
                                        std::integer_sequence<char, '@', '0'>>,
                    pis::type_pair<pis::Impl::meta_identifier_flag,
                                        std::integer_sequence<char, 'R', '3'>>>,
                pis::type_list<pis::type_pair<
                    pis::Impl::meta_identifier_flag,
                    std::integer_sequence<char, '@', '1'>>>>>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'R', '3'>,
            pis::type_list<
                pis::type_list<
                    pis::type_pair<pis::Impl::special_sequence_flag,
                                        std::integer_sequence<char,
                                                              'l',
                                                              'e',
                                                              'x',
                                                              'e',
                                                              'r',
                                                              ' ',
                                                              'a',
                                                              'b',
                                                              'c'>>,
                    pis::type_pair<
                        pis::Impl::repeated_sequence_flag,
                        pis::type_list<
                            pis::type_list<pis::type_pair<
                                pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'R', '3'>>>>>>,
                pis::type_list<
                    pis::type_pair<pis::Impl::empty_sequence_flag,
                                        std::integer_sequence<char>>>>>,
        1,
        std::integer_sequence<char, 'R', '2'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '2'>>,
                pis::type_pair<pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'R', '3'>>>,
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'R', '3'>>>>>,
    2,
    std::integer_sequence<char, '@', '2'>,
    pis::type_list<
        pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'R', '2'>>,
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '2'>>>,
        pis::type_list<
            pis::type_pair<pis::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>;
static constexpr const char calculator[] = R"(
Primary = Primary, "+", Sub | Primary, "-", Sub | Sub;
Sub = Sub, "*", Factor | Sub, "/", Factor | Factor;
Factor = '(', Primary, ')' | 'i';
)";
using calculator_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'F', 'a', 'c', 't', 'o', 'r'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '('>>,
                pis::type_pair<pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char,
                                                          'P',
                                                          'r',
                                                          'i',
                                                          'm',
                                                          'a',
                                                          'r',
                                                          'y'>>,
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>>,
            pis::type_list<
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, 'i'>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'S', 'u', 'b'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'S', 'u', 'b'>>,
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '*'>>,
                pis::type_pair<
                    pis::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'F', 'a', 'c', 't', 'o', 'r'>>>,
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'S', 'u', 'b'>>,
                pis::type_pair<pis::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '/'>>,
                pis::type_pair<
                    pis::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'F', 'a', 'c', 't', 'o', 'r'>>>,
            pis::type_list<pis::type_pair<
                pis::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'F', 'a', 'c', 't', 'o', 'r'>>>>>,
    1,
    std::integer_sequence<char, 'P', 'r', 'i', 'm', 'a', 'r', 'y'>,
    pis::type_list<
        pis::type_list<
            pis::type_pair<
                pis::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'P', 'r', 'i', 'm', 'a', 'r', 'y'>>,
            pis::type_pair<pis::Impl::terminal_string_flag,
                                std::integer_sequence<char, '+'>>,
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'S', 'u', 'b'>>>,
        pis::type_list<
            pis::type_pair<
                pis::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'P', 'r', 'i', 'm', 'a', 'r', 'y'>>,
            pis::type_pair<pis::Impl::terminal_string_flag,
                                std::integer_sequence<char, '-'>>,
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'S', 'u', 'b'>>>,
        pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'S', 'u', 'b'>>>>>;
DECL_RULE(redundant, "A = 'a'; A = 'b';");
using redundant_decl_merged = tavl::tavl_node<
    tavl::empty_node,
    tavl::empty_node,
    0,
    std::integer_sequence<char, 'A'>,
    pis::type_list<pis::type_list<pis::type_pair<
                            pis::Impl::terminal_string_flag,
                            std::integer_sequence<char, 'b'>>>,
                        pis::type_list<pis::type_pair<
                            pis::Impl::terminal_string_flag,
                            std::integer_sequence<char, 'a'>>>>>;

namespace pis
{
    using test_empty_rules = code_to_rules_t<empty_rules>;
    static_assert(std::is_same_v<test_empty_rules, empty_rules_result>,
                  "empty rules test");
    // expected failure
    // using test_failed_rules = code_to_rules_t<failed_rules>;
    using test_unmerged_rules = code_to_rules_t<unmerged_rules>;
    static_assert(std::is_same_v<test_unmerged_rules, unmerged_rules_result>,
                  "unmerged rules test");
    using test_lexer_rules = code_to_rules_t<lexer_rules>;
    static_assert(std::is_same_v<test_lexer_rules, lexer_rules_result>,
                  "lexer rules test");
    using test_calculator = code_to_rules_t<calculator>;
    static_assert(std::is_same_v<test_calculator, calculator_result>,
                  "calculator Rules test");
    using test_redundant = code_to_rules_t<redundant_rule>;
    static_assert(std::is_same_v<test_redundant, redundant_decl_merged>,
                  "merge rules with same identifier");

} // namespace pis

#endif
