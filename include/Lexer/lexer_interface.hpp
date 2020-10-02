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

#ifndef PARSER_IN_STRING_LEXER_LEXERINTERFACE
#define PARSER_IN_STRING_LEXER_LEXERINTERFACE

#include "Common/runtime.hpp"
#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
namespace Compiler
{
    class lexer_interface;
    using lexer_listener_t = std::function<std::string(std::vector<std::any>&,
                                                       lexer_interface&,
                                                       const std::string&)>;
    std::string lexer_listener_dummy(std::vector<std::any>& /*tokens_current*/,
                                     lexer_interface& /* lexer */,
                                     const std::string& /* target */)
    {
        // do nothing
        return "";
    }
    using parser_interface = std::function<std::vector<std::any>&()>;

    class lexer_interface
    {
    public:
        virtual std::pair<std::vector<std::string>, std::string> read_any() = 0;
        virtual std::istream& get_stream()                                  = 0;
        lexer_listener_t      get_listener(const std::string& target) const
        {
            if (auto it = _listeners.get().find(target);
                it != _listeners.get().end())
            {
                return it->second;
            }
            return lexer_listener_dummy;
        }
        void set_listener(const std::string& target, lexer_listener_t listener)
        {
            _listeners.get()[target] = listener;
        }

    protected:
        lexer_interface(
            parser_interface                                   parser,
            std::unordered_map<std::string, lexer_listener_t>& listener_map):
            _parser(parser),
            _listeners(listener_map)
        {}
        template <typename Str>
        lexer_listener_t get_listener() const
        {
            return get_listener(to_string(Str{}));
        }
        template <typename Str>
        std::string call_listener()
        {
            auto listener = get_listener<Str>();
            return listener(_parser(), *this, to_string(Str{}));
        }

    private:
        parser_interface _parser;
        std::reference_wrapper<
            std::unordered_map<std::string, lexer_listener_t>>
            _listeners;
    };
} // namespace Compiler

#endif
