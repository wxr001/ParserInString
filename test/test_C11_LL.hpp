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

#include "Common/Tstring.hpp"
#include "Lexer/Lexer.hpp"
#include "Matcher/LL.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <doctest.h>
#include <type_traits>
#ifdef C11
#    include "test_C11_rule.hpp"
#endif
namespace Compiler
{
#ifndef C11
    using c11 = void;
#endif
    using c11_ll = ll_parsing<c11, _TSTR("translation unit"), 1>;
} // namespace Compiler
TEST_CASE("ll(1)")
{
    Compiler::c11_ll parser;
    parser.dump();
}
#endif
