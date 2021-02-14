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

#ifndef PARSER_IN_STRING_MATCHER_TESTGENERATEDRULEMERGER
#define PARSER_IN_STRING_MATCHER_TESTGENERATEDRULEMERGER

#include "TAVL.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include <type_traits>
static constexpr const char no_generated_rule[] = "abc = abc;";
using no_generated_rule_result =
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    std::integer_sequence<char, 'a', 'b', 'c'>,
                    pis::type_list<pis::type_list<pis::type_pair<
                        pis::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'a', 'b', 'c'>>>>>;
static constexpr const char merge_one[] = "abc = [abc]; def = [abc];";
using merge_one_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, '@', '0'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                               std::integer_sequence<char, 'a', 'b', 'c'>>>,
            pis::type_list<pis::type_pair<pis::Impl::empty_sequence_flag,
                                          std::integer_sequence<char>>>>>,
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    std::integer_sequence<char, 'd', 'e', 'f'>,
                    pis::type_list<pis::type_list<pis::type_pair<
                        pis::Impl::meta_identifier_flag,
                        std::integer_sequence<char, '@', '0'>>>>>,
    1,
    std::integer_sequence<char, 'a', 'b', 'c'>,
    pis::type_list<
        pis::type_list<pis::type_pair<pis::Impl::meta_identifier_flag,
                                      std::integer_sequence<char, '@', '0'>>>>>;
static constexpr const char generated_but_no_merge[] = "abc = [abc];";
using generated_but_no_merge_result                  = tavl::tavl_node<
    tavl::empty_node,
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    std::integer_sequence<char, 'a', 'b', 'c'>,
                    pis::type_list<pis::type_list<pis::type_pair<
                        pis::Impl::meta_identifier_flag,
                        std::integer_sequence<char, '@', '0'>>>>>,
    1,
    std::integer_sequence<char, '@', '0'>,
    pis::type_list<
        pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                           std::integer_sequence<char, 'a', 'b', 'c'>>>,
        pis::type_list<pis::type_pair<pis::Impl::empty_sequence_flag,
                                      std::integer_sequence<char>>>>>;
static constexpr const char multiple_merging[] = R"(
abc = [abc] | "123";
def = [abc];
ghi = [abc];)";
using multiple_merging_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, '@', '0'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                               std::integer_sequence<char, 'a', 'b', 'c'>>>,
            pis::type_list<pis::type_pair<pis::Impl::empty_sequence_flag,
                                          std::integer_sequence<char>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        std::integer_sequence<char, 'g', 'h', 'i'>,
                        pis::type_list<pis::type_list<pis::type_pair<
                            pis::Impl::meta_identifier_flag,
                            std::integer_sequence<char, '@', '0'>>>>>,
        1,
        std::integer_sequence<char, 'd', 'e', 'f'>,
        pis::type_list<pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                           std::integer_sequence<char, '@', '0'>>>>>,
    2,
    std::integer_sequence<char, 'a', 'b', 'c'>,
    pis::type_list<
        pis::type_list<pis::type_pair<pis::Impl::meta_identifier_flag,
                                      std::integer_sequence<char, '@', '0'>>>,
        pis::type_list<
            pis::type_pair<pis::Impl::terminal_string_flag,
                           std::integer_sequence<char, '1', '2', '3'>>>>>;
static constexpr const char merge_and_not[] =
    "abc = [abc] | (def);def = [abc];";
using merge_and_not_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, '@', '0'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                               std::integer_sequence<char, 'a', 'b', 'c'>>>,
            pis::type_list<pis::type_pair<pis::Impl::empty_sequence_flag,
                                          std::integer_sequence<char>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        std::integer_sequence<char, 'd', 'e', 'f'>,
                        pis::type_list<pis::type_list<pis::type_pair<
                            pis::Impl::meta_identifier_flag,
                            std::integer_sequence<char, '@', '0'>>>>>,
        1,
        std::integer_sequence<char, 'a', 'b', 'c'>,
        pis::type_list<pis::type_list<pis::type_pair<
                           pis::Impl::meta_identifier_flag,
                           std::integer_sequence<char, '@', '0'>>>,
                       pis::type_list<pis::type_pair<
                           pis::Impl::meta_identifier_flag,
                           std::integer_sequence<char, '@', '1'>>>>>,
    2,
    std::integer_sequence<char, '@', '1'>,
    pis::type_list<pis::type_list<
        pis::type_pair<pis::Impl::meta_identifier_flag,
                       std::integer_sequence<char, 'd', 'e', 'f'>>>>>;
static constexpr const char recursive_merge[] = R"(
R1 = [("abc")] | "abc";
R2 = ("abc");
R3 = [("abc")];)";
using recursive_merge_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, '@', '0'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::meta_identifier_flag,
                               std::integer_sequence<char, '@', '1'>>>,
            pis::type_list<pis::type_pair<pis::Impl::empty_sequence_flag,
                                          std::integer_sequence<char>>>>>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'R', '1'>,
            pis::type_list<pis::type_list<pis::type_pair<
                               pis::Impl::meta_identifier_flag,
                               std::integer_sequence<char, '@', '0'>>>,
                           pis::type_list<pis::type_pair<
                               pis::Impl::terminal_string_flag,
                               std::integer_sequence<char, 'a', 'b', 'c'>>>>>,
        tavl::tavl_node<tavl::empty_node,
                        tavl::empty_node,
                        0,
                        std::integer_sequence<char, 'R', '3'>,
                        pis::type_list<pis::type_list<pis::type_pair<
                            pis::Impl::meta_identifier_flag,
                            std::integer_sequence<char, '@', '0'>>>>>,
        1,
        std::integer_sequence<char, 'R', '2'>,
        pis::type_list<pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                           std::integer_sequence<char, '@', '1'>>>>>,
    2,
    std::integer_sequence<char, '@', '1'>,
    pis::type_list<pis::type_list<
        pis::type_pair<pis::Impl::terminal_string_flag,
                       std::integer_sequence<char, 'a', 'b', 'c'>>>>>;
static constexpr const char should_not_merge[] = R"(
abc = ["123", abc] | abc;
def = "123", abc | ;)";
using should_not_merge_result                  = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, '@', '0'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::terminal_string_flag,
                               std::integer_sequence<char, '1', '2', '3'>>,
                pis::type_pair<pis::Impl::meta_identifier_flag,
                               std::integer_sequence<char, 'a', 'b', 'c'>>>,
            pis::type_list<pis::type_pair<pis::Impl::empty_sequence_flag,
                                          std::integer_sequence<char>>>>>,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'd', 'e', 'f'>,
        pis::type_list<
            pis::type_list<
                pis::type_pair<pis::Impl::terminal_string_flag,
                               std::integer_sequence<char, '1', '2', '3'>>,
                pis::type_pair<pis::Impl::meta_identifier_flag,
                               std::integer_sequence<char, 'a', 'b', 'c'>>>,
            pis::type_list<pis::type_pair<pis::Impl::empty_sequence_flag,
                                          std::integer_sequence<char>>>>>,
    1,
    std::integer_sequence<char, 'a', 'b', 'c'>,
    pis::type_list<
        pis::type_list<pis::type_pair<pis::Impl::meta_identifier_flag,
                                      std::integer_sequence<char, '@', '0'>>>,
        pis::type_list<
            pis::type_pair<pis::Impl::meta_identifier_flag,
                           std::integer_sequence<char, 'a', 'b', 'c'>>>>>;
namespace pis
{
    using test_no_generated_rule =
        generated_rule_merger_t<rule_to_map_t<no_generated_rule>>;
    static_assert(
        std::is_same_v<test_no_generated_rule, no_generated_rule_result>,
        "no generated rules test");
    // using test_merge_one = generated_rule_merger_t<rule_to_map_t<merge_one>>;
    // static_assert(std::is_same_v<test_merge_one, merge_one_result>,
    //               "merge one rule test");
    using test_generated_but_no_merge =
        generated_rule_merger_t<rule_to_map_t<generated_but_no_merge>>;
    static_assert(std::is_same_v<test_generated_but_no_merge,
                                 generated_but_no_merge_result>,
                  "generated but no merging test");
    // using test_multiple_merging =
    //     generated_rule_merger_t<rule_to_map_t<multiple_merging>>;
    // static_assert(
    //     std::is_same_v<test_multiple_merging, multiple_merging_result>,
    //     "multiple merging test");
    // using test_merge_and_not =
    //     generated_rule_merger_t<rule_to_map_t<merge_and_not>>;
    // static_assert(std::is_same_v<test_merge_and_not, merge_and_not_result>,
    //               "merged rules and cannot-merge rules test");
    // using test_recursive_merge =
    //     generated_rule_merger_t<rule_to_map_t<recursive_merge>>;
    // static_assert(std::is_same_v<test_recursive_merge,
    // recursive_merge_result>,
    //               "recursively merged rules test");
    using test_should_not_merge =
        generated_rule_merger_t<rule_to_map_t<should_not_merge>>;
    static_assert(
        std::is_same_v<test_should_not_merge, should_not_merge_result>,
        "rules that should not be merged test");
} // namespace pis
#endif
