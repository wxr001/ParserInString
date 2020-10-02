// ParserInString -*- C++ -*-

// Copyright (C) 2018-2020 Scott Brown

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

#ifndef PARSER_IN_STRING_MATCHER_LL
#define PARSER_IN_STRING_MATCHER_LL

#include "Common/Identity.hpp"
#include "Common/TAVLHelper.hpp"
#include "Common/Tfuncs.hpp"
#include "Common/Tlist.hpp"
#include "Common/Tstring.hpp"
#include "Common/Utils.hpp"
#include "Common/log.hpp"
#include "Common/runtime.hpp"
#include "FirstFollow.hpp"
#include "Lexer/Lexer.hpp"
#include "Matcher/lookahead.hpp"
#include "Rules/CodeToRules.hpp"
#include "Rules/EBNF.hpp"
#include "TAVL.hpp"
#include "parser_base.hpp"
#include <any>
#include <functional>
#include <istream>
#include <list>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
namespace Compiler
{
    namespace Impl
    {
        struct ll_table_flag
        {};
    } // namespace Impl
    template <typename First, typename Target, typename Def, typename Follows>
    struct ll_table_item_tmp
    {
        using key        = First;
        using production = type_pair<Target, Def>;
        using sets       = Follows;
    };
    template <typename Key, typename State, typename ResultItem>
    struct ll_table_item
    {
        using state = State;
        using key   = Key;
        using item  = ResultItem;
    };
    template <typename T>
    struct single_yi_set_to_list
    {
        template <typename L, typename K>
        struct do_merge
        {
            using type = type_list_push_front<L, K>;
        };
        using type = tavl::
            tavl_for_each_middle_order_t<T, get_key, do_merge, type_list<>>;
    };
    template <typename... Ts>
    struct single_yi_set_to_list<type_list<Ts...>>
    {
        using type = type_list<Ts...>;
    };
    template <typename T>
    using single_yi_set_to_list_t = typename single_yi_set_to_list<T>::type;
    template <typename T>
    struct yi_to_list
    {
        using type = type_list_for_each<T, single_yi_set_to_list>;
    };
    template <typename T>
    using yi_to_list_t = typename yi_to_list<T>::type;
    template <typename T>
    using yis_to_list_t = type_list_for_each<T, yi_to_list>;
    struct ll_table_interface
    {
        virtual std::vector<std::any> parse_this_rule()
        {
            return {};
        }
        virtual bool internal_rule() const
        {
            return false;
        }
        virtual std::string dump_this() const
        {
            return "";
        }
    };
    template <typename L>
    class ll_table_base
    {
        static_assert(std::is_constructible_v<
                          L,
                          std::istream&,
                          parser_interface,
                          std::unordered_map<std::string, lexer_listener_t>&,
                          std::string> &&
                          std::is_base_of_v<lexer_interface, L>,
                      "invalid lexer type");

    public:
        using state_map =
            std::unordered_multimap<std::type_index, ll_table_interface*>;
        using parsing_table = std::unordered_map<
            std::type_index,
            std::unordered_map<std::shared_ptr<lookahead_symbol>,
                               ll_table_interface*>>;
        ll_table_base(state_map&                   states,
                      parsing_table&               table,
                      impl::parser_listener_map_t& listeners):
            _states(states),
            _table(table),
            _listeners(listeners)
        {}
        virtual ~ll_table_base() = default;
        // multi-rules interface
        template <typename T>
        std::vector<std::any>
            try_parse(const std::vector<std::vector<std::string>>& lookahead)
        {
            PRINT_LOG(print_lookahead(lookahead));
            if (auto next = get_item<T>(lookahead); next)
            {
                return next->parse_this_rule();
            }
            PRINT_LOG("ERROR: no matching rules " + print_lookahead(lookahead));
            // error no matching rules
            return {};
        }
        template <typename T>
        bool is_internal(
            const std::vector<std::vector<std::string>>& lookahead) const
        {
            if (auto next = get_item<T>(lookahead); next)
            {
                return next->internal_rule();
            }
            return false;
        }
        template <typename T>
        std::pair<std::vector<std::any>, bool> parse_with_internal_flag(
            const std::vector<std::vector<std::string>>& lookahead)
        {
            if (auto next = get_item<T>(lookahead); next)
            {
                return std::make_pair(next->parse_this_rule(),
                                      next->internal_rule());
            }
            PRINT_LOG("ERROR: next step not found");
            return std::make_pair(std::vector<std::any>{}, false);
        }
        std::string dump() const
        {
            std::string result;
            for (const auto& [type, map] : _table.get())
            {
                for (const auto& [look, ptr] : map)
                {
                    result += n2hexstr(type.hash_code()) + ", " +
                              look->to_string() + ":" + ptr->dump_this() + "\n";
                }
            }
            return result;
        }
        // lexer interface
        void set_lexer(
            std::istream&                                      input,
            std::unordered_map<std::string, lexer_listener_t>& listeners,
            const std::string& skip = " \f\n\r\t\v")
        {
            _lexer.reset(new L{input,
                               [&]() -> std::vector<std::any>& {
                                   return _stack.back();
                               },
                               listeners,
                               skip});
        }
        void set_lexer(std::shared_ptr<L> lexer)
        {
            _lexer = lexer;
        }
        L& get_lexer()
        {
            return *_lexer;
        }
        const L& get_lexer() const
        {
            return *_lexer;
        }
        bool has_lexer() const
        {
            return !!_lexer;
        }

    protected:
        std::vector<std::any>& new_result() const
        {
            _stack.push_back({});
            PRINT_LOG("push-" + n2hexstr(reinterpret_cast<ptrdiff_t>(this)) +
                      "-" + std::to_string(_stack.size()));
            return _stack.back();
        }
        std::vector<std::any> pop_result() const
        {
            std::vector<std::any> result = _stack.back();
            _stack.pop_back();
            PRINT_LOG("pop-" + n2hexstr(reinterpret_cast<ptrdiff_t>(this)) +
                      "-" + std::to_string(_stack.size()));
            return result;
        }
        template <typename Target>
        std::any call_listener(std::vector<std::any> tokens)
        {
            auto target = to_string(Target{});
            PRINT_LOG("calling: " + target);
            if (auto it = _listeners.get().find(target);
                it != _listeners.get().end())
            {
                return it->second(std::move(tokens), get_lexer(), target);
            }
            PRINT_LOG("calling default: " + target);
            return parser_listener_dummy(
                std::move(tokens), get_lexer(), target);
        }

    private:
        template <typename T>
        ll_table_interface* get_item(
            const std::vector<std::vector<std::string>>& lookahead) const
        {
            using namespace std::string_literals;
            auto lookahead_map = _table.get().at(std::type_index{typeid(T)});
            ll_table_interface* empty_trans = nullptr;
            for (const auto& [look, ptr] : lookahead_map)
            {
                if (look->is_same(lookahead))
                {
                    return ptr;
                }
                if (look->is_empty())
                {
                    empty_trans = ptr;
                }
            }
            PRINT_LOG("Using @EMPTY rule: " +
                      (empty_trans ? "Valid"s : "NONE"s));
            return empty_trans;
        }
        std::reference_wrapper<state_map>                   _states;
        std::reference_wrapper<parsing_table>               _table;
        std::shared_ptr<L>                                  _lexer;
        mutable std::list<std::vector<std::any>>            _stack;
        std::reference_wrapper<impl::parser_listener_map_t> _listeners;
    };
    template <typename Elem, typename L>
    class ll_table_impl:
        virtual public ll_table_base<L>,
        public ll_table_interface
    {
        using target = typename Elem::item::production::first_type;
        using defs   = typename Elem::item::production::second_type;
        static constexpr unsigned K = type_list_size<typename Elem::key>;

    public:
        using state_map     = typename ll_table_base<L>::state_map;
        using parsing_table = typename ll_table_base<L>::parsing_table;
        ll_table_impl(state_map&                   states,
                      parsing_table&               table,
                      impl::parser_listener_map_t& listeners):
            ll_table_base<L>(states, table, listeners)
        {
            states.emplace(std::type_index{typeid(typename Elem::state)}, this);
            table[std::type_index{typeid(typename Elem::state)}]
                 [std::make_shared<lookahead_list<typename Elem::key>>()] =
                     this;
        }
        std::string dump_this() const override
        {
            std::string result = to_string(target{}) + " = ";
            do_dump<defs>(result);
            return result.substr(0, result.size() - 2);
        }
        std::vector<std::any> parse_this_rule() override
        {
            PRINT_LOG("Parsing " + dump_this());
            for_each_term<defs>(ll_table_base<L>::new_result());
            PRINT_LOG("Finish Parsing " + dump_this());
            if (!internal_rule())
            {
                return {ll_table_base<L>::template call_listener<target>(
                    std::move(ll_table_base<L>::pop_result()))};
            }
            return ll_table_base<L>::pop_result();
        }
        bool internal_rule() const override
        {
            return first_character_s<target>::value == '@';
        }

    private:
        template <typename Defs>
        std::enable_if_t<!std::is_same_v<Defs, type_list<>>>
            do_dump(std::string& str) const
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
                                              Impl::ll_table_flag>)
            {
                str += "<" + type_info_str<typename current::second_type>() +
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
            do_dump<next>(str);
        }
        template <typename Defs>
        std::enable_if_t<std::is_same_v<Defs, type_list<>>>
            do_dump(std::string&) const
        {
            // finish parsing
            return;
        }
        template <typename Defs>
        std::enable_if_t<!std::is_same_v<Defs, type_list<>>>
            for_each_term(std::vector<std::any>& result)
        {
            using current = type_list_top<Defs>;
            using next    = type_list_tail<Defs>;
            auto& lexer   = ll_table_base<L>::get_lexer();
            if constexpr (std::is_same_v<typename current::first_type,
                                         Impl::terminal_string_flag>)
            {
                PRINT_LOG("Try reading " +
                          to_string(typename current::second_type{}));
                std::string terminal =
                    lexer.template readNext<typename current::second_type>();
                if (terminal.empty())
                {
                    // error bad symbol
                    PRINT_LOG("ERROR: bad symbol \"" +
                              to_string(typename current::second_type{}) +
                              "\"");
                    return;
                }
                result.emplace_back(terminal);
            }
            else if constexpr (std::is_same_v<typename current::first_type,
                                              Impl::ll_table_flag>)
            {
                auto lookahead = lexer.template lookahead<K>();
                PRINT_LOG("Next table " +
                          type_info_str<typename current::second_type>() +
                          " with lookahead " + print_lookahead(lookahead));
                if (const auto& [rules_result, internal] =
                        ll_table_base<L>::template parse_with_internal_flag<
                            typename current::second_type>(lookahead);
                    internal)
                {
                    PRINT_LOG("Internal rule");
                    // internal rule does not have a listener
                    result.reserve(result.size() + rules_result.size());
                    result.insert(
                        result.end(), rules_result.begin(), rules_result.end());
                }
                else
                {
                    PRINT_LOG("Normal rule");
                    assert(rules_result.size() == 1);
                    result.push_back(std::move(rules_result[0]));
                }
            }
            else if constexpr (std::is_same_v<typename current::first_type,
                                              Impl::empty_sequence_flag>)
            {
                PRINT_LOG("Empty sequence");
                result.emplace_back();
            }
            else if constexpr (std::is_same_v<typename current::first_type,
                                              Impl::special_sequence_flag>)
            {
                PRINT_LOG("Speical Sequence");
                assert(false);
                // special sequence here
            }
            else
            {
                PRINT_LOG("Unsupported Type");
                assert(false);
                // unsupported type
            }
            for_each_term<next>(result);
        }
        template <typename Defs>
        std::enable_if_t<std::is_same_v<Defs, type_list<>>>
            for_each_term(std::vector<std::any>&)
        {
            // finish parsing
            return;
        }
    };
    template <typename Elem, typename L, typename R, typename Lexer>
    class ll_table: private L, private R, public ll_table_impl<Elem, Lexer>
    {
        // merge two branches
        //     Elem
        //    /    \
        //   /      \
        //  L        R
        using impl_t = ll_table_impl<Elem, Lexer>;
        using target = typename Elem::item::production::first_type;
        using defs   = typename Elem::item::production::second_type;
        static constexpr unsigned K = type_list_size<typename Elem::key>;

    public:
        using state_map     = typename ll_table_base<Lexer>::state_map;
        using parsing_table = typename ll_table_base<Lexer>::parsing_table;
        static_assert(std::is_constructible_v<L,
                                              state_map&,
                                              parsing_table&,
                                              impl::parser_listener_map_t&> &&
                      std::is_constructible_v<R,
                                              state_map&,
                                              parsing_table&,
                                              impl::parser_listener_map_t&>);
        ll_table(state_map&                   states,
                 parsing_table&               table,
                 impl::parser_listener_map_t& listeners):
            ll_table_base<Lexer>(states, table, listeners),
            L(states, table, listeners),
            R(states, table, listeners),
            ll_table_impl<Elem, Lexer>(states, table, listeners)
        {}
    };
    template <typename Elem, typename L, typename Lexer>
    class ll_table<Elem, L, L, Lexer>:
        private L,
        public ll_table_impl<Elem, Lexer>
    {
        // merge two branches
        //     Elem
        //    /    \
        //   /      \
        //  L        R
        using impl_t = ll_table_impl<Elem, Lexer>;
        using target = typename Elem::item::production::first_type;
        using defs   = typename Elem::item::production::second_type;
        static constexpr unsigned K = type_list_size<typename Elem::key>;

    public:
        using state_map     = typename ll_table_base<Lexer>::state_map;
        using parsing_table = typename ll_table_base<Lexer>::parsing_table;
        static_assert(std::is_constructible_v<L,
                                              state_map&,
                                              parsing_table&,
                                              impl::parser_listener_map_t&>);
        ll_table(state_map&                   states,
                 parsing_table&               table,
                 impl::parser_listener_map_t& listeners):
            ll_table_base<Lexer>(states, table, listeners),
            L(states, table, listeners),
            ll_table_impl<Elem, Lexer>(states, table, listeners)
        {}
    };
    template <typename Elem, typename L>
    class ll_table<Elem, void, void, L>: public ll_table_impl<Elem, L>
    {
        // table content without branch
        using impl_t = ll_table_impl<Elem, L>;

    public:
        using state_map     = typename ll_table_base<L>::state_map;
        using parsing_table = typename ll_table_base<L>::parsing_table;
        using elem_type     = Elem;
        ll_table(state_map&                   states,
                 parsing_table&               table,
                 impl::parser_listener_map_t& listeners):
            ll_table_base<L>(states, table, listeners),
            ll_table_impl<Elem, L>(states, table, listeners)
        {}
    };
    template <typename Elem, typename L>
    class ll_table<Elem,
                   ll_table<void, void, void, L>,
                   ll_table<void, void, void, L>,
                   L>: public ll_table_impl<Elem, L>
    {
        // table content without branch
        using impl_t = ll_table_impl<Elem, L>;

    public:
        using state_map     = typename ll_table_base<L>::state_map;
        using parsing_table = typename ll_table_base<L>::parsing_table;
        using elem_type     = Elem;
        ll_table(state_map&                   states,
                 parsing_table&               table,
                 impl::parser_listener_map_t& listeners):
            ll_table_base<L>(states, table, listeners),
            ll_table_impl<Elem, L>(states, table, listeners)
        {}
    };
    template <typename L>
    class ll_table<void, void, void, L>: virtual public ll_table_base<L>
    {
        // do nothing

    public:
        using state_map     = typename ll_table_base<L>::state_map;
        using parsing_table = typename ll_table_base<L>::parsing_table;
        ll_table(state_map&                   states,
                 parsing_table&               table,
                 impl::parser_listener_map_t& listeners):
            ll_table_base<L>(states, table, listeners)
        {}
    };
    template <typename Key, typename State, typename ResultItem>
    using ll_table_elem = ll_table_item<Key, State, ResultItem>;
    template <typename L>
    using ll_table_default = ll_table<void, void, void, L>;
    template <typename Rules,
              typename Start,
              unsigned K,
              template <typename> typename Lexer = lexer>
    struct ll_parsing
    {
    private:
        template <typename Item1, typename Item2>
        struct ll_conflicts_error;
        template <typename T1, typename T2, typename T3>
        using union_with_conflict_check =
            tavl::_tavl_union_with_func_3<ll_conflicts_error, T1, T2, T3>;
        using lexer_t     = Lexer<Rules>;
        using lexer_rules = typename lexer_t::type;

        using first_set = first_set_k<lexer_rules, K>;
        template <typename D>
        using first_set_for_definition_k =
            first_set_for_definition_t<first_set, D, K>;
        template <typename S1, typename S2>
        using set_addition_helper = set_addition_t<S1, S2, K>;
        // preprocessing
        // calculate Yi for each definition without suffix
        struct skip_flag;
        template <typename Target, typename RemainingDef, typename Symbol>
        struct yi_for_each_symbol
        {
            static constexpr bool is_nonterminal =
                std::is_same_v<typename Symbol::first_type,
                               Impl::meta_identifier_flag>;
            using type = typename std::conditional_t<
                is_nonterminal,
                lazy_template<first_set_for_definition_k,
                              type_list_tail<RemainingDef>>,
                identity<skip_flag>>::type;
        };
        template <typename Last, typename Curr>
        struct yi_definition_merger
        {
            using type = type_list_push_back<Last, Curr>;
        };
        template <typename Last, typename Curr>
        struct yi_symbol_merger
        {
            using type = typename std::conditional_t<
                std::is_same_v<Curr, skip_flag>,
                identity<Last>,
                lazy_template<type_list_push_back,
                              Last,
                              std::conditional_t<tavl::is_empty_node_v<Curr>,
                                                 empty_set,
                                                 Curr>>>::type;
        };
        using yi_pre = for_each_symbol_t<lexer_rules,
                                         yi_for_each_symbol,
                                         tavl::tavl_union_3,
                                         yi_definition_merger,
                                         yi_symbol_merger,
                                         tavl::empty_node,
                                         type_list<>,
                                         type_list<>>;
        template <typename Target, typename Follow>
        struct generate_yi
        {
            static_assert(!std::is_same_v<tavl::tavl_find_t<yi_pre, Target>,
                                          tavl::empty_node>,
                          "Target not exists");
            using yi_list = typename tavl::tavl_find_t<yi_pre, Target>::value;
            template <typename Yi>
            struct for_each_yi
            {
                using type = set_addition_t<Yi, Follow, K>;
            };
            template <typename Y>
            struct for_each_def
            {
                // type_list_for_each
                using type = type_list_for_each<Y, for_each_yi>;
            };
            // type_list_for_each
            using type = type_list_for_each<yi_list, for_each_def>;
        };
        template <typename Target, typename... Symbol>
        struct generate_yi<Target, type_list<Symbol...>>
        {
            using type = typename generate_yi<
                Target,
                type_list_to_tavl<type_list<Symbol...>>>::type;
        };
        template <typename Target>
        struct generate_yi<Target, tavl::kv_pair<empty_symbol, std::true_type>>
        {
            using type = typename tavl::tavl_find_t<yi_pre, Target>::value;
        };
        template <typename Target, typename Follow>
        using generate_yi_t = typename generate_yi<Target, Follow>::type;

        template <typename Target, typename Follow>
        struct calculate_table
        {
            static_assert(
                !std::is_same_v<tavl::tavl_find_t<lexer_rules, Target>,
                                tavl::empty_node>,
                "Target not exists");
            using defs = typename tavl::tavl_find_t<lexer_rules, Target>::value;
            using yi_list     = yis_to_list_t<generate_yi_t<Target, Follow>>;
            using follow_tavl = typename std::conditional_t<
                is_type_list_v<Follow>,
                lazy_template<type_list_to_tavl, Follow>,
                identity<Follow>>::type;
            template <typename Defs, typename Yis>
            struct for_each_definition
            {
                using current = type_list_top<Defs>;
                using yi      = type_list_top<Yis>;
                using first   = typename std::conditional_t<
                    tavl::is_empty_node_v<first_set_for_definition_k<current>>,
                    identity<follow_tavl>,
                    lazy_template<set_addition_helper,
                                  first_set_for_definition_k<current>,
                                  follow_tavl>>::type;
                template <typename Key, typename>
                struct generate_table_item
                {
                    using type = tavl::kv_pair<
                        ll_table_item_tmp<Key, Target, current, yi>,
                        std::true_type>;
                };

                using table = tavl::tavl_for_each_t<first,
                                                    generate_table_item,
                                                    tavl::tavl_union_3,
                                                    tavl::empty_node>;
                using type  = tavl::tavl_union_with_func_t<
                    ll_conflicts_error,
                    typename for_each_definition<type_list_tail<Defs>,
                                                 type_list_tail<Yis>>::type,
                    table>;
            };
            template <typename Yis>
            struct for_each_definition<type_list<>, Yis>
            {
                using type = tavl::empty_node;
            };
            using type = typename for_each_definition<defs, yi_list>::type;
        };
        template <typename Target, typename Follow>
        using calculate_table_t =
            typename calculate_table<Target, Follow>::type;

        using initial_set = calculate_table_t<Start, empty_set>;

        template <typename Def, typename Yi>
        struct do_generation
        {
            using next_def = type_list_tail<Def>;
            using type     = typename do_generation<next_def, Yi>::type;
        };
        template <typename Target,
                  typename CurYi,
                  typename... Defs,
                  typename... Yis>
        struct do_generation<
            type_list<type_pair<Impl::meta_identifier_flag, Target>, Defs...>,
            type_list<CurYi, Yis...>>
        {
            using this_result = calculate_table_t<Target, CurYi>;
            using type        = type_list_try_push_front<
                typename do_generation<type_list<Defs...>,
                                       type_list<Yis...>>::type,
                typename calculate_table<Target, CurYi>::type>;
        };
        template <typename Def>
        struct do_generation<Def, type_list<>>
        {
            using type = type_list<>;
        };
        template <typename Item, typename>
        struct for_each_table_item
        {
            using type =
                typename do_generation<typename Item::production::second_type,
                                       typename Item::sets>::type;
        };
        template <typename Table>
        struct for_each_table_in_set
        {
            using type = tavl::tavl_for_each_t<Table,
                                               for_each_table_item,
                                               tlist_set_union_3,
                                               type_list<>>;
        };
        template <typename Last>
        struct generate_tables
        {
            using this_new =
                tlist_set_not_in_s<typename Last::second_type,
                                   tlist_set_for_each<typename Last::first_type,
                                                      for_each_table_in_set,
                                                      tlist_set_union_impl,
                                                      type_list<>>>;
            using type = type_pair<
                this_new,
                type_list_push_front_m<typename Last::second_type, this_new>>;
        };
        template <typename Old, typename New>
        struct stop_until
        {
            static constexpr bool value =
                std::is_same_v<typename New::first_type, type_list<>>;
        };
        using table_sets = typename do_until<
            stop_until,
            generate_tables,
            type_pair<type_list<initial_set>,
                      type_list<initial_set>>>::type::second_type;
        // rewrite LL(k) parsing tables
        // using Table to replace the nonterminal symbol
        template <typename Target, typename Follow>
        using calculate_table_rewrite =
            type_pair<Impl::ll_table_flag,
                      typename calculate_table<Target, Follow>::type>;
        template <typename Def, typename Yi>
        struct rewrite_production_defs
        {
            using current = type_list_top<Def>;
            static constexpr bool is_nt =
                std::is_same_v<typename current::first_type,
                               Impl::meta_identifier_flag>;
            using next_def = type_list_tail<Def>;
            using next_yi  = std::conditional_t<is_nt, type_list_tail<Yi>, Yi>;
            using following =
                typename rewrite_production_defs<next_def, next_yi>::type;
            using this_result = typename std::conditional_t<
                is_nt,
                lazy_template<calculate_table_rewrite,
                              typename current::second_type,
                              type_list_top<Yi>>,
                identity<current>>::type;
            using type = type_list_push_front<following, this_result>;
        };
        template <typename Def>
        struct rewrite_production_defs<Def, type_list<>>
        {
            using type = Def;
        };
        template <typename Item, typename>
        struct rewrite_table_item
        {
            using type = tavl::kv_pair<
                ll_table_item_tmp<typename Item::key,
                                  typename Item::production::first_type,
                                  typename rewrite_production_defs<
                                      typename Item::production::second_type,
                                      typename Item::sets>::type,
                                  type_list<>>,
                Item>;
        };
        template <typename Table>
        struct rewrite_table
        {
            using type = tavl::kv_pair<Table,
                                       tavl::tavl_for_each_t<Table,
                                                             rewrite_table_item,
                                                             tavl::tavl_union_3,
                                                             tavl::empty_node>>;
        };
        using rewrite_table_pre = tlist_set_for_each<table_sets,
                                                     rewrite_table,
                                                     tavl_merge_for_union,
                                                     tavl::empty_node>;

        // generate LL(k) parsing table(compile-time)
        template <typename OldTable, typename NewTable>
        struct generate_parsing_table
        {
            template <typename Item, typename>
            struct fill_table
            {
                using type =
                    tavl::kv_pair<type_pair<OldTable, typename Item::key>,
                                  Item>;
            };
            using type = tavl::tavl_for_each_t<NewTable,
                                               fill_table,
                                               tavl::tavl_union_3,
                                               tavl::empty_node>;
        };
        using compile_time_table = tavl::tavl_for_each_t<rewrite_table_pre,
                                                         generate_parsing_table,
                                                         tavl::tavl_union_3,
                                                         tavl::empty_node>;

        template <typename, typename>
        struct generate_runtime_table;
        template <typename Table, typename Key, typename Item>
        struct generate_runtime_table<type_pair<Table, Key>, Item>
        {
            using type = ll_table_elem<Key, Table, Item>;
        };
        template <typename L, typename R, typename C>
        struct merge_runtime_table
        {
            using type = ll_table<C, L, R, lexer_t>;
        };
        using runtime_table_t =
            tavl::tavl_for_each_t<compile_time_table,
                                  generate_runtime_table,
                                  merge_runtime_table,
                                  ll_table_default<lexer_t>>;

    public:
        using start_table   = initial_set;
        using parsing_table = runtime_table_t;
        static_assert(!tavl::tavl_is_same_v<parsing_table, tavl::empty_node>,
                      "empty parsing table");
        ll_parsing() = default;
        std::any parse(std::istream&      input,
                       const std::string& skip = " \f\n\r\t\v")
        {
            PRINT_LOG("\n" + _table.dump());
            _table.set_lexer(input, _lexer_listeners, skip);
            auto& lexer     = _table.get_lexer();
            auto  lookahead = lexer.template lookahead<K>();
            auto  parse_result =
                _table.template try_parse<start_table>(lookahead);
            assert(parse_result.size() == 1);
            return parse_result[0];
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

    private:
        typename parsing_table::state_map                 _s;
        typename parsing_table::parsing_table             _p;
        impl::parser_listener_map_t                       _parser_listeners;
        std::unordered_map<std::string, lexer_listener_t> _lexer_listeners;
        runtime_table_t _table{_s, _p, _parser_listeners};
    };
} // namespace Compiler
namespace tavl
{
    template <typename... Arg1, typename... Arg2>
    struct compare<Compiler::ll_table_item_tmp<Arg1...>,
                   Compiler::ll_table_item_tmp<Arg2...>>
    {
    public:
        static constexpr int value =
            compare<Compiler::type_list<Arg1...>,
                    Compiler::type_list<Arg2...>>::value;
    };
// for ll_table_flag
#define COMP_DEF(flag1, flag2)                                   \
    template <>                                                  \
    struct compare<Compiler::Impl::flag1, Compiler::Impl::flag2> \
    {                                                            \
        static constexpr int value = 0;                          \
    }
#define COMP_DEFS(flag)                     \
    COMP_DEF(flag, ll_table_flag);          \
    COMP_DEF(flag, empty_sequence_flag);    \
    COMP_DEF(flag, grouped_sequence_flag);  \
    COMP_DEF(flag, meta_identifier_flag);   \
    COMP_DEF(flag, optional_sequence_flag); \
    COMP_DEF(flag, repeated_sequence_flag); \
    COMP_DEF(flag, special_sequence_flag);  \
    COMP_DEF(flag, terminal_string_flag);   \
    COMP_DEF(empty_sequence_flag, flag);    \
    COMP_DEF(grouped_sequence_flag, flag);  \
    COMP_DEF(meta_identifier_flag, flag);   \
    COMP_DEF(optional_sequence_flag, flag); \
    COMP_DEF(repeated_sequence_flag, flag); \
    COMP_DEF(special_sequence_flag, flag);  \
    COMP_DEF(terminal_string_flag, flag)
    COMP_DEFS(ll_table_flag);
#undef COMP_DEFS
#undef COMP_DEF
} // namespace tavl
#endif
