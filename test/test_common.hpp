// ParserInString -*- C++ -*-

// Copyright (C) 2020 Scott Brown

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
#include <any>
#include <iostream>
#include <type_traits>
#include <vector>
#ifndef PARSER_IN_STRING_TEST_TESTCOMMON
#    define PARSER_IN_STRING_TEST_TESTCOMMON
inline std::string rtrim(const std::string& str)
{
    return str.substr(0, str.find_last_not_of(" \t\n") + 1);
}
inline std::string print_any(const std::vector<std::any>& any_input);
inline std::string print_any(const std::any& val)
{
    std::string result;
    if (val.type() == typeid(std::string))
    {
        result += std::any_cast<std::string>(val);
    }
    else if (val.type() == typeid(std::vector<std::any>))
    {
        result += print_any(std::any_cast<std::vector<std::any>>(val));
    }
    else if (val.has_value())
    {
        std::cerr << "Warning: unknown type of std::any[" << val.type().name()
                  << std::endl;
    }
    return result;
}
inline std::string print_any(const std::vector<std::any>& any_input)
{
    std::string result;
    result = "[";
    for (const auto& member : any_input)
    {
        result += print_any(member);
        result += ", \n";
    }
    return result.substr(0, result.size() - 3) + "]";
}

#endif
