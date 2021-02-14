// ParserInString -*- C++ -*-

// Copyright (C) 2019 Scott Brown

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

#ifndef PARSER_IN_STRING_COMMON_TFUNCS
#define PARSER_IN_STRING_COMMON_TFUNCS

#include "Tlist.hpp"
#include <type_traits>

namespace pis
{
    template <typename expr,
              typename conds,
              typename vals,
              typename default_val = void>
    struct template_switch
    {
    private:
        static_assert(type_list_size<conds> > 0, "empty conditions");
        static_assert(type_list_size<conds> >= type_list_size<vals>,
                      "Results are too many");
        using conditions = conds;
        using values_list =
            type_list_push_back_n<vals,
                                  default_val,
                                  type_list_size<conds> - type_list_size<vals>>;
        static constexpr int target_cond = type_list_find<conditions, expr>;
        template <typename L>
        using type_list_get_wrapper = type_list_get<L, target_cond>;
        using result                = typename std::conditional_t<
            target_cond == -1,
            identity<default_val>,
            lazy_template<type_list_get_wrapper, values_list>>::type;

    public:
        using type = result;
    };
    template <typename expr,
              typename conds,
              typename vals,
              typename default_val = void>
    using template_switch_t =
        typename template_switch<expr, conds, vals, default_val>::type;
    template <template <typename Last, typename Curr> typename Expr,
              template <typename Last>
              typename Func,
              typename Start = void>
    struct do_until
    {
    private:
        template <typename Last>
        struct helper
        {
            using current = Func<Last>;
            static constexpr bool has_next =
                !Expr<Last, typename current::type>::value;
            using type = typename std::conditional_t<
                has_next,
                lazy_template<helper, typename current::type>,
                identity<current>>::type::type;
        };

    public:
        using type = typename helper<Start>::type;
    };
    template <template <typename Last, typename Curr> typename Expr,
              template <typename Last>
              typename Func,
              typename Start = void>
    struct do_while
    {
    private:
        template <typename Last>
        struct helper
        {
            using current = Func<Last>;
            static constexpr bool has_next =
                Expr<Last, typename current::type>::value;
            using type = typename std::conditional_t<
                has_next,
                lazy_template<helper, typename current::type>,
                identity<current>>::type::type;
        };

    public:
        using type = typename helper<Start>::type;
    };
} // namespace pis
#endif
