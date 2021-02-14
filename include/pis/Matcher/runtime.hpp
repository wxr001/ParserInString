// ParserInString -*- C++ -*-

// Copyright (C) 2018  Scott Brown

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

#ifndef UNIVERSAL_COMPILER_LIBRARY_GRAMMAR_MATCHER_RUNTIME
#define UNIVERSAL_COMPILER_LIBRARY_GRAMMAR_MATCHER_RUNTIME

#include "pis/Common/Tstring.hpp"
#include <istream>
#include <iterator>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

namespace pis
{
    template <char... chars>
    inline std::string to_string(t_string<chars...>)
    {
        using namespace std::string_literals;
        return (""s + ... + chars);
    }
    // codes originally from https://stackoverflow.com/a/33447587
    template <typename I>
    std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1)
    {
        static const char* digits = "0123456789ABCDEF";
        std::string        rc(hex_len, '0');
        for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
            rc[i] = digits[(w >> j) & 0x0f];
        return rc;
    }
    template <typename T>
    inline std::string type_info_str()
    {
        return n2hexstr(std::type_index{typeid(T)}.hash_code());
    }
    template <char... chars>
    inline int comp_spaceship(const std::string& str, t_string<chars...>)
    {
        char diff = 0;
        if (auto it = str.begin(), e = str.end();
            (... && (it != e && ((diff = *it++ - chars) == 0))))
        {
            // t_string reaches end
            return it == e ? std::distance(str.begin(), it) - sizeof...(chars) :
                             1;
        }
        else
        {
            if (it == e && diff == 0)
            {
                return -1;
            }
            // diff is not 0
            return diff;
        }
    }
    template <char... chars>
    inline int comp_spaceship(t_string<chars...> v, const std::string& str)
    {
        return -comp_spaceship(str, v);
    }
    inline std::string peek(std::istream& input, int count)
    {
        std::string result(count, '\0');
        input.get(&result[0], count);
        input.seekg(-count, input.cur);
        return result;
    }
} // namespace pis

#endif
