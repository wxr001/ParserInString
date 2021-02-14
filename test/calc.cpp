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
using namespace pis;
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
using test_calculator =
    ll_parsing<code_to_rules_t<calculator_no_left_recursion>, _TSTR("Expr"), 1>;
int main()
{
    auto const_listener =
        [](std::vector<std::any> values, lexer_interface&, const std::string&) {
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
                    multis = std::any_cast<std::vector<std::any>>(multis[2]);
                }
                else
                {
                    break;
                }
            }
        }
        return std::make_any<int>(val);
    };
    auto expr_listener =
        [](std::vector<std::any> values, lexer_interface&, const std::string&) {
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
    test_calculator parser;
    parser.add_parser_listener("Const", const_listener);
    parser.add_parser_listener("Multi", multi_listener);
    parser.add_parser_listener("Expr", expr_listener);
    std::string test_input;
    std::cin >> test_input;
    std::stringstream ss_input{test_input};
    auto              parse_result = parser.parse(ss_input);
    CHECK(parse_result.has_value());
    CHECK_EQ(parse_result.type(), typeid(int));
    std::cout << std::any_cast<int>(parse_result) << std::endl;
}