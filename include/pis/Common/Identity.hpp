// ParserInString -*- C++ -*-

// Copyright (C) 2018  Scott Brown

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

#ifndef PARSER_IN_STRING_COMMON_IDENTITY
#define PARSER_IN_STRING_COMMON_IDENTITY
#if __cpp_lib_type_identity >= 201806L and __has_include(<type_identity>)
#    include <type_identity>
#endif
namespace pis
{
#if __cpp_lib_type_identity >= 201806L
    template <typename T>
    using identity = std::type_identity<T>;
#else
    template <typename T>
    struct identity
    {
        using type = T;
    };
#endif
    // codes originally from
    // https://musteresel.github.io/posts/2018/03/c++-lazy-template-instantiation.html
    template <template <class...> class T, typename... Args>
    struct lazy_template
    {
        using type = T<Args...>;
    };
    template <typename T>
    struct empty_base: private T
    {
        using T::T;
    };
} // namespace pis
#endif
