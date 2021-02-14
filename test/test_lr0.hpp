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

#ifndef PARSER_IN_STRING_TESTLR0
#define PARSER_IN_STRING_TESTLR0

#include "TAVL.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tfuncs.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Lexer/Lexer.hpp"
#include "pis/Matcher/LR0.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include "pis/Rules/EBNF.hpp"
#include "test_common.hpp"
#include <doctest.h>
#include <iostream>
#include <type_traits>
DECL_RULE(calculator, R"(
E = E, '-', T |
    T;
T = ?lexer i?, {?number?} |
    '(', E, ')';)");
namespace pis
{
    namespace LR0_TESTING
    {
        using calculator_test =
            lr_parsing<code_to_rules_t<calculator_rule>, t_string<'E'>, 0>;
    }
} // namespace pis
TEST_SUITE("LR(0)")
{
    using namespace pis;
    TEST_CASE("calculator")
    {
        typename LR0_TESTING::calculator_test calc;
        std::string                           test_input = "1-3-(6)";
        std::stringstream                     ss_input{test_input};
        constexpr auto                        parse_result = R"([[[[1]], 
-, 
[3]], 
-, 
[(, 
[[6]], 
)]])";
        std::cout << parse_result << std::endl;
        CHECK_EQ(print_any(calc.parse(ss_input)), parse_result);
    }
    TEST_CASE("calculator-runtime")
    {
        typename LR0_TESTING::calculator_test calc;
        std::string                           test_input = "10-3-(6)";
        std::stringstream                     ss_input{test_input};
        auto t_listener = [](std::vector<std::any> values,
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
                // ( E )
                CHECK_EQ(values.size(), 3);
                CHECK_EQ(values[1].type(), typeid(int));
                return values[1];
            }
        };
        auto e_listener = [](std::vector<std::any> values,
                             lexer_interface&,
                             const std::string&) {
            if (values.size() == 1)
            {
                CHECK_EQ(values[0].type(), typeid(int));
                return values[0];
            }
            else
            {
                // ( E )
                CHECK_EQ(values.size(), 3);
                CHECK_EQ(values[0].type(), typeid(int));
                CHECK_EQ(values[2].type(), typeid(int));
                return std::make_any<int>(std::any_cast<int>(values[0]) -
                                          std::any_cast<int>(values[2]));
            }
        };
        calc.add_parser_listener("T", t_listener);
        calc.add_parser_listener("E", e_listener);
        auto result = calc.parse(ss_input);
        CHECK_EQ(result.type(), typeid(int));
        CHECK_EQ(std::any_cast<int>(result), 1);
    }
}
#endif
