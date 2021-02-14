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

#ifndef PARSER_IN_STRING_COMMON_LOG
#define PARSER_IN_STRING_COMMON_LOG

#include <iomanip>
#include <iostream>
namespace pis
{
    inline void log(const std::string& func, int line, const std::string& msg)
    {
        std::cerr << std::setw(12) << func << ":" << std::setw(4) << line << ":"
                  << msg << std::endl;
    }
#ifndef PIS_NO_LOG
#    define PRINT_LOG(msg) log(__func__, __LINE__, msg)
#else
#    define PRINT_LOG(msg)
#endif
} // namespace pis

#endif
