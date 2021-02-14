// ParserInString -*- C++ -*-

// Copyright (C) 2020  Scott Brown

// This file is part of the ParserInString library.
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

#ifndef PARSER_IN_STRING_MATCHER_PARSERBASE
#define PARSER_IN_STRING_MATCHER_PARSERBASE

#include "pis/Lexer/lexer_interface.hpp"
#include <any>
#include <functional>

namespace pis
{
    using parser_listener_t = std::function<
        std::any(std::vector<std::any>, lexer_interface&, const std::string&)>;
    std::any parser_listener_dummy(std::vector<std::any> tokens_current,
                                   lexer_interface& /* lexer */,
                                   const std::string& /* target */)
    {
        return std::make_any<std::vector<std::any>>(tokens_current);
    }
    namespace impl
    {
        using parser_listener_map_t =
            std::unordered_map<std::string, parser_listener_t>;
    }
} // namespace pis

#endif
