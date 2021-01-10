// ParserInString -*- C++ -*-

// Copyright (C) 2019-2020 Scott Brown

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

#ifndef PARSER_IN_STRING_TEST_TESTLEXER
#define PARSER_IN_STRING_TEST_TESTLEXER

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Lexer/Lexer.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include "test_common.hpp"
#include <any>
#include <doctest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

DECL_RULE(no_lexer, "A = ;");
DECL_RULE(tstr_only, R"(A = "a", "abc" | "abd", "b";)");

DECL_RULE(identifier, R"(A = ?lexer a?, ("_"|?alpha?), {?alnum?};)");
using identifier_rule_result =
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    std::integer_sequence<char, 'A'>,
                    Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                        Compiler::Impl::terminal_string_flag,
                        std::integer_sequence<char,
                                              '\'',
                                              '\"',
                                              'l',
                                              'e',
                                              'x',
                                              'e',
                                              'r',
                                              ' ',
                                              'a'>>>>>;
DECL_RULE(optional, R"(
A = ?normal special?;
B = "abc" | ?lexer test?, ["abc"];
)");
using optional_rule_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'B'>,
        Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                                Compiler::Impl::terminal_string_flag,
                                std::integer_sequence<char, 'a', 'b', 'c'>>>,
                            Compiler::type_list<Compiler::type_pair<
                                Compiler::Impl::terminal_string_flag,
                                std::integer_sequence<char,
                                                      '\'',
                                                      '\"',
                                                      'l',
                                                      'e',
                                                      'x',
                                                      'e',
                                                      'r',
                                                      ' ',
                                                      't',
                                                      'e',
                                                      's',
                                                      't'>>>>>,
    1,
    std::integer_sequence<char, 'A'>,
    Compiler::type_list<Compiler::type_list<
        Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                            std::integer_sequence<char,
                                                  '\'',
                                                  '\"',
                                                  'n',
                                                  'o',
                                                  'r',
                                                  'm',
                                                  'a',
                                                  'l',
                                                  ' ',
                                                  's',
                                                  'p',
                                                  'e',
                                                  'c',
                                                  'i',
                                                  'a',
                                                  'l'>>>>>;
DECL_RULE(term_non, R"(A = "abc" | ?lexer a?, {?alpha?};)");
using term_non_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::empty_node,
    0,
    std::integer_sequence<char, 'A'>,
    Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                            Compiler::Impl::terminal_string_flag,
                            std::integer_sequence<char, 'a', 'b', 'c'>>>,
                        Compiler::type_list<Compiler::type_pair<
                            Compiler::Impl::terminal_string_flag,
                            std::integer_sequence<char,
                                                  '\'',
                                                  '\"',
                                                  'l',
                                                  'e',
                                                  'x',
                                                  'e',
                                                  'r',
                                                  ' ',
                                                  'a'>>>>>;
DECL_RULE(number, R"(
Expr = Multi, Expr1;
Expr1 = '+', Multi, Expr1 |
        '-', Multi, Expr1 |
        ;
Multi = Const, Multi1;
Multi1 = '*', Const, Multi1 |
         '/', Const, Multi1 |
         ;
Const = ?lexer i?, {?number?} |
        '(', Expr, ')';
)");
using number_result = tavl::tavl_node<
    tavl::tavl_node<
        tavl::empty_node,
        tavl::empty_node,
        0,
        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>,
        Compiler::type_list<
            Compiler::type_list<Compiler::type_pair<
                Compiler::Impl::terminal_string_flag,
                std::integer_sequence<char,
                                      '\'',
                                      '\"',
                                      'l',
                                      'e',
                                      'x',
                                      'e',
                                      'r',
                                      ' ',
                                      'i'>>>,
            Compiler::type_list<
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, '('>>,
                Compiler::type_pair<
                    Compiler::Impl::meta_identifier_flag,
                    std::integer_sequence<char, 'E', 'x', 'p', 'r'>>,
                Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                    std::integer_sequence<char, ')'>>>>>,
    tavl::tavl_node<
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'E', 'x', 'p', 'r', '1'>,
            Compiler::type_list<
                Compiler::type_list<
                    Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                        std::integer_sequence<char, '+'>>,
                    Compiler::type_pair<
                        Compiler::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>,
                    Compiler::type_pair<
                        Compiler::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'E', 'x', 'p', 'r', '1'>>>,
                Compiler::type_list<
                    Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                        std::integer_sequence<char, '-'>>,
                    Compiler::type_pair<
                        Compiler::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>,
                    Compiler::type_pair<
                        Compiler::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'E', 'x', 'p', 'r', '1'>>>,
                Compiler::type_list<
                    Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                        std::integer_sequence<char>>>>>,
        tavl::tavl_node<
            tavl::empty_node,
            tavl::empty_node,
            0,
            std::integer_sequence<char, 'M', 'u', 'l', 't', 'i', '1'>,
            Compiler::type_list<
                Compiler::type_list<
                    Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                        std::integer_sequence<char, '*'>>,
                    Compiler::type_pair<
                        Compiler::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>>,
                    Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                        std::integer_sequence<char,
                                                              'M',
                                                              'u',
                                                              'l',
                                                              't',
                                                              'i',
                                                              '1'>>>,
                Compiler::type_list<
                    Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                                        std::integer_sequence<char, '/'>>,
                    Compiler::type_pair<
                        Compiler::Impl::meta_identifier_flag,
                        std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>>,
                    Compiler::type_pair<Compiler::Impl::meta_identifier_flag,
                                        std::integer_sequence<char,
                                                              'M',
                                                              'u',
                                                              'l',
                                                              't',
                                                              'i',
                                                              '1'>>>,
                Compiler::type_list<
                    Compiler::type_pair<Compiler::Impl::empty_sequence_flag,
                                        std::integer_sequence<char>>>>>,
        1,
        std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>,
        Compiler::type_list<Compiler::type_list<
            Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'C', 'o', 'n', 's', 't'>>,
            Compiler::type_pair<
                Compiler::Impl::meta_identifier_flag,
                std::integer_sequence<char, 'M', 'u', 'l', 't', 'i', '1'>>>>>,
    2,
    std::integer_sequence<char, 'E', 'x', 'p', 'r'>,
    Compiler::type_list<Compiler::type_list<
        Compiler::type_pair<
            Compiler::Impl::meta_identifier_flag,
            std::integer_sequence<char, 'M', 'u', 'l', 't', 'i'>>,
        Compiler::type_pair<
            Compiler::Impl::meta_identifier_flag,
            std::integer_sequence<char, 'E', 'x', 'p', 'r', '1'>>>>>;
DECL_RULE(special_sequence, R"(A = ?special 1?, ?special 2?;)");
using special_sequence_result = tavl::tavl_node<
    tavl::empty_node,
    tavl::empty_node,
    0,
    std::integer_sequence<char, 'A'>,
    Compiler::type_list<Compiler::type_list<
        Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                            std::integer_sequence<char,
                                                  '\'',
                                                  '\"',
                                                  's',
                                                  'p',
                                                  'e',
                                                  'c',
                                                  'i',
                                                  'a',
                                                  'l',
                                                  ' ',
                                                  '1'>>,
        Compiler::type_pair<Compiler::Impl::terminal_string_flag,
                            std::integer_sequence<char,
                                                  '\'',
                                                  '\"',
                                                  's',
                                                  'p',
                                                  'e',
                                                  'c',
                                                  'i',
                                                  'a',
                                                  'l',
                                                  ' ',
                                                  '2'>>>>>;
DECL_RULE(regex, R"(
A = ?lexer regex test?, ?@ab[cd1-9\]]*(\def)*?;)");
using regex_result =
    tavl::tavl_node<tavl::empty_node,
                    tavl::empty_node,
                    0,
                    std::integer_sequence<char, 'A'>,
                    Compiler::type_list<Compiler::type_list<Compiler::type_pair<
                        Compiler::Impl::terminal_string_flag,
                        std::integer_sequence<char,
                                              '\'',
                                              '\"',
                                              'l',
                                              'e',
                                              'x',
                                              'e',
                                              'r',
                                              ' ',
                                              'r',
                                              'e',
                                              'g',
                                              'e',
                                              'x',
                                              ' ',
                                              't',
                                              'e',
                                              's',
                                              't'>>>>>;
namespace Compiler
{
    using test_no_lexer = lexer<code_to_rules_t<no_lexer_rule>>;
    static_assert(tavl::is_empty_node_v<typename test_no_lexer::dfa>);
    using test_tstr_only = lexer<code_to_rules_t<tstr_only_rule>>;
    static_assert(tavl::tavl_is_same_v<typename test_tstr_only::type,
                                       code_to_rules_t<tstr_only_rule>>);

    using test_identifier = lexer<code_to_rules_t<identifier_rule>>;
    static_assert(!tavl::is_empty_node_v<typename test_identifier::dfa>);
    static_assert(tavl::tavl_is_same_v<typename test_identifier::type,
                                       identifier_rule_result>);
    using test_optional = lexer<code_to_rules_t<optional_rule>>;
    static_assert(!tavl::is_empty_node_v<typename test_optional::dfa>);
    static_assert(tavl::tavl_is_same_v<typename test_optional::type,
                                       optional_rule_result>);
    using test_term_nont = lexer<code_to_rules_t<term_non_rule>>;
    static_assert(!tavl::is_empty_node_v<typename test_term_nont::dfa>);
    static_assert(
        tavl::tavl_is_same_v<typename test_term_nont::type, term_non_result>);
    using test_number = lexer<code_to_rules_t<number_rule>>;
    static_assert(!tavl::is_empty_node_v<typename test_number::dfa>);
    static_assert(
        tavl::tavl_is_same_v<typename test_number::type, number_result>);
    using test_special = lexer<code_to_rules_t<special_sequence_rule>>;
    static_assert(tavl::tavl_is_same_v<typename test_special::type,
                                       special_sequence_result>);
    using test_regex = lexer<code_to_rules_t<regex_rule>>;
    static_assert(
        tavl::tavl_is_same_v<typename test_regex::type, regex_result>);
    // typename test_regex::type i = 0;
    std::vector<std::any>& parser_dummy()
    {
        static std::vector<std::any> dummy;
        return dummy;
    }
} // namespace Compiler
TEST_SUITE("Lexer")
{
    std::unordered_map<std::string, Compiler::lexer_listener_t> dummy_listeners;
    TEST_CASE("number")
    {
        std::cout
            << rtrim(Compiler::lexer<Compiler::code_to_rules_t<number_rule>>{
                   std::cin, Compiler::parser_dummy, dummy_listeners}
                         .dump())
            << std::endl;
    }
    TEST_CASE("tstr_only")
    {
        auto           tstr_dump = rtrim(Compiler::test_tstr_only{
            std::cin, Compiler::parser_dummy, dummy_listeners}
                                   .dump());
        constexpr auto tstr_dump_result =
            R"EOF({0 1 3 7 11}
	a
		{[2] 4 8}
	b
		{[12]}
{[2] 4 8}
	b
		{5 9}
{5 9}
	c
		{[6]}
	d
		{[10]}

Finite: 
{[2] 4 8 } (a)
{[6] } (abc)
{[10] } (abd)
{[12] } (b))EOF";
        CHECK_EQ(tstr_dump, tstr_dump_result);
    }
    TEST_CASE("identifier")
    {
        auto           identifier_dump = rtrim(Compiler::test_identifier{
            std::cin, Compiler::parser_dummy, dummy_listeners}
                                         .dump());
        constexpr auto identifier_result =
#include "test_lexer_identifier.str"
            ;
        CHECK_EQ(identifier_dump, identifier_result);
    }
    TEST_CASE("optional")
    {
        auto              optional_dump = rtrim(Compiler::test_optional{
            std::cin, Compiler::parser_dummy, dummy_listeners}
                                       .dump());
        const std::string optional_result =
            R"({0 1 5 [9]}
	a
		{2 6}
{2 6}
	b
		{3 7}
{3 7}
	c
		{[4] 8 [9]}

Finite: 
{0 1 5 [9] } (lexer test)
{[4] 8 [9] } (abc|lexer test))";
        CHECK_EQ(optional_dump, optional_result);
    }
    TEST_CASE("Terminal & Nonterminal")
    {
        auto              term_nont_dump = rtrim(Compiler::test_term_nont{
            std::cin, Compiler::parser_dummy, dummy_listeners}
                                        .dump());
        const std::string term_nont_result =
#include "test_lexer_term_nont_str.str"
            ;
        CHECK_EQ(term_nont_dump, term_nont_result);
    }
    TEST_CASE("readNext")
    {
        std::string              test_input = "aabdabcb";
        std::stringstream        ss_input{test_input};
        Compiler::test_tstr_only test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a'>>(), "a");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'd'>>(),
                 "abd");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'c'>>(),
                 "abc");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'b'>>(), "b");
    }
    TEST_CASE("read fail in the middle")
    {
        std::string              test_input = "aaba";
        std::stringstream        ss_input{test_input};
        Compiler::test_tstr_only test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a'>>(), "a");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'd'>>(), "");
    }
    TEST_CASE("readNext: marker not exist")
    {
        std::string              test_input = "aaba";
        std::stringstream        ss_input{test_input};
        Compiler::test_tstr_only test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'c'>>(), "");
    }
    TEST_CASE("readNext: read EOF")
    {
        std::string              test_input = "aab";
        std::stringstream        ss_input{test_input};
        Compiler::test_tstr_only test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a'>>(), "a");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'd'>>(), "");
    }
    TEST_CASE("readNext: lexer rule")
    {
        std::string               test_input = "aab.";
        std::stringstream         ss_input{test_input};
        Compiler::test_identifier test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(
            test_lexer.readNext<
                Compiler::
                    t_string<'\'', '\"', 'l', 'e', 'x', 'e', 'r', ' ', 'a'>>(),
            "aab");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'd'>>(), "");
    }
    TEST_CASE("readNext: lexer rule not exist")
    {
        std::string               test_input = "aab.";
        std::stringstream         ss_input{test_input};
        Compiler::test_identifier test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(
            test_lexer.readNext<
                Compiler::
                    t_string<'\'', '\"', 'l', 'e', 'x', 'e', 'r', ' ', 'v'>>(),
            "");
    }
    TEST_CASE("readNext: empty input")
    {
        std::string             test_input = "";
        std::stringstream       ss_input{test_input};
        Compiler::test_optional test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "");
    }
    TEST_CASE("readNext: optional")
    {
        std::string             test_input = "abcabc";
        std::stringstream       ss_input{test_input};
        Compiler::test_optional test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        std::cout << test_lexer.dump() << std::endl;
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "abc");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'c'>>(),
                 "abc");
    }
    TEST_CASE("readNext: Terminal & Nonterminal")
    {
        std::string              test_input = "abc abcd";
        std::stringstream        ss_input{test_input};
        Compiler::test_term_nont test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'c'>>(),
                 "abc");
        CHECK_EQ(
            test_lexer.readNext<
                Compiler::
                    t_string<'\'', '\"', 'l', 'e', 'x', 'e', 'r', ' ', 'a'>>(),
            "abcd");
        std::string              test_input_f = "abcd";
        std::stringstream        ss_input_f{test_input_f};
        Compiler::test_term_nont test_lexer_f{
            ss_input_f, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'a', 'b', 'c'>>(), "");
    }
    TEST_CASE("Bug: missing empty_trans when starting repeated sequence")
    {
        std::string           test_input = "12+2";
        std::stringstream     ss_input{test_input};
        Compiler::test_number test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        CHECK_EQ(
            test_lexer.readNext<
                Compiler::
                    t_string<'\'', '\"', 'l', 'e', 'x', 'e', 'r', ' ', 'i'>>(),
            "12");
    }
    TEST_CASE("special sequence")
    {
        using namespace std::string_literals;
        static std::vector<std::any> dummy;
        auto parser_dummy = []() -> std::vector<std::any>& {
            return dummy;
        };
        std::string       test_input = "test input";
        std::stringstream ss_input{test_input};
        std::unordered_map<std::string, Compiler::lexer_listener_t> listeners;
        Compiler::test_special test_lexer{ss_input, parser_dummy, listeners};
        listeners.emplace("special 1"s,
                          [&](std::vector<std::any>&     tokens,
                              Compiler::lexer_interface& lexer,
                              const std::string&         target) {
                              CHECK_EQ(target, "special 1");
                              CHECK(tokens.empty());
                              CHECK_EQ(&lexer, &test_lexer);
                              return "1"s;
                          });
        listeners.emplace("special 2",
                          [&](std::vector<std::any>&     tokens,
                              Compiler::lexer_interface& lexer,
                              const std::string&         target) {
                              CHECK_EQ(target, "special 2");
                              CHECK(tokens.empty());
                              CHECK_EQ(&lexer, &test_lexer);
                              return "2"s;
                          });
        CHECK_EQ(test_lexer.template readNext<Compiler::t_string<'\'',
                                                                 '\"',
                                                                 's',
                                                                 'p',
                                                                 'e',
                                                                 'c',
                                                                 'i',
                                                                 'a',
                                                                 'l',
                                                                 ' ',
                                                                 '1'>>(),
                 "1");
        CHECK_EQ(test_lexer.template readNext<Compiler::t_string<'\'',
                                                                 '\"',
                                                                 's',
                                                                 'p',
                                                                 'e',
                                                                 'c',
                                                                 'i',
                                                                 'a',
                                                                 'l',
                                                                 ' ',
                                                                 '2'>>(),
                 "2");
        dummy.emplace_back(0);
        listeners.erase("special 2");
        listeners.emplace("special 2",
                          [&](std::vector<std::any>&     tokens,
                              Compiler::lexer_interface& lexer,
                              const std::string&         target) {
                              CHECK_EQ(target, "special 2");
                              CHECK_EQ(tokens.size(), 1);
                              CHECK_EQ(&lexer, &test_lexer);
                              tokens.emplace_back("test"s);
                              return "3"s;
                          });
        CHECK_EQ(test_lexer.template readNext<Compiler::t_string<'\'',
                                                                 '\"',
                                                                 's',
                                                                 'p',
                                                                 'e',
                                                                 'c',
                                                                 'i',
                                                                 'a',
                                                                 'l',
                                                                 ' ',
                                                                 '2'>>(),
                 "3");
        CHECK_EQ(dummy.size(), 2);
        CHECK_EQ(std::any_cast<int>(dummy[0]), 0);
        CHECK_EQ(std::any_cast<std::string>(dummy[1]), "test");
    }
    TEST_CASE("regex in special sequence")
    {
        // ab[cd1-9\]]*(def)*
        std::string test_input = "ab1 abccdef abdef ab9def ab123c]defdef abde";
        std::stringstream    ss_input{test_input};
        Compiler::test_regex test_lexer{
            ss_input, Compiler::parser_dummy, dummy_listeners};
        constexpr auto regex_result =
#include "test_lexer_regex.str"
            ;
        auto regex_dump = rtrim(test_lexer.dump());
        CHECK_EQ(regex_dump, regex_result);
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        'r',
                                                        'e',
                                                        'g',
                                                        'e',
                                                        'x',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "ab1");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        'r',
                                                        'e',
                                                        'g',
                                                        'e',
                                                        'x',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "abccdef");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        'r',
                                                        'e',
                                                        'g',
                                                        'e',
                                                        'x',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "abdef");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        'r',
                                                        'e',
                                                        'g',
                                                        'e',
                                                        'x',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "ab9def");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        'r',
                                                        'e',
                                                        'g',
                                                        'e',
                                                        'x',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "ab123c]defdef");
        CHECK_EQ(test_lexer.readNext<Compiler::t_string<'\'',
                                                        '\"',
                                                        'l',
                                                        'e',
                                                        'x',
                                                        'e',
                                                        'r',
                                                        ' ',
                                                        'r',
                                                        'e',
                                                        'g',
                                                        'e',
                                                        'x',
                                                        ' ',
                                                        't',
                                                        'e',
                                                        's',
                                                        't'>>(),
                 "abd");
    }
}
#endif
