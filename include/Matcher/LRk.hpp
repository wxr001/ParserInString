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

#ifndef PARSER_IN_STRING_MATCHER_LRK
#define PARSER_IN_STRING_MATCHER_LRK

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Lexer/Lexer.hpp"
#include "Matcher/FirstFollow.hpp"
#include "Matcher/LR_common.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include "lookahead.hpp"
#include <any>
#include <optional>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>
namespace Compiler
{
    namespace impl
    {
        template <typename Rules, typename Starting, unsigned K>
        struct generate_itemset
        {
            // calculate set of dot symbols
            template <typename Item, typename>
            struct for_each_item
            {
                using type =
                    tavl::kv_pair<type_list_top<typename Item::post_dot>>;
            };
            template <typename S, typename P, typename L, typename V>
            struct for_each_item<lr_single_item<S, P, type_list<>, L>, V>
            {
                using type = tavl::empty_node;
            };
            // for each itemset
            template <typename Set, typename /*outs*/>
            struct for_each_itemset
            {
                using type = type_pair<Set,
                                       tavl::tavl_for_each_t<Set,
                                                             for_each_item,
                                                             tavl::tavl_union_3,
                                                             tavl::empty_node>>;
            };
            template <typename SetAndMap, typename Dot>
            struct try_goto
            {
                using set  = typename SetAndMap::first_type;
                using map  = typename SetAndMap::second_type;
                using type = type_pair<
                    set,
                    tavl::tavl_update_t<
                        map,
                        set,
                        tavl::tavl_update_t<tavl_find_empty_node<map, set>,
                                            Dot,
                                            goto_k_t<Rules, set, Dot, K>>>>;
            };
            template <typename Map, typename SetAndDotSymbols>
            struct for_each_dot_symbol
            {
                using current_set = typename SetAndDotSymbols::first_type;
                using dot_symbols = typename SetAndDotSymbols::second_type;
                using type        = typename tavl::tavl_for_each_middle_order_t<
                    dot_symbols,
                    get_key,
                    try_goto,
                    type_pair<current_set, Map>>::second_type;
            };
            template <typename LastMap>
            struct repeat_until
            {
                using type =
                    tavl::tavl_for_each_middle_order_t<LastMap,
                                                       for_each_itemset,
                                                       for_each_dot_symbol,
                                                       LastMap>;
            };
            using type = typename do_until<
                tavl::tavl_is_same,
                repeat_until,
                tavl::kv_pair<Starting, tavl::empty_node>>::type;
        };
        template <typename Rules, typename Starting, unsigned K>
        using generate_itemset_t =
            typename generate_itemset<Rules, Starting, K>::type;
        template <typename Item>
        struct is_reduce_item
        {
            static constexpr bool value =
                std::is_same_v<typename Item::post_dot, type_list<>>;
        };
        template <typename Item>
        inline constexpr bool is_reduce_item_v = is_reduce_item<Item>::value;
        template <typename Item, typename>
        using is_reduce_item_func =
            identity<std::bool_constant<is_reduce_item_v<Item>>>;
        template <typename ItemSet>
        struct contains_reduce_item
        {
            static constexpr bool value =
                tavl::tavl_for_each_t<ItemSet,
                                      is_reduce_item_func,
                                      logical_or_merger,
                                      std::false_type>::value;
        };
        template <typename ItemSet>
        inline constexpr bool contains_reduce_item_v =
            contains_reduce_item<ItemSet>::value;
        template <typename Item>
        struct is_shift_item
        {
            static constexpr bool value =
                !std::is_same_v<typename Item::post_dot, type_list<>>;
        };
        template <typename Item>
        inline constexpr bool is_shift_item_v = is_shift_item<Item>::value;
        template <typename Item, typename>
        using is_shift_item_func =
            identity<std::bool_constant<is_shift_item_v<Item>>>;
        template <typename ItemSet>
        struct contains_shift_item
        {
            static constexpr bool value =
                tavl::tavl_for_each_t<ItemSet,
                                      is_shift_item_func,
                                      logical_or_merger,
                                      std::false_type>::value;
        };
        template <typename ItemSet>
        inline constexpr bool contains_shift_item_v =
            contains_shift_item<ItemSet>::value;
        class lr_stack;
        class lr_state
        {
        public:
            virtual std::optional<std::pair<std::string, std::vector<std::any>>>
                try_reduce(lr_stack&,
                           const lookahead_symbol::lookahead_type&) const
            {
                return std::optional<
                    std::pair<std::string, std::vector<std::any>>>();
            }
            virtual std::string dump() const
            {
                return "";
            }

        protected:
            template <typename Item>
            std::string do_dump() const
            {
                if constexpr (tavl::is_empty_node_v<Item>)
                {
                    return "";
                }
                else
                {
                    return do_dump<typename Item::left>() +
                           dump_single_item<typename Item::key>() +
                           do_dump<typename Item::right>();
                }
            }
            template <typename Item>
            std::string dump_single_item() const
            {
                std::string result;
                dump_single_item<Item>(result);
                return result;
            }
            template <typename Item>
            void dump_single_item(std::string& result) const
            {
                result += to_string(typename Item::target{}) + " -> ";
                dump_def<typename Item::pre_dot>(result);
                result += " * ";
                dump_def<typename Item::post_dot>(result);
                dump_lookahead(typename Item::lookahead{}, result);
                result += "\n";
            }

        private:
            template <typename Defs>
            std::enable_if_t<!std::is_same_v<Defs, type_list<>>>
                dump_def(std::string& str) const
            {
                using current = type_list_top<Defs>;
                using next    = type_list_tail<Defs>;
                if constexpr (std::is_same_v<typename current::first_type,
                                             Impl::terminal_string_flag> ||
                              std::is_same_v<typename current::first_type,
                                             Impl::special_sequence_flag>)
                {
                    str += "\"" + to_string(typename current::second_type{}) +
                           "\", ";
                }
                else if constexpr (std::is_same_v<typename current::first_type,
                                                  Impl::meta_identifier_flag>)
                {
                    str += "<" + to_string(typename current::second_type{}) +
                           ">, ";
                }
                else if constexpr (std::is_same_v<typename current::first_type,
                                                  Impl::empty_sequence_flag>)
                {
                    str += " , ";
                }
                else
                {
                    // unsupported type
                }
                dump_def<next>(str);
            }
            template <typename Defs>
            std::enable_if_t<std::is_same_v<Defs, type_list<>>>
                dump_def(std::string&) const
            {
                // finish parsing
                return;
            }
            template <typename... Str>
            void dump_lookahead(type_list<Str...>, std::string& str) const
            {
                using namespace std::string_literals;
                str += "["s + ((to_string(Str{}) + ", ") + ...);
                str.pop_back();
                str.back() = ']';
            }
        };
        class lr_stack
        {
            using stack_item =
                std::tuple<std::type_index, lr_stack_symbol, std::any>;
            template <std::size_t IDX>
            decltype(auto) get_value_from_state_item(const std::any& item) const
            {
                return std::get<IDX>(std::any_cast<stack_item>(item));
            }
            std::type_index get_state(const std::any& item) const
            {
                return get_value_from_state_item<0>(item);
            }
            const lr_stack_symbol& get_symbol(const std::any& item) const
            {
                return get_value_from_state_item<1>(item);
            }
            const std::any& get_value(const std::any& item) const
            {
                return get_value_from_state_item<2>(item);
            }

        public:
            std::vector<std::any> pop(std::size_t n)
            {
                if (n > _stack.size())
                {
                    throw std::out_of_range("No enough items to pop");
                }
                std::vector<std::any> datas;
                datas.reserve(n);
                for (auto it = std::make_move_iterator(
                              std::prev(_stack.end(), n)),
                          end = std::make_move_iterator(_stack.end());
                     it != end;
                     ++it)
                {
                    datas.emplace_back(get_value(*it));
                }
                _stack.erase(std::prev(_stack.end(), n), _stack.end());
                return datas;
            }

        private:
            std::vector<std::any> _stack;
        };

        template <typename ItemSet,
                  bool has_reduce = contains_reduce_item_v<ItemSet>>
        class lr_state_impl: public lr_state
        {
        public:
            std::string dump() const override
            {
                return do_dump<ItemSet>();
            }
        };
        template <typename ItemSet>
        class lr_state_impl<ItemSet, true>: public lr_state
        {
            static_assert(!contains_shift_item_v<ItemSet>,
                          "shift/reduce conflict");
            struct reduce_item
            {
                std::unique_ptr<lookahead_symbol> lookahead;
                std::string                       target;
                std::size_t                       len_of_def;
                reduce_item(std::unique_ptr<lookahead_symbol> lookahead,
                            const std::string&                target,
                            std::size_t                       len):
                    lookahead(std::move(lookahead)),
                    target(target),
                    len_of_def(len)
                {}
                reduce_item(std::unique_ptr<lookahead_symbol> lookahead,
                            std::string&&                     target,
                            std::size_t                       len):
                    lookahead(std::move(lookahead)),
                    target(std::move(target)),
                    len_of_def(len)
                {}
            };
            template <typename Item>
            void init_lookaheads()
            {
                if constexpr (tavl::is_empty_node_v<Item>)
                {
                    return;
                }
                else
                {
                    init_lookaheads<typename Item::left>();
                    if constexpr (is_reduce_item_v<typename Item::key>)
                    {
                        using item = typename Item::key;
                        _lookaheads.emplace_back(
                            std::make_unique<
                                lookahead_list<typename item::lookahead>>(),
                            to_string(typename item::target{}),
                            type_list_size<typename item::definition>);
                    }
                    init_lookaheads<typename Item::right>();
                }
            }

        public:
            lr_state_impl()
            {
                init_lookaheads<ItemSet>();
            }
            std::optional<std::pair<std::string, std::vector<std::any>>>
                try_reduce(lr_stack&                               stack,
                           const lookahead_symbol::lookahead_type& lookahead)
                    const override
            {
                for (const auto& [la, target, len] : _lookaheads)
                {
                    if (la->is_same(lookahead))
                    {
                        return std::make_pair(target, stack.pop(len));
                    }
                }
                return std::optional<
                    std::pair<std::string, std::vector<std::any>>>();
            }
            std::string dump() const override
            {
                return do_dump<ItemSet>();
            }

        private:
            std::vector<reduce_item> _lookaheads;
        };
    } // namespace impl
    template <typename Rules,
              typename Start,
              unsigned K,
              template <typename>
              typename Lexer>
    class lr_parsing
    {
        using lexer_t     = Lexer<Rules>;
        using lexer_rules = typename lexer_t::type;
        // add dummy rule S' = S to the rule-set
        using rules = impl::add_dummy_rule<lexer_rules, Start>;

        using starting_items = impl::closure_t<
            rules,
            tavl::kv_pair<impl::lr_single_item<
                impl::dummy_target,
                type_list<>,
                type_list<type_pair<Impl::meta_identifier_flag, Start>>,
                end_marker<K>>>,
            K>;
        using transition_map =
            impl::generate_itemset_t<Rules, starting_items, K>;
        template <typename Out, typename Edge, typename In>
        void add_edge_to_map()
        {
            _parsing_table[std::type_index(typeid(Out))]
                          [impl::make_lr_stack_symbol<Edge>()] =
                              std::type_index(typeid(In));
        }
        template <typename Out, typename Edge, typename InNode>
        void for_each_in_node()
        {
            if constexpr (tavl::is_empty_node_v<InNode>)
            {
                return;
            }
            else
            {
                using in_node = typename InNode::key;
                for_each_in_node<Out, Edge, typename InNode::left>();
                add_edge_to_map<Out, Edge, in_node>();
                for_each_in_node<Out, Edge, typename InNode::right>();
            }
        }
        template <typename Out, typename EdgeNode>
        void for_each_edge()
        {
            if constexpr (tavl::is_empty_node_v<EdgeNode>)
            {
                return;
            }
            else
            {
                using current_symbol = typename EdgeNode::key;
                for_each_edge<Out, typename EdgeNode::left>();
                for_each_in_node<Out,
                                 current_symbol,
                                 typename EdgeNode::value>();
                for_each_edge<Out, typename EdgeNode::right>();
            }
        }
        template <typename MapNode>
        void for_each_out_node()
        {
            if constexpr (tavl::is_empty_node_v<MapNode>)
            {
                return;
            }
            else
            {
                using out_node = typename MapNode::key;
                for_each_out_node<typename MapNode::left>();
                for_each_edge<out_node, typename MapNode::value>();
                for_each_out_node<typename MapNode::right>();
            }
        }

    public:
        lr_parsing()
        {
            for_each_out_node<transition_map>();
        }

    private:
        std::unordered_map<std::type_index,
                           std::unordered_map<impl::lr_stack_symbol,
                                              std::type_index,
                                              impl::lr_stack_symbol::hash>>
            _parsing_table;
        std::unordered_map<std::type_index,
                           std::unique_ptr<impl::lr_stack_symbol>>
            _stack_table;
    };
} // namespace Compiler
#endif
