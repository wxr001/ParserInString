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

// Grammar rules originally from https://github.com/cviebig/lib-sql-text
#ifndef PARSER_IN_STRING_TESTSQL
#define PARSER_IN_STRING_TESTSQL

#include "Common/Tstring.hpp"
#include "Lexer/Lexer.hpp"
#include "Matcher/LL.hpp"
#include "Rules/CodeToRules.hpp"
#include "TAVL.hpp"
#include "test_common.hpp"
#include "doctest.h"
#include "test_sql_rules.hpp"

namespace Compiler
{
    using sql_ll    = ll_parsing<sql_rules, _TSTR("sql"), 1>;
} // namespace Compiler
TEST_CASE("ll(1)")
{
    typename Compiler::sql_ll::parsing_table::state_map          _s;
    typename Compiler::sql_ll::parsing_table::parsing_table      _p;
    std::unordered_map<std::string, Compiler::parser_listener_t> _m;
    Compiler::sql_ll::parsing_table table{_s, _p, _m};
    std::cerr<<table.dump()<<std::endl;
}
#endif
