// ParserInString -*- C++ -*-

// Copyright (C) 2018  Scott Brown

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

#ifndef PARSER_IN_STRING_MATCHER_TESTRULETOMAP
#define PARSER_IN_STRING_MATCHER_TESTRULETOMAP

#include "Common/Identity.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Rules/CodeToRules.hpp"
#include "TAVL.hpp"
#include <type_traits>
static constexpr const char single_rule_single_term[] = "abc = abc;";
using single_rule_single_term_result =
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    std::integer_sequence<char, 'a', 'b', 'c'>,
                    Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                        Compiler::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'a', 'b', 'c'>>>>>;
static constexpr const char single_rule_terms[] =
    R"(abc = abc, "1234", , ? special ?;)";
using single_rule_terms_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::empty_node,
    0,
    std::integer_sequence<char, 'a', 'b', 'c'>,
    Compiler::type_list<Compiler::type_list<
        Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                            std::integer_sequence<char, 'a', 'b', 'c'>>,
        Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                            std::integer_sequence<char, '1', '2', '3', '4'>>,
        Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                            std::integer_sequence<char>>,
        Compiler::type_pair<Compiler::Impl::special_sequence_flag,
                            std::integer_sequence<char,
                                                  ' ',
                                                  's',
                                                  'p',
                                                  'e',
                                                  'c',
                                                  'i',
                                                  'a',
                                                  'l',
                                                  ' '>>>>>;
static constexpr const char single_rule_option_group[] = R"(
abc = abc, [ def, "1234" /);)";
using single_rule_option_group_result                  = tavl::tavl_node<
    tavl::empty_node,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a', 'b', 'c'>,
        Compiler::type_list<Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'a', 'b', 'c'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '0'>>>>>,
    1,
    std::integer_sequence<char, '@', '0'>,
    Compiler::type_list<
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            Compiler::type_pair<
                Compiler::Impl::terminal_string_flag,
                std::integer_sequence<char, '1', '2', '3', '4'>>>,
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>;
static constexpr const char single_rule_repeat_group[] =
    R"(abc = abc, {def, "1234":);)";
using single_rule_repeat_group_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a', 'b', 'c'>,
        Compiler::type_list<Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'a', 'b', 'c'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '0'>>>>>,
    1,
    std::integer_sequence<char, '@', '0'>,
    Compiler::type_list<
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'd', 'e', 'f'>>,
            Compiler::type_pair<
                Compiler::Impl::terminal_string_flag,
                std::integer_sequence<char, '1', '2', '3', '4'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '0'>>>,
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>;
static constexpr const char single_rule_grouped_group[] =
    R"(abc = abc, (def, "12345");)";
using single_rule_grouped_group_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a', 'b', 'c'>,
        Compiler::type_list<Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'a', 'b', 'c'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '0'>>>>>,
    1,
    std::integer_sequence<char, '@', '0'>,
    Compiler::type_list<Compiler::type_list<
        Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                            std::integer_sequence<char, 'd', 'e', 'f'>>,
        Compiler::type_pair<
            Compiler::Impl::terminal_string_flag,
            std::integer_sequence<char, '1', '2', '3', '4', '5'>>>>>;
static constexpr const char single_rule_mixed[] =
    R"(abc = abc, ("123ab c", {?123?, [abc]});)";
using single_rule_mixed_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, '@', '0'>,
        Compiler::type_list<Compiler::type_list<
            Compiler::type_pair<
                Compiler::Impl::terminal_string_flag,
                std::integer_sequence<char, '1', '2', '3', 'a', 'b', ' ', 'c'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '1'>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'a', 'b', 'c'>,
            Compiler::type_list<Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'a', 'b', 'c'>>,
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '0'>>>>>,
        1,
        std::integer_sequence<char, '@', '2'>,
        Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                                Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'a', 'b', 'c'>>>,
                            Compiler::type_list<Compiler::type_pair<
                                Compiler::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>,
    2,
    std::integer_sequence<char, '@', '1'>,
    Compiler::type_list<
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::special_sequence_flag,
                                std::integer_sequence<char, '1', '2', '3'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '2'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '1'>>>,
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>;
static constexpr const char single_rule_multiple_definitions[] =
    R"(abc = def, ("123") | ?test?|;)";
using single_rule_multiple_definitions_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a', 'b', 'c'>,
        Compiler::type_list<
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'd', 'e', 'f'>>,
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '0'>>>,
            Compiler::type_list<Compiler::type_pair<
                Compiler::Impl::special_sequence_flag,
                std::integer_sequence<char, 't', 'e', 's', 't'>>>,
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                    std::integer_sequence<char>>>>>,
    1,
    std::integer_sequence<char, '@', '0'>,
    Compiler::type_list<Compiler::type_list<
        Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                            std::integer_sequence<char, '1', '2', '3'>>>>>;
static constexpr const char multiple_rules[] = R"(
abc = abc, def | def;
def = abc|;)";
using multiple_rules_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'a', 'b', 'c'>,
        Compiler::type_list<
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, 'a', 'b', 'c'>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'd', 'e', 'f'>>>,
            Compiler::type_list<Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'd', 'e', 'f'>>>>>,
    tavl::empty_node,
    1,
    std::integer_sequence<char, 'd', 'e', 'f'>,
    Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                            Compiler::Impl::meta_identifier_flag,
                            std::integer_sequence<char, 'a', 'b', 'c'>>>,
                        Compiler::type_list<Compiler::type_pair<
                            Compiler::Impl::empty_sequence_flag,
                            std::integer_sequence<char>>>>>;
static constexpr const char groups_in_different_rules[] = R"(
R1 = "abc", {"123", R1}|["def"];
R2 = [(def), R2];)";
using groups_in_different_rules_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, '@', '0'>,
            Compiler::type_list<
                Compiler::type_list<
                    Compiler::type_pair<
                        Compiler::Impl::terminal_string_flag,
                        std::integer_sequence<char, '1', '2', '3'>>,
                    Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                        std::integer_sequence<char, 'R', '1'>>,
                    Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                        std::integer_sequence<char, '@', '0'>>>,
                Compiler::type_list<
                    Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                        std::integer_sequence<char>>>>>,
        tavl::empty_node,
        1,
        std::integer_sequence<char, '@', '1'>,
        Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                                Compiler::Impl::terminal_string_flag,
                                std::integer_sequence<char, 'd', 'e', 'f'>>>,
                            Compiler::type_list<Compiler::type_pair<
                                Compiler::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, '@', '3'>,
            Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'd', 'e', 'f'>>>>>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'R', '2'>,
            Compiler::type_list<Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '2'>>>>>,
        1,
        std::integer_sequence<char, 'R', '1'>,
        Compiler::type_list<
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, 'a', 'b', 'c'>>,
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '0'>>>,
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '1'>>>>>,
    2,
    std::integer_sequence<char, '@', '2'>,
    Compiler::type_list<
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '3'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'R', '2'>>>,
        Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                std::integer_sequence<char>>>>>;
static constexpr const char recursive_same_groups[] =
    R"(R = ((((("123", R)))));)";
using recursive_same_groups_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, '@', '0'>,
            Compiler::type_list<Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '1'>>>>>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, '@', '2'>,
            Compiler::type_list<Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '3'>>>>>,
        1,
        std::integer_sequence<char, '@', '1'>,
        Compiler::type_list<Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, '@', '2'>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'R'>,
            Compiler::type_list<Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                    std::integer_sequence<char, '@', '0'>>>>>,
        1,
        std::integer_sequence<char, '@', '4'>,
        Compiler::type_list<Compiler::type_list<
            Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                std::integer_sequence<char, '1', '2', '3'>>,
            Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                std::integer_sequence<char, 'R'>>>>>,
    2,
    std::integer_sequence<char, '@', '3'>,
    Compiler::type_list<Compiler::type_list<
        Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                            std::integer_sequence<char, '@', '4'>>>>>;
namespace Compiler
{
    namespace InHeaderDebug
    {
        using test_single_rule_single_term =
            rule_to_map_t<single_rule_single_term>;
        static_assert(std::is_same_v<test_single_rule_single_term,
                                     single_rule_single_term_result>,
                      "single rule with single term test");
        using test_single_rule_terms = rule_to_map_t<single_rule_terms>;
        static_assert(
            std::is_same_v<test_single_rule_terms, single_rule_terms_result>,
            "single rule with multiple terms test");
        using test_single_rule_option_group =
            rule_to_map_t<single_rule_option_group>;
        static_assert(std::is_same_v<test_single_rule_option_group,
                                     single_rule_option_group_result>,
                      "single rule with optional group test");
        using test_single_rule_repeat_group =
            rule_to_map_t<single_rule_repeat_group>;
        static_assert(std::is_same_v<test_single_rule_repeat_group,
                                     single_rule_repeat_group_result>,
                      "single rule with repeated group test");
        using test_single_rule_grouped_group =
            rule_to_map_t<single_rule_grouped_group>;
        static_assert(std::is_same_v<test_single_rule_grouped_group,
                                     single_rule_grouped_group_result>,
                      "single rule with grouped group test");
        using test_single_rule_mixed = rule_to_map_t<single_rule_mixed>;
        static_assert(
            std::is_same_v<test_single_rule_mixed, single_rule_mixed_result>,
            "single rule with mixed groups test");
        using test_single_rule_multiple_definitions =
            rule_to_map_t<single_rule_multiple_definitions>;
        static_assert(std::is_same_v<test_single_rule_multiple_definitions,
                                     single_rule_multiple_definitions_result>,
                      "single rule with multiple definitions test");
        using test_multiple_rules = rule_to_map_t<::multiple_rules>;
        static_assert(
            std::is_same_v<test_multiple_rules, multiple_rules_result>,
            "multiple rulse test");
        using test_groups_in_different_rules =
            rule_to_map_t<groups_in_different_rules>;
        static_assert(std::is_same_v<test_groups_in_different_rules,
                                     groups_in_different_rules_result>,
                      "groups in different groups test");
        using test_recursive_same_groups = rule_to_map_t<recursive_same_groups>;
        static_assert(std::is_same_v<test_recursive_same_groups,
                                     recursive_same_groups_result>,
                      "recursively defined same-type groups test");
        using test_undefined_checker_success =
            undefined_checker_t<single_rule_single_term_result>;
        static_assert(std::is_same_v<test_undefined_checker_success,
                                     single_rule_single_term_result>,
                      "undefined_checker tests");
        // expected failure
        // using test_undefined_checker_fail =
        //     undefined_checker_t<single_rule_option_group_result>;
    } // namespace InHeaderDebug
} // namespace Compiler

#endif
