// ParserInString -*- C++ -*-

// Copyright (C) 2019  Scott Brown

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

#ifndef PARSER_IN_STRING_TESTLL
#define PARSER_IN_STRING_TESTLL

#include "TAVL.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tfuncs.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Matcher/LL.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include "test_common.hpp"
#include <any>
#include <doctest.h>
#include <sstream>
#include <string>
#include <type_traits>

static constexpr const char example_rules[] =
    R"(S = 'a', A, 'a', 'a' | 'b', A, 'b', 'a'; A = 'b' | ;)";
static constexpr const char calculator_no_left_recursion[] = R"(
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
)";

namespace pis
{
    namespace LLTesting
    {
        // not LL(1)
        // using test_example_rules_1 =
        //     ll_parsing<code_to_rules_t<example_rules>, t_string<'S'>, 1>;
        using test_example_rules_2 =

            ll_parsing<code_to_rules_t<example_rules>, t_string<'S'>, 2>;
        using test_calculator =
            ll_parsing<code_to_rules_t<calculator_no_left_recursion>,
                       _TSTR("Expr"),
                       1>;

        // typename test_calculator::parsing_table i = 0;
    } // namespace LLTesting

} // namespace pis
TEST_SUITE("LL(k)")
{
    using namespace pis;
    TEST_CASE("k=2, r1")
    {
        std::string                                   test_input = "abaa";
        std::stringstream                             ss_input{test_input};
        typename pis::LLTesting::test_example_rules_2 parser;
        auto           parse_result  = parser.parse(ss_input);
        constexpr auto expect_result = R"([a, 
[b], 
a, 
a])";
        CHECK_EQ(print_any(parse_result), expect_result);
    }
    TEST_CASE("k=2, r2e")
    {
        std::string                                   test_input = "bba";
        std::stringstream                             ss_input{test_input};
        typename pis::LLTesting::test_example_rules_2 parser;
        auto           parse_result  = parser.parse(ss_input);
        constexpr auto expect_result = R"([b, 
[], 
b, 
a])";
        CHECK_EQ(print_any(parse_result), expect_result);
    }
    TEST_CASE("k=2, r2")
    {
        std::string                                   test_input = "bbba";
        std::stringstream                             ss_input{test_input};
        typename pis::LLTesting::test_example_rules_2 parser;
        auto           parse_result  = parser.parse(ss_input);
        constexpr auto expect_result = R"([b, 
[b], 
b, 
a])";
        CHECK_EQ(print_any(parse_result), expect_result);
    }
    TEST_CASE("k=2 r1e")
    {
        std::string                                   test_input = "aaa";
        std::stringstream                             ss_input{test_input};
        typename pis::LLTesting::test_example_rules_2 parser;
        auto           parse_result  = parser.parse(ss_input);
        constexpr auto expect_result = R"([a, 
[], 
a, 
a])";
        CHECK_EQ(print_any(parse_result), expect_result);
    }
    TEST_CASE("calculator")
    {
        std::string                              test_input = "1+2-3*4/5+(6)";
        std::stringstream                        ss_input{test_input};
        typename pis::LLTesting::test_calculator parser;

        auto           parse_result  = parser.parse(ss_input);
        constexpr auto expect_result = R"([[[1], 
[]], 
[+, 
[[2], 
[]], 
[-, 
[[3], 
[*, 
[4], 
[/, 
[5], 
[]]]], 
[+, 
[[(, 
[[[6], 
[]], 
[]], 
)], 
[]], 
[]]]]])";
        CHECK_EQ(print_any(parse_result), expect_result);
    }
    TEST_CASE("calculator with listener")
    {
        auto const_listener = [](std::vector<std::any> values,
                                 lexer_interface&,
                                 const std::string&) {
            if (values.size() == 1)
            {
                CHECK_EQ(values[0].type(), typeid(std::string));
                return std::make_any<int>(
                    std::stoi(std::any_cast<std::string>(values[0])));
            }
            else
            {
                CHECK_EQ(values[1].type(), typeid(int));
                return values[1];
            }
        };
        auto multi_listener = [](std::vector<std::any> values,
                                 lexer_interface&,
                                 const std::string&) {
            CHECK_EQ(values[0].type(), typeid(int));
            auto val = std::any_cast<int>(values[0]);
            if (values[1].has_value())
            {
                CHECK_EQ(values[1].type(), typeid(std::vector<std::any>));
                auto multis = std::any_cast<std::vector<std::any>>(values[1]);
                while (!multis.empty())
                {
                    if (multis.size() == 1)
                    {
                        CHECK_FALSE(multis[0].has_value());
                        break;
                    }
                    auto op = std::any_cast<std::string>(multis[0])[0];
                    switch (op)
                    {
                    case '*':
                        val *= std::any_cast<int>(multis[1]);
                        break;
                    case '/':
                        val /= std::any_cast<int>(multis[1]);
                        break;
                    default:
                        // error
                        return std::any{};
                    }
                    if (multis[2].has_value() &&
                        multis[2].type() == typeid(std::vector<std::any>))
                    {
                        multis =
                            std::any_cast<std::vector<std::any>>(multis[2]);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            return std::make_any<int>(val);
        };
        auto expr_listener = [](std::vector<std::any> values,
                                lexer_interface&,
                                const std::string&) {
            CHECK_EQ(values[0].type(), typeid(int));
            auto val = std::any_cast<int>(values[0]);
            if (values[1].has_value())
            {
                CHECK_EQ(values[1].type(), typeid(std::vector<std::any>));
                auto exprs = std::any_cast<std::vector<std::any>>(values[1]);
                while (true)
                {
                    if (exprs.size() == 1)
                    {
                        CHECK_FALSE(exprs[0].has_value());
                        break;
                    }
                    auto op = std::any_cast<std::string>(exprs[0])[0];
                    switch (op)
                    {
                    case '+':
                        val += std::any_cast<int>(exprs[1]);
                        break;
                    case '-':
                        val -= std::any_cast<int>(exprs[1]);
                        break;
                    default:
                        // error
                        return std::any{};
                    }
                    if (exprs[2].has_value() &&
                        exprs[2].type() == typeid(std::vector<std::any>))
                    {
                        exprs = std::any_cast<std::vector<std::any>>(exprs[2]);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            return std::make_any<int>(val);
        };
        std::string                              test_input = "1+2-3*4/5+(6)";
        std::stringstream                        ss_input{test_input};
        typename pis::LLTesting::test_calculator parser;
        parser.add_parser_listener("Const", const_listener);
        parser.add_parser_listener("Multi", multi_listener);
        parser.add_parser_listener("Expr", expr_listener);
        auto parse_result = parser.parse(ss_input);
        CHECK(parse_result.has_value());
        CHECK_EQ(parse_result.type(), typeid(int));
        CHECK_EQ(std::any_cast<int>(parse_result), 7);
    }
}
#endif
