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

#ifndef PARSER_IN_STRING_STREAM_STREAM
#define PARSER_IN_STRING_STREAM_STREAM
#include "pis/Config.hpp"
#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <istream>
#include <locale>
#include <memory>
#include <streambuf>
#include <type_traits>

namespace pis
{
    template <typename CharT, typename Traits = std::char_traits<CharT>>
    class basic_postream
    {
    public:
        using int_type = typename std::basic_istream<CharT, Traits>::int_type;
        using pos_type = typename Traits::pos_type;
        using off_type = typename Traits::off_type;
        explicit basic_postream(std::basic_istream<CharT, Traits>& input):
            _input(input)
        {}
        basic_postream(basic_postream&&)      = default;
        basic_postream(const basic_postream&) = delete;
        basic_postream& operator=(basic_postream&&) = default;
        int_type        get()
        {
            read();
            int_type result = EOF;
            std::swap(_lookahead, result);
            return result;
        }
        int_type peek()
        {
            read();
            return _lookahead;
        }
        void swap(basic_postream& other)
        {
            std::swap(_lookahead, other._lookahead);
            std::swap(_input, other._input);
        }
        pos_type tellg()
        {
            return _input.get().tellg();
        }
        basic_postream& seekg(pos_type pos)
        {
            _input.get().seekg(pos);
            return *this;
        }
        basic_postream& seekg(off_type off, std::ios_base::seekdir dir)
        {
            _input.get().seekg(off, dir);
            return *this;
        }

    private:
        void read()
        {
            if (_lookahead == EOF && !_input.get().eof())
            {
                _lookahead = _input.get().get();
                if constexpr (config_use_newline)
                {
                    if (_lookahead == '\r')
                    {
                        _lookahead = '\n';
                        if (_input.get().peek() == '\n')
                        {
                            _input.get().get();
                        }
                    }
                }
            }
        }
        int_type _lookahead = EOF;
        std::reference_wrapper<std::basic_istream<CharT, Traits>> _input;
    };
} // namespace pis

#endif
