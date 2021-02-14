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

#ifndef PARSER_IN_STRING_LEXER_FSM
#define PARSER_IN_STRING_LEXER_FSM

#include "TAVL.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tfuncs.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Rules/EBNF.hpp"
#include <type_traits>
template <typename T>
struct check_valid
{
    using type = T;
};
template <>
struct check_valid<tavl::Impl::invalid>
{};
template <typename T>
using check_valid_t = typename check_valid<T>::type;
namespace pis
{
    inline constexpr char16_t EMPTY_TRANS_EDGE = -1;
    inline constexpr unsigned START_STATE      = 0;
    namespace Impl
    {
        // FA:
        // tavl_node<L, R, H,
        //     value_t<DFA_STATE>,
        //     tavl_node<L, R, H,
        //         value_t<EDGE>,
        //         tavl_node<L, R, H, value_t<DFA_STATE>>
        //     >
        // >
        template <typename Start, typename L, typename End, typename FA>
        struct fa_add_edge
        {
            template <typename F>
            using try_add_end = check_valid_t<tavl::tavl_update_t<
                F,
                Start,
                tavl::tavl_update_t<
                    tavl_find_value<F, Start>,
                    L,
                    set_try_insert_t<
                        tavl_find_value<tavl_find_value<F, Start>, L>,
                        End>>>>;
            template <typename F>
            using try_add_label = check_valid_t<tavl::tavl_update_t<
                F,
                Start,
                tavl::tavl_insert_t<tavl_find_value<F, Start>,
                                    L,
                                    tavl::kv_pair<End>>>>;
            template <typename F>
            using try_add_all = check_valid_t<
                tavl::tavl_insert_t<F,
                                    Start,
                                    tavl::kv_pair<L, tavl::kv_pair<End>>>>;
            template <typename F>
            using start_exist = std::conditional_t<
                tavl::tavl_contain_v<tavl_find_value<F, Start>, L>,
                lazy_template<try_add_end, FA>,
                // L not exist
                lazy_template<try_add_label, FA>>;
            using type = typename std::conditional_t<
                tavl::tavl_contain_v<FA, Start>,
                lazy_template<start_exist, FA>,
                // Start not exist
                identity<lazy_template<try_add_all, FA>>>::type::type;
        };
        template <typename Start, typename L, typename End, typename FA>
        using fa_add_edge_t = typename fa_add_edge<Start, L, End, FA>::type;
        template <typename Start,
                  typename End,
                  char16_t CurrChar,
                  char16_t EndChar,
                  typename NFA>
        struct fa_add_chars
        {
            using type = typename fa_add_chars<
                Start,
                End,
                CurrChar + 1,
                EndChar,
                fa_add_edge_t<Start, value_wrapper<CurrChar>, End, NFA>>::type;
        };
        template <typename Start, typename End, char16_t EndChar, typename NFA>
        struct fa_add_chars<Start, End, EndChar, EndChar, NFA>: identity<NFA>
        {};
        template <typename Start,
                  typename End,
                  char16_t CurrChar,
                  char16_t EndChar,
                  typename NFA>
        using fa_add_chars_t =
            typename fa_add_chars<Start, End, CurrChar, EndChar, NFA>::type;
    } // namespace Impl
} // namespace pis
#endif
