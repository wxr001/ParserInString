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

#ifndef PARSER_IN_STRING_TEST_C11LL
#define PARSER_IN_STRING_TEST_C11LL

#include "pis/Common/Tstring.hpp"
#include "pis/Lexer/Lexer.hpp"
#include "pis/Matcher/LL.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include "pis/Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <doctest.h>
#include <type_traits>
#ifdef C11
#    include "test_C11_rule.hpp"
#endif
namespace pis
{
#ifndef C11
    using c11 = void;
#endif
    using c11_ll = ll_parsing<c11, _TSTR("translation unit"), 1>;
} // namespace pis
TEST_CASE("ll(1)")
{
    Compiler::c11_ll parser;
    parser.dump();
}
#endif
