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

#ifndef PARSER_IN_STRING_COMPILER
#define PARSER_IN_STRING_COMPILER

#include "TAVL.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tfuncs.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Lexer/Lexer.hpp"
#include "pis/Matcher/FirstFollow.hpp"
#include "pis/Matcher/parser_base.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include <type_traits>
namespace pis
{
    template <template <typename> typename Lexer,
              template <typename R,
                        typename S,
                        unsigned KP,
                        template <typename> typename L = Lexer>
              typename parser,
              typename Start,
              unsigned K,
              const char*... Rules>
    class compiler_t
    {
    public:
        using real_parser_t =
            parser<code_to_rules_t<Rules...>, Start, K, Lexer>;
        compiler_t() = default;
        std::any parse(std::istream&      input,
                       const std::string& skip = " \f\n\r\t\v")
        {
            return _parser.parse(input, skip);
        }
        void add_lexer_listener(const std::string& name,
                                lexer_listener_t   listener)
        {
            _parser.add_lexer_listener(name, listener);
        }
        void add_parser_listener(const std::string& name,
                                 parser_listener_t  listener)
        {
            _parser.add_parser_listener(name, listener);
        }

    private:
        real_parser_t _parser;
    };
} // namespace pis
#endif
