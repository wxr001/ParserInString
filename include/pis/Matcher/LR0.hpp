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

#ifndef PARSER_IN_STRING_MATCHER_LR0
#define PARSER_IN_STRING_MATCHER_LR0

#include "LR_common.hpp"
#include "TAVL.hpp"
#include "parser_base.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tfuncs.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "pis/Common/Utils.hpp"
#include "pis/Lexer/Lexer.hpp"
#include "pis/Matcher/FirstFollow.hpp"
#include "pis/Rules/CodeToRules.hpp"
#include "pis/Rules/EBNF.hpp"
#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace pis
{
    // note: item set is type_list<> not tavl_node<>
    template <typename Target, typename PreDef, typename PostDef>
    struct lr0_single_item:
        impl::lr_single_item<Target, PreDef, PostDef, type_list<>>
    {};
    template <typename Items, bool HasConflict>
    struct conflict_check
    {
        using type = Items;
    };
    template <typename Items>
    struct conflict_check<Items, true>
    {};
    namespace lr0_impl
    {
        using namespace impl;
        // helping method
        template <typename Item>
        using next_item_impl = lr0_single_item<
            typename Item::target,
            type_list_push_back<typename Item::pre_dot,
                                type_list_top<typename Item::post_dot>>,
            type_list_tail<typename Item::post_dot>>;
        template <typename Item>
        inline constexpr bool is_last_item =
            std::is_same_v<typename Item::post_dot, type_list<>>;
        template <typename Item>
        using next_item = typename std::conditional_t<
            is_last_item<Item>,
            identity<no_such_item>,
            lazy_template<lr0_impl::next_item_impl, Item>>::type;
        // closure function
        template <typename Rules, typename Set>
        struct closure
        {
            template <typename Item>
            static constexpr bool is_expandable = std::is_same_v<
                typename type_list_top<typename Item::post_dot>::first_type,
                Impl::meta_identifier_flag>;
            template <typename Target>
            struct create_item_for
            {
                template <typename Def>
                using type =
                    identity<lr0_single_item<Target, type_list<>, Def>>;
            };
            template <
                typename Item,
                bool NT = std::is_same_v<
                    typename type_list_top<typename Item::post_dot>::first_type,
                    Impl::meta_identifier_flag>>
            struct do_expand
            {
                using defs_to_expand = typename tavl::tavl_find_t<
                    Rules,
                    typename type_list_top<
                        typename Item::post_dot>::second_type>::value;
                using item_list = type_list_for_each<
                    defs_to_expand,
                    create_item_for<typename type_list_top<
                        typename Item::post_dot>::second_type>::template type>;
                using type = item_list;
            };
            template <typename Item>
            struct do_expand<Item, false>
            {
                using type = type_list<Item>;
            };
            template <typename Item,
                      bool empty =
                          std::is_same_v<typename Item::post_dot, type_list<>>>
            struct do_expand_nonempty: public do_expand<Item>
            {};
            template <typename Item>
            struct do_expand_nonempty<Item, true>
            {
                using type = type_list<Item>;
            };
            template <typename Item>
            using do_expand_t = do_expand_nonempty<Item>;
            template <typename Last>
            struct expand_for_each_item
            {
                using this_new = tlist_set_not_in_s<
                    typename Last::second_type,
                    tlist_set_for_each<typename Last::first_type,
                                       do_expand_t,
                                       tlist_set_union_impl,
                                       type_list<>>>;
                using type =
                    type_pair<this_new,
                              type_list_push_front_m<typename Last::second_type,
                                                     this_new>>;
            };
            template <typename Old, typename New>
            struct stop_until
            {
                static constexpr bool value =
                    std::is_same_v<typename Old::first_type, type_list<>>;
            };
            using type =
                typename do_until<stop_until,
                                  expand_for_each_item,
                                  type_pair<Set, Set>>::type::second_type;
        };
        template <typename Rules, typename Set>
        using closure_t = typename closure<Rules, Set>::type;
        // GOTO(Set, Symbol)
        template <typename Set, typename Symbol>
        struct expand
        {
            template <typename I>
            struct for_each_item
            {
                using type = no_such_item;
            };
            template <typename T, typename Pre, typename... Posts>
            struct for_each_item<
                lr0_single_item<T, Pre, type_list<Symbol, Posts...>>>
            {
                using type = next_item<
                    lr0_single_item<T, Pre, type_list<Symbol, Posts...>>>;
            };
            template <typename B, typename C>
            struct item_set_merger
            {
                using type = type_list_push_back<B, C>;
            };
            template <typename B>
            struct item_set_merger<B, no_such_item>
            {
                using type = B;
            };
            using type = tlist_set_for_each<Set,
                                            for_each_item,
                                            item_set_merger,
                                            type_list<>>;
        };
        // GOTO(Set, Symbol)
        template <typename Set, typename Symbol>
        using expand_t = typename expand<Set, Symbol>::type;
        template <typename Rules>
        struct generate_sets
        {
            static_assert(
                !std::is_same_v<
                    type_list_top<tavl_find_value<Rules, dummy_target>>,
                    type_list<>>,
                "missing starting rule");
            using starting_items = closure_t<
                Rules,
                type_list<lr0_single_item<
                    dummy_target,
                    type_list<>,
                    type_list_top<tavl_find_value<Rules, dummy_target>>>>>;

            template <typename Item, bool is_end = is_last_item<Item>>
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
                using maybe_next_trans =
                    type_list_remove<tlist_set_for_each<Items,
                                                        try_get_next_item_t,
                                                        tlist_set_try_insert_2,
                                                        type_list<>>,
                                     no_such_item>;
                using type = type_pair<Items, maybe_next_trans>;
            };
            template <typename SetsTrans, typename P>
            struct add_trans;
            template <typename SetsTrans, typename P>
            using add_trans_2 = add_trans<SetsTrans, P>;
            template <typename SetTrans, typename Items, typename Labels>
            struct add_trans_impl;
            template <typename SetTrans, typename Items, typename Labels>
            using add_trans_impl_t =
                typename add_trans_impl<SetTrans, Items, Labels>::type;
            template <typename Sets,
                      typename Trans,
                      typename Items,
                      typename Labels>
            struct add_trans<type_pair<Sets, Trans>, type_pair<Items, Labels>>
            {
                using type =
                    add_trans_impl_t<type_pair<Sets, Trans>, Items, Labels>;
            };
            template <typename Trans,
                      typename Items,
                      typename Label,
                      typename Target,
                      bool Exist = tavl::tavl_contain_v<Trans, Items>>
            struct add_trans_to_map
            {
                using type = tavl::tavl_update_t<
                    Trans,
                    Items,
                    tavl::tavl_update_t<
                        typename tavl::tavl_find_t<Trans, Items>::value,
                        Label,
                        Target>>;
            };
            template <typename Trans,
                      typename Items,
                      typename Label,
                      typename Target>
            struct add_trans_to_map<Trans, Items, Label, Target, false>
            {
                using type = tavl::
                    tavl_insert_t<Trans, Items, tavl::kv_pair<Label, Target>>;
            };
            template <typename SetTrans, typename Items, typename Labels>
            struct add_trans_impl
            {
                using label     = type_list_top<Labels>;
                using target    = closure_t<Rules, expand_t<Items, label>>;
                using sets      = typename SetTrans::first_type;
                using trans_map = typename SetTrans::second_type;
                using added_map =
                    typename add_trans_to_map<trans_map, Items, label, target>::
                        type;
                using type = typename add_trans_impl<
                    type_pair<set_try_insert_t<sets, target>, added_map>,
                    Items,
                    type_list_tail<Labels>>::type;
            };
            template <typename TransMap, typename Items>
            struct add_trans_impl<TransMap, Items, type_list<>>
            {
                using type = TransMap;
            };
            template <typename SetsTrans>
            struct generate_item_sets
            {
                using sets = typename SetsTrans::first_type;
                using type =
                    tavl::tavl_for_each_middle_order_t<sets,
                                                       for_each_item_set,
                                                       add_trans_2,
                                                       SetsTrans>;
            };
            using type = typename do_until<
                tavl::tavl_is_same,
                generate_item_sets,
                type_pair<tavl::kv_pair<starting_items>,
                          tavl::kv_pair<starting_items, tavl::empty_node>>>::
                type;
        };
        template <typename Rules>
        using generate_item_sets =
            typename generate_sets<Rules>::type::first_type;
        template <typename Rules>
        using generate_trans_graph =
            typename generate_sets<Rules>::type::second_type;
        template <typename Rules>
        using generate_starting_item_set =
            typename generate_sets<Rules>::starting_items;
        template <typename Items>
        struct contain_reduce_item;
        template <typename... Items>
        struct contain_reduce_item<type_list<Items...>>
        {
            static constexpr bool value = (... || is_last_item<Items>);
        };
        template <typename Items>
        inline constexpr bool contain_reduce_item_v =
            contain_reduce_item<Items>::value;
        template <typename CurrentItem,
                  typename OtherItems,
                  bool shift_or_reduce = !is_last_item<CurrentItem>>
        struct conflict_check_impl;
        template <typename Item, typename Items>
        struct conflict_check_impl<Item, Items, true>
        {
            // shift/reduce conflict
            static constexpr bool value = !contain_reduce_item_v<Items>;
        };
        template <typename Item, typename Items>
        struct conflict_check_impl<Item, Items, false>
        {
            // reduce/reduce conflict or shift/reduce conflict
            static constexpr bool value = std::is_same_v<Items, type_list<>>;
        };
        template <typename Items>
        struct validity_check;
        template <typename Item, typename... Others>
        struct validity_check<type_list<Item, Others...>>
        {
            static constexpr bool value =
                conflict_check_impl<Item, type_list<Others...>>::value;
        };
        template <>
        struct validity_check<type_list<>>
        {
            static constexpr bool value = true;
        };
        template <typename Items>
        using validity_check_t =
            typename conflict_check<Items, !validity_check<Items>::value>::type;
    } // namespace lr0_impl
    using lr0_stack_symbol = impl::lr_stack_symbol;
    using impl::make_nonterminal_symbol;
    using impl::make_terminal_symbol;
    class lr0_stack;
    class lr0_state
    {
    public:
        virtual std::string target() const
        {
            return {};
        }
        virtual bool is_accepting_state() const noexcept
        {
            return false;
        }
        virtual std::vector<std::any> do_reduce(lr0_stack& /*stack*/) const
        {
            return {};
        }
        virtual std::string dump() const
        {
            return "";
        }

    protected:
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
                str +=
                    "\"" + to_string(typename current::second_type{}) + "\", ";
            }
            else if constexpr (std::is_same_v<typename current::first_type,
                                              Impl::meta_identifier_flag>)
            {
                str += "<" + to_string(typename current::second_type{}) + ">, ";
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
    };
    class lr0_stack
    {
        // structure of STACK:
        // init_state |state symbol value| |state symbol value| ...
    public:
        lr0_stack(std::type_index init_state): _values({init_state}) {}
        void push(std::type_index state, lr0_stack_symbol type, std::any value)
        {
            _values.emplace_back(state);
            _values.emplace_back(std::move(type));
            _values.emplace_back(std::move(value));
        }
        std::vector<std::any> pop(std::size_t n)
        {
            if (3 * n > _values.size())
            {
                throw std::out_of_range("Not enough items to pop");
            }
            // move datas from stack to a new vector

            std::vector<std::any> datas(n);
            std::size_t           i = 0;
            for (auto begit = std::prev(_values.end(), 3 * n),
                      endit = _values.end();
                 begit < endit;
                 ++begit)
            {
                assert(begit->type() == typeid(std::type_index));
                ++begit;
                assert(begit->type() == typeid(lr0_stack_symbol));
                ++begit;
                datas[i++] = *begit;
            }
            _values.erase(std::prev(_values.end(), 3 * n), _values.end());
            return datas;
        }
        std::type_index top_state() const
        {
            if (_values.size() == 1)
            {
                assert(_values.back().type() == typeid(std::type_index));
                return std::any_cast<std::type_index>(_values.back());
            }
            assert(_values[_values.size() - 3].type() ==
                   typeid(std::type_index));
            return std::any_cast<std::type_index>(_values[_values.size() - 3]);
        }
        const lr0_stack_symbol& top_symbol() const
        {
            assert(_values[_values.size() - 2].type() ==
                   typeid(lr0_stack_symbol));
            return std::any_cast<const lr0_stack_symbol&>(
                _values[_values.size() - 2]);
        }
        const std::any& top_value() const
        {
            return _values.back();
        }
        operator parser_interface()
        {
            return [&]() -> std::vector<std::any>& {
                return _values;
            };
        }

    private:
        std::vector<std::type_index>  _states;
        std::vector<lr0_stack_symbol> _types;
        std::vector<std::any>         _values;
    };
    template <typename ItemSet,
              bool accepting_state = lr0_impl::contain_reduce_item_v<ItemSet>>
    class lr0_state_impl;
    template <typename ItemSet>
    class lr0_state_impl<ItemSet, true>: public lr0_state
    {
        static_assert(type_list_size<ItemSet> == 1, "reduce/reduce conflict");
        using reduce_item = type_list_top<ItemSet>;
        using definition  = typename reduce_item::definition;

    public:
        std::string target() const override
        {
            return to_string(typename reduce_item::target{});
        }
        bool is_accepting_state() const noexcept override
        {
            return true;
        }
        std::vector<std::any> do_reduce(lr0_stack& stack) const override
        {
            return stack.pop(type_list_size<definition>);
        }
        std::string dump() const override
        {
            return dump_single_item<reduce_item>();
        }
    };
    template <typename ItemSet>
    class lr0_state_impl<ItemSet, false>: public lr0_state
    {
    public:
        bool is_accepting_state() const noexcept override
        {
            return false;
        }
        std::string dump() const override
        {
            std::string result;
            do_dump(ItemSet{}, result);
            return result;
        }

    protected:
        template <typename... Item>
        void do_dump(type_list<Item...>, std::string& result) const
        {
            (dump_single_item<Item>(result), ...);
        }
    };
    template <typename Rules,
              typename Start,
              template <typename>
              typename Lexer>
    class lr_parsing<Rules, Start, 0, Lexer>
    {
    private:
        using lexer_t     = Lexer<Rules>;
        using lexer_rules = typename lexer_t::type;
        // add dummy rule S' = S to the rule-set
        using rules = impl::add_dummy_rule_with_eof_tag<lexer_rules, Start>;
        // generate grammar graph(aka automaton)
        using item_sets   = lr0_impl::generate_item_sets<rules>;
        using trans_graph = lr0_impl::generate_trans_graph<rules>;
        static_assert(!tavl::is_empty_node_v<item_sets> &&
                          !tavl::is_empty_node_v<trans_graph>,
                      "No Valid States");
        // check for LR(0)-validity
        template <typename Items, typename>
        struct check_validity_for_each_items
        {
            using type = tavl::kv_pair<lr0_impl::validity_check_t<Items>>;
        };
        using validity_check_t =
            tavl::tavl_for_each_t<item_sets,
                                  check_validity_for_each_items,
                                  tavl::tavl_union_3,
                                  tavl::empty_node>;
        static_assert(tavl::tavl_is_same_v<item_sets, validity_check_t>,
                      "Grammar is not a LR(0)-grammar");

        using lr0_parsing_table =
            std::unordered_map<std::type_index,
                               std::unordered_map<lr0_stack_symbol,
                                                  std::type_index,
                                                  lr0_stack_symbol::hash>>;
        using lr0_state_table =
            std::unordered_map<std::type_index, std::unique_ptr<lr0_state>>;
        template <typename Items>
        void fill_states()
        {
            if constexpr (tavl::is_empty_node_v<Items>)
            {
                return;
            }
            else
            {
                fill_states<typename Items::left>();
                _states.emplace(
                    std::type_index{typeid(typename Items::key)},
                    std::make_unique<lr0_state_impl<typename Items::key>>());
                fill_states<typename Items::right>();
            }
        }
        template <typename From, typename Labels>
        void fill_trans_label()
        {
            if constexpr (tavl::is_empty_node_v<Labels>)
            {
                return;
            }
            else
            {
                fill_trans_label<From, typename Labels::left>();
                _trans_map[std::type_index{typeid(From)}].try_emplace(
                    lr0_stack_symbol{typename Labels::key{}},
                    std::type_index{typeid(typename Labels::value)});
                assert(_states.find(std::type_index{
                           typeid(typename Labels::value)}) != _states.end() &&
                       "missing state");
                fill_trans_label<From, typename Labels::right>();
            }
        }
        template <typename Trans>
        void fill_trans_from_state()
        {
            if constexpr (tavl::is_empty_node_v<Trans>)
            {
                return;
            }
            else
            {
                // left
                fill_trans_from_state<typename Trans::left>();
                // current
                assert(_states.find(std::type_index{
                           typeid(typename Trans::key)}) != _states.end() &&
                       "missing state");
                fill_trans_label<typename Trans::key, typename Trans::value>();
                // right
                fill_trans_from_state<typename Trans::right>();
            }
        }

    public:
        using start_items   = lr0_impl::generate_starting_item_set<rules>;
        using parsing_table = trans_graph;
        lr_parsing()
        {
            fill_states<item_sets>();
            fill_trans_from_state<trans_graph>();
        }
        void add_lexer_listener(const std::string& name,
                                lexer_listener_t   listener)
        {
            _lexer_listeners[name] = listener;
        }
        void add_parser_listener(const std::string& name,
                                 parser_listener_t  listener)
        {
            _parser_listeners[name] = listener;
        }
        std::string dump() const
        {
            std::string result;
            result += "States: \n";
            for (const auto& [type, ptr] : _states)
            {
                result +=
                    n2hexstr(type.hash_code()) + ": \n" + ptr->dump() + "\n";
            }
            result += "\n\n\nTransition Map: \n";
            for (const auto& [beg_item, trans] : _trans_map)
            {
                for (const auto& [symbol, end_item] : trans)
                {
                    result += n2hexstr(beg_item.hash_code()) + " ---" +
                              symbol.dump() + "---> " +
                              n2hexstr(end_item.hash_code()) + "\n";
                }
            }
            return result;
        }
        std::any parse(std::istream&      input,
                       const std::string& skip = " \f\n\r\t\v")
        {
            PRINT_LOG("\n" + dump());
            _stack.reset(new lr0_stack{std::type_index{typeid(start_items)}});
            _lexer.reset(new lexer_t(input, *_stack, _lexer_listeners, skip));
            assert(_stack && _lexer && "failed to init LR(0) parser");
            unsigned char eof_cnt = 0;
            do
            {
                const lr0_stack_symbol* current_symbol;
                bool                    to_be_deleted = false;
                std::any                value;
                if (lr0_state* current_state = top_state();
                    current_state->is_accepting_state())
                {
                    auto target    = current_state->target();
                    current_symbol = new lr0_stack_symbol(true, target);
                    to_be_deleted  = true;
                    value          = call_listener(target,
                                          current_state->do_reduce(*_stack));
                }
                else
                {
                    auto [types, str] = _lexer->read_any();
                    if (str.empty() && _lexer->eof())
                    {
                        if (eof_cnt == 1)
                        {
                            // end of parsing
                            // no more data
                            break;
                        }
                        using namespace std::string_literals;
                        types = {"\0"s};
                        str   = "\0"s;
                        ++eof_cnt;
                    }
                    else if (str.empty())
                    {
                        // error no more tokens to read
                        throw std::logic_error("no more tokens");
                    }
                    auto out_trans = _trans_map.at(_stack->top_state());
                    bool found     = false;
                    for (const auto& [symbol, target_state] : out_trans)
                    {
                        if (symbol.is_match(types))
                        {
                            found          = true;
                            current_symbol = new lr0_stack_symbol(symbol);
                            value          = std::make_any<std::string>(str);
                            break;
                        }
                    }
                    if (!found)
                    {
                        // error unknown symbol
                        throw std::logic_error("unknown symbol: " + str);
                    }
                }
                assert(current_symbol);
                if (auto next_state =
                        find_next_state(_stack->top_state(), *current_symbol);
                    next_state)
                {
                    _stack->push(
                        next_state.value(), *current_symbol, std::move(value));
                }
                else if (*current_symbol == make_nonterminal_symbol(to_string(
                                                impl::dummy_target{})))
                {
                    assert(value.type() == typeid(std::vector<std::any>));
                    return std::any_cast<std::vector<std::any>>(value)[0];
                }
                else
                {
                    // error invalid input
                    throw std::logic_error("invalid input");
                }
                if (current_symbol)
                {
                    delete current_symbol;
                }
            } while (_stack->top_symbol() !=
                     make_nonterminal_symbol(to_string(impl::dummy_target{})));
            if (_stack->top_symbol() !=
                make_nonterminal_symbol(to_string(impl::dummy_target{})))
            {
                // error missing symbol
            }
            if (!_lexer->eof())
            {
                // error unknown symbol
            }
            return _stack->top_value();
        }

    private:
        std::string dump_itemset(std::type_index itemset) const
        {
            if (auto it = _states.find(itemset); it != _states.end())
            {
                return it->second->dump();
            }
            PRINT_LOG("ERROR: missing itemset<" +
                      n2hexstr(itemset.hash_code()) + ">");
            return "";
        }
        lr0_state* top_state() const
        {
            return _states.at(_stack->top_state()).get();
        }
        std::any call_listener(const std::string&    target,
                               std::vector<std::any> tokens)
        {
            if (auto it = _parser_listeners.find(target);
                it != _parser_listeners.end())
            {
                return it->second(std::move(tokens), *_lexer, target);
            }
            return parser_listener_dummy(std::move(tokens), *_lexer, target);
        }
        std::optional<std::type_index>
            find_next_state(std::type_index         current_state,
                            const lr0_stack_symbol& symbol)
        {
            if (auto it = _trans_map.find(current_state);
                it != _trans_map.end())
            {
                if (auto result_it = it->second.find(symbol);
                    result_it != it->second.end())
                {
                    return result_it->second;
                }
            }
            return std::nullopt;
        }
        lr0_parsing_table                                 _trans_map;
        lr0_state_table                                   _states;
        impl::parser_listener_map_t                       _parser_listeners;
        std::unordered_map<std::string, lexer_listener_t> _lexer_listeners;
        std::unique_ptr<lr0_stack>                        _stack;
        std::unique_ptr<lexer_t>                          _lexer;
    };
} // namespace pis
namespace tavl
{
    template <typename Target1,
              typename Pre1,
              typename Post1,
              typename Target2,
              typename Pre2,
              typename Post2>
    struct compare<pis::lr0_single_item<Target1, Pre1, Post1>,
                   pis::lr0_single_item<Target2, Pre2, Post2>>
    {
        static constexpr int value = compare_v<Target1, Target2> == 0 ?
                                         compare_v<Pre1, Pre2> == 0 ?
                                         compare_v<Post1, Post2> :
                                         compare_v<Pre1, Pre2> :
                                         compare_v<Target1, Target2>;
    };
} // namespace tavl
#endif
