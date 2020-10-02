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

#ifndef PARSER_IN_STRING_MATCHER_COMPILER
#define PARSER_IN_STRING_MATCHER_COMPILER

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "LR0.hpp"
#include "LR_common.hpp"
#include "Lexer/Lexer.hpp"
#include "Matcher/FirstFollow.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include <type_traits>

namespace Compiler
{
    namespace lalr_impl
    {
        using namespace impl;
        // a copy of LR(0)'s item set generation method
        // with unnecessary closure removed
        template <typename Rules>
        struct generate_lr0_kernel_sets
        {
            static_assert(
                !std::is_same_v<
                    type_list_top<tavl_find_value<Rules, dummy_target>>,
                    type_list<>>,
                "missing starting rule");
            using starting_items = type_list<lr0_single_item<
                dummy_target,
                type_list<>,
                type_list_top<tavl_find_value<Rules, dummy_target>>>>;

            template <typename Item, bool is_end = lr0_impl::is_last_item<Item>>
            struct try_get_next_item
            {
                using type = type_list_top<typename Item::post_dot>;
            };
            template <typename Item>
            struct try_get_next_item<Item, true>
            {
                using type = no_such_item;
            };
            template <typename I>
            using try_get_next_item_t = try_get_next_item<I>;
            template <typename Items, typename>
            struct for_each_item_set
            {
                using maybe_next_trans = type_list_remove<
                    tlist_set_for_each<lr0_impl::closure_t<Rules, Items>,
                                       try_get_next_item_t,
                                       tlist_set_try_insert_2,
                                       type_list<>>,
                    no_such_item>;
                using type = type_pair<Items, maybe_next_trans>;
            };
            template <typename SetsTrans, typename P>
            struct add_trans;
            template <typename SetTrans, typename Items, typename Labels>
            struct add_trans_impl;
            template <typename SetTrans, typename Items, typename Labels>
            using add_trans_impl_t =
                typename add_trans_impl<SetTrans, Items, Labels>::type;
            template <typename Sets, typename Items, typename Labels>
            struct add_trans<Sets, type_pair<Items, Labels>>
            {
                using type = add_trans_impl_t<Sets, Items, Labels>;
            };
            template <typename Sets, typename Items, typename Labels>
            struct add_trans_impl
            {
                using label = type_list_top<Labels>;
                using target =
                    lr0_impl::expand_t<lr0_impl::closure_t<Rules, Items>,
                                       label>;
                using type =
                    typename add_trans_impl<set_try_insert_t<Sets, target>,
                                            Items,
                                            type_list_tail<Labels>>::type;
            };
            template <typename Sets, typename Items>
            struct add_trans_impl<Sets, Items, type_list<>>
            {
                using type = Sets;
            };
            template <typename Sets>
            struct generate_item_sets
            {
                using type =
                    tavl::tavl_for_each_middle_order_t<Sets,
                                                       for_each_item_set,
                                                       add_trans,
                                                       Sets>;
            };
            using type = typename do_until<tavl::tavl_is_same,
                                           generate_item_sets,
                                           tavl::kv_pair<starting_items>>::type;
        };
        template <typename Rules>
        using generate_lr0_kernel_sets_t =
            typename generate_lr0_kernel_sets<Rules>::type;

        template <typename Rules, typename Items, typename Symbol>
        struct determine_lookaheads
        {};
    } // namespace lalr_impl
} // namespace Compiler
#endif
