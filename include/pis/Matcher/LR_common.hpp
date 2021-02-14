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

#ifndef PARSER_IN_STRING_MATCHER_LRCOMMON
#define PARSER_IN_STRING_MATCHER_LRCOMMON

#include "TAVL.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tfuncs.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Lexer/Lexer.hpp"
#include "pis/Matcher/FirstFollow.hpp"
#include "pis/Rules/EBNF.hpp"
#include <type_traits>
namespace pis
{
    namespace impl
    {
        using eof_tag_term =
            type_pair<Impl::terminal_string_flag, t_string<'\0'>>;
        using dummy_target = _TSTR("@@START");
        template <typename Rules, typename Start>
        using add_dummy_rule = tavl::tavl_insert_t<
            Rules,
            dummy_target,
            type_list<type_list<type_pair<Impl::meta_identifier_flag, Start>>>>;
        template <typename Rules, typename Start>
        using add_dummy_rule_with_eof_tag = tavl::tavl_insert_t<
            Rules,
            dummy_target,
            type_list<type_list<type_pair<Impl::meta_identifier_flag, Start>,
                                eof_tag_term>>>;
        template <typename Target,
                  typename PreDef,
                  typename PostDef,
                  typename Lookahead>
        struct lr_single_item
        {
            using target     = Target;
            using definition = type_list_push_back_m<PreDef, PostDef>;
            using pre_dot    = PreDef;
            using post_dot   = PostDef;
            using lookahead  = Lookahead;
        };
        struct no_such_item;
        template <typename Item>
        inline constexpr bool is_last_item =
            std::is_same_v<typename Item::post_dot, type_list<>>;
        template <typename Item>
        struct next_item
        {
            using type = lr_single_item<
                typename Item::target,
                type_list_push_back<typename Item::pre_dot,
                                    type_list_top<typename Item::post_dot>>,
                type_list_tail<typename Item::post_dot>,
                typename Item::lookahead>;
        };
        template <typename Target, typename PreDef, typename Lookahead>
        struct next_item<lr_single_item<Target, PreDef, type_list<>, Lookahead>>
        {
            using type = no_such_item;
        };
        template <typename Item>
        using next_item_t = typename next_item<Item>::type;
        template <typename Rules, typename Items, unsigned K>
        struct closure
        {
            using first_set = first_set_k<Rules, K>;
            template <typename Item, typename>
            struct for_each_item
            {
                using type = tavl::empty_node;
            };
            template <typename Target,
                      typename PreDef,
                      typename NonTerminal,
                      typename... PostDef,
                      typename Lookahead,
                      typename T>
            struct for_each_item<
                lr_single_item<Target,
                               PreDef,
                               type_list<type_pair<Impl::meta_identifier_flag,
                                                   NonTerminal>,
                                         PostDef...>,
                               Lookahead>,
                T>
            {
                using first_set_after_nt = set_addition_t<
                    first_set_for_definition_t<first_set,
                                               type_list<PostDef...>,
                                               K>,
                    tavl::kv_pair<Lookahead>,
                    K>;
                template <typename Definition>
                struct for_each_definition_of_target
                {
                    template <typename Symbol, typename>
                    struct for_each_first_symbol
                    {
                        using type = tavl::kv_pair<lr_single_item<NonTerminal,
                                                                  type_list<>,
                                                                  Definition,
                                                                  Symbol>>;
                    };
                    using type = tavl::tavl_for_each_t<first_set_after_nt,
                                                       for_each_first_symbol,
                                                       tavl::tavl_union_3,
                                                       tavl::empty_node>;
                };
                using type = tlist_set_for_each<
                    typename tavl::tavl_find_t<Rules, NonTerminal>::value,
                    for_each_definition_of_target,
                    tavl::tavl_union_2,
                    tavl::empty_node>;
            };
            template <typename Last>
            struct repeat_until
            {
                using last_items = typename Last::first_type;
                using last_new   = typename Last::second_type;
                using this_new   = tavl::tavl_for_each_t<last_new,
                                                       for_each_item,
                                                       tavl::tavl_union_3,
                                                       tavl::empty_node>;
                using this_items = tavl::tavl_union_t<last_items, this_new>;
                using type       = type_pair<this_items, this_new>;
            };
            template <typename Last, typename Cur>
            struct stop_cond
            {
                static constexpr bool value =
                    tavl::is_empty_node_v<typename Cur::second_type>;
            };
            using type =
                typename do_until<stop_cond,
                                  repeat_until,
                                  type_pair<Items, Items>>::type::first_type;
        };
        template <typename Rules, typename Items, unsigned K>
        using closure_t = typename closure<Rules, Items, K>::type;
        template <typename Rules, typename Set, typename Symbol, unsigned K>
        struct goto_k
        {
            template <typename Item, typename>
            struct for_each_item
            {
                using type = tavl::empty_node;
            };
            template <typename Target,
                      typename Pre,
                      typename... Post,
                      typename Lookahead,
                      typename V>
            struct for_each_item<lr_single_item<Target,
                                                Pre,
                                                type_list<Symbol, Post...>,
                                                Lookahead>,
                                 V>
            {
                using type = tavl::kv_pair<
                    next_item_t<lr_single_item<Target,
                                               Pre,
                                               type_list<Symbol, Post...>,
                                               Lookahead>>>;
            };
            using goto_kernel =
                tavl::tavl_remove_t<tavl::tavl_for_each_t<Set,
                                                          for_each_item,
                                                          tavl::tavl_union_3,
                                                          tavl::empty_node>,
                                    no_such_item>;
            using type = closure_t<Rules, goto_kernel, K>;
        };
        template <typename Rules, typename Set, typename Symbol, unsigned K>
        using goto_k_t = typename goto_k<Rules, Set, Symbol, K>::type;
        class lr_stack_symbol
        {
        public:
            struct hash
            {
                std::size_t operator()(const lr_stack_symbol& s) const noexcept
                {
                    auto h1 = std::hash<bool>{}(s._is_nonterminal);
                    auto h2 = std::hash<std::string>{}(s._value);
                    return h1 ^ (h2 << 1);
                }
            };
            template <typename T>
            lr_stack_symbol(T)
            {
                _is_nonterminal = std::is_same_v<typename T::first_type,
                                                 Impl::meta_identifier_flag>;
                _value          = to_string(typename T::second_type{});
            }
            lr_stack_symbol(const lr_stack_symbol&) = default;
            lr_stack_symbol(bool nont, const std::string& str):
                _is_nonterminal(nont),
                _value(str)
            {}
            std::string dump() const
            {
                if (_is_nonterminal)
                {
                    return "<" + _value + ">";
                }
                else
                {
                    return "\"" + _value + "\"";
                }
            }
            bool is_match(const std::vector<std::string>& types) const
            {
                return !_is_nonterminal &&
                       std::find(types.begin(), types.end(), _value) !=
                           types.end();
            }
            bool operator==(const lr_stack_symbol& rhs) const noexcept
            {
                return _is_nonterminal == rhs._is_nonterminal &&
                       _value == rhs._value;
            }
            bool operator!=(const lr_stack_symbol& rhs) const noexcept
            {
                return !(*this == rhs);
            }

        private:
            bool        _is_nonterminal;
            std::string _value;
        };
        inline lr_stack_symbol make_terminal_symbol(const std::string& str)
        {
            return lr_stack_symbol{false, str};
        }
        inline lr_stack_symbol make_nonterminal_symbol(const std::string& str)
        {
            return lr_stack_symbol{true, str};
        }
        template <typename Symbol>
        inline lr_stack_symbol make_lr_stack_symbol()
        {
            if constexpr (std::is_same_v<typename Symbol::first_type,
                                         Impl::terminal_string_flag>)
            {
                return make_terminal_symbol(
                    to_string(typename Symbol::second_type{}));
            }
            else
            {
                static_assert(std::is_same_v<typename Symbol::first_type,
                                             Impl::meta_identifier_flag>,
                              "Unknown Symbol Type");
                return make_nonterminal_symbol(
                    to_string(typename Symbol::second_type{}));
            }
        }
    } // namespace impl
    template <typename Rules,
              typename Start,
              unsigned K,
              template <typename> typename Lexer = lexer>
    class lr_parsing;

} // namespace pis

#endif
