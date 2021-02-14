// ParserInString -*- C++ -*-

// Copyright (C) 2019-2020  Scott Brown

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

#ifndef PARSER_IN_STRING_RULES_CODETORULES
#define PARSER_IN_STRING_RULES_CODETORULES

#include "TAVL.hpp"
#include "pis/Common/Identity.hpp"
#include "pis/Common/TAVLHelper.hpp"
#include "pis/Common/Tlist.hpp"
#include "pis/Common/Tstring.hpp"
#include "rules_common.hpp"
#include <type_traits>
namespace pis
{
    using namespace InjectedString;
    template <typename T>
    struct rules_wrapper
    {
        using rules = T;
    };
    namespace impl_phase_2_3
    {
        // phase 2: rewrite some rules
        // (for each optional group) [...] ->
        //     create another identifier (A -> ... | @empty)
        // (for each repeated group) {...} ->
        //     create another identifier (A -> ... A | @empty)
        // (for other groups) (...) ->
        //     create another identifier (A -> ...)

        /**
         * @brief generate unused identifier used for rewriting rules
         * @tparam args arguments for the identifier, each of which should be
         * one of [0-9A-Z]
         * @note initial value: <'0'>
         */
        template <char... args>
        struct identifier_generator
        {
        private:
            template <char C,
                      char... others,
                      typename = std::enable_if_t<('0' <= C && C < '9') ||
                                                  ('A' <= C && C < 'Z')>>
            static identifier_generator<C + 1, others...>
                generate(t_string<C, others...>);
            template <char... others>
            static identifier_generator<'A', others...>
                generate(t_string<'9', others...>);
            template <char... others>
            static identifier_generator<'0', 'Z', others...>
                generate(t_string<'Z', others...>);

        public:
            using type = t_string<'@', args...>;
            using next = decltype(generate(t_string<args...>()));
        };
        template <typename definition,
                  typename next_unused_identifier,
                  typename new_rules = type_list<>>
        struct parse_optional_group
        {
        private:
            using term        = type_list_top<definition>;
            using other_terms = type_list_tail<definition>;
            static constexpr bool is_optional_group =
                std::is_same_v<typename term::first_type,
                               Impl::optional_sequence_flag>;
            using next_idenfitier = std::conditional_t<
                is_optional_group,
                typename next_unused_identifier::next, // generate new ones
                next_unused_identifier>;
            using term_parsed = std::conditional_t<
                is_optional_group,
                type_pair<Impl::meta_identifier_flag,
                          typename next_unused_identifier::type>, // replace
                                                                  // with new id
                term>;
            template <typename option_rule>
            using added_rule_with_empty =
                type_list<typename next_unused_identifier::type,
                          type_list_push_back<
                              option_rule,
                              type_list<type_pair<Impl::empty_sequence_flag,
                                                  t_string<>>>>>;
            template <template <typename...> typename base,
                      template <typename...>
                      typename added_base,
                      typename new_rules_t,
                      typename option_rule_t>
            struct lazy_push_back
            {
                using type = base<new_rules_t, added_base<option_rule_t>>;
            };
            using rules = typename std::conditional_t<
                is_optional_group,
                lazy_push_back<type_list_push_back,
                               added_rule_with_empty,
                               new_rules,
                               typename term::second_type>,
                identity<new_rules>>::type;

        public:
            using type = type_list_push_front<
                typename parse_optional_group<other_terms,
                                              next_idenfitier,
                                              rules>::type,
                term_parsed>;
            using next = typename parse_optional_group<other_terms,
                                                       next_idenfitier,
                                                       rules>::next;
            using rule = typename parse_optional_group<other_terms,
                                                       next_idenfitier,
                                                       rules>::rule;
        };
        template <typename next_unused_identifier, typename new_rules>
        struct parse_optional_group<type_list<>,
                                    next_unused_identifier,
                                    new_rules>
        {
            using type = type_list<>;
            using next = next_unused_identifier;
            using rule = new_rules;
        };
        template <typename definition,
                  typename next_unused_identifier,
                  typename new_rules = type_list<>>
        struct parse_repeated_group
        {
        private:
            using term        = type_list_top<definition>;
            using other_terms = type_list_tail<definition>;
            static constexpr bool is_repeated_group =
                std::is_same_v<typename term::first_type,
                               Impl::repeated_sequence_flag>;
            template <typename Placeholder, bool enable>
            struct helper
            {
                using identifier     = typename next_unused_identifier::type;
                using next_unused_id = typename next_unused_identifier::next;
                using new_term =
                    type_pair<Impl::meta_identifier_flag, identifier>;

                template <typename T>
                struct add_right_recursive
                {
                    using type = type_list_push_back<
                        T,
                        type_pair<Impl::meta_identifier_flag, identifier>>;
                };
                using rules = type_list_push_back<
                    new_rules,
                    type_list<identifier,
                              type_list_push_back<
                                  type_list_for_each<typename term::second_type,
                                                     add_right_recursive>,
                                  type_list<type_pair<Impl::empty_sequence_flag,
                                                      t_string<>>>>>>;

                using type = type_list_push_front<
                    typename parse_repeated_group<other_terms,
                                                  next_unused_id,
                                                  rules>::type,
                    new_term>;
                using next = typename parse_repeated_group<other_terms,
                                                           next_unused_id,
                                                           rules>::next;
                using rule = typename parse_repeated_group<other_terms,
                                                           next_unused_id,
                                                           rules>::rule;
            };
            template <typename Placeholder>
            struct helper<Placeholder, false>
            {
                using type = type_list_push_front<
                    typename parse_repeated_group<other_terms,
                                                  next_unused_identifier,
                                                  new_rules>::type,
                    term>;
                using next =
                    typename parse_repeated_group<other_terms,
                                                  next_unused_identifier,
                                                  new_rules>::next;
                using rule =
                    typename parse_repeated_group<other_terms,
                                                  next_unused_identifier,
                                                  new_rules>::rule;
            };

        public:
            using type = typename helper<void, is_repeated_group>::type;
            using next = typename helper<void, is_repeated_group>::next;
            using rule = typename helper<void, is_repeated_group>::rule;
        };
        template <typename next_unused_identifier, typename new_rules>
        struct parse_repeated_group<type_list<>,
                                    next_unused_identifier,
                                    new_rules>
        {
            using type = type_list<>;
            using next = next_unused_identifier;
            using rule = new_rules;
        };
        template <typename definition,
                  typename next_unused_identifier,
                  typename new_rules = type_list<>>
        struct parse_grouped_group
        {
        private:
            using term        = type_list_top<definition>;
            using other_terms = type_list_tail<definition>;
            static constexpr bool is_grouped_group =
                std::is_same_v<typename term::first_type,
                               Impl::grouped_sequence_flag>;
            template <typename Placeholder, bool enable>
            struct helper
            {
                using identifier     = typename next_unused_identifier::type;
                using next_unused_id = typename next_unused_identifier::next;
                using new_term =
                    type_pair<Impl::meta_identifier_flag, identifier>;

                template <typename T>
                struct add_right_recursive
                {
                    using type = type_list_push_back<
                        T,
                        type_pair<Impl::meta_identifier_flag, identifier>>;
                };
                using rules = type_list_push_back<
                    new_rules,
                    type_list<identifier, typename term::second_type>>;

                using type = type_list_push_front<
                    typename parse_grouped_group<other_terms,
                                                 next_unused_id,
                                                 rules>::type,
                    new_term>;
                using next = typename parse_grouped_group<other_terms,
                                                          next_unused_id,
                                                          rules>::next;
                using rule = typename parse_grouped_group<other_terms,
                                                          next_unused_id,
                                                          rules>::rule;
            };
            template <typename Placeholder>
            struct helper<Placeholder, false>
            {
                using type = type_list_push_front<
                    typename parse_grouped_group<other_terms,
                                                 next_unused_identifier,
                                                 new_rules>::type,
                    term>;
                using next =
                    typename parse_grouped_group<other_terms,
                                                 next_unused_identifier,
                                                 new_rules>::next;
                using rule =
                    typename parse_grouped_group<other_terms,
                                                 next_unused_identifier,
                                                 new_rules>::rule;
            };

        public:
            using type = typename helper<void, is_grouped_group>::type;
            using next = typename helper<void, is_grouped_group>::next;
            using rule = typename helper<void, is_grouped_group>::rule;
        };
        template <typename next_unused_identifier, typename new_rules>
        struct parse_grouped_group<type_list<>,
                                   next_unused_identifier,
                                   new_rules>
        {
            using type = type_list<>;
            using next = next_unused_identifier;
            using rule = new_rules;
        };
        template <typename rule, typename generator, typename generated_rules>
        struct for_each_definition;
        template <typename rule,
                  typename generator,
                  typename generated_rules,
                  bool skip>
        struct for_each_definition_impl
        {
            using current_definition = type_list_top<rule>;
            using other_definitions  = type_list_tail<rule>;
            // optional group
            using optional_parser = parse_optional_group<current_definition,
                                                         generator,
                                                         generated_rules>;
            // repeated group
            using repeated_parser =
                parse_repeated_group<typename optional_parser::type,
                                     typename optional_parser::next,
                                     typename optional_parser::rule>;
            // grouped group
            using grouped_parser =
                parse_grouped_group<typename repeated_parser::type,
                                    typename repeated_parser::next,
                                    typename repeated_parser::rule>;
            // result
            using result_definition = typename grouped_parser::type;
            using next_generator    = typename grouped_parser::next;
            using rules             = typename grouped_parser::rule;
            using other_definitions_result =
                for_each_definition<other_definitions, next_generator, rules>;

            using type =
                type_list_push_front<typename other_definitions_result::type,
                                     result_definition>;
            using result_generator =
                typename other_definitions_result::result_generator;
            using result_rules =
                typename other_definitions_result::result_rules;
        };
        template <typename rule, typename generator, typename generated_rules>
        struct for_each_definition_impl<rule, generator, generated_rules, true>
        {
            using current_definition = type_list_top<rule>;
            using other_definitions  = type_list_tail<rule>;
            using other_definitions_result =
                for_each_definition<other_definitions,
                                    generator,
                                    generated_rules>;

            using type =
                type_list_push_front<typename other_definitions_result::type,
                                     current_definition>;
            using result_generator =
                typename other_definitions_result::result_generator;
            using result_rules =
                typename other_definitions_result::result_rules;
        };
        template <typename rule, typename generator, typename generated_rules>
        struct for_each_definition
        {
            using current_definition   = type_list_top<rule>;
            static constexpr bool skip = is_lexer_rule<current_definition>;

            using other_definitions = type_list_tail<rule>;

            using type = typename for_each_definition_impl<rule,
                                                           generator,
                                                           generated_rules,
                                                           skip>::type;

            using result_generator =
                typename for_each_definition_impl<rule,
                                                  generator,
                                                  generated_rules,
                                                  skip>::result_generator;
            using result_rules =
                typename for_each_definition_impl<rule,
                                                  generator,
                                                  generated_rules,
                                                  skip>::result_rules;
        };
        template <typename generator, typename generated_rules>
        struct for_each_definition<type_list<>, generator, generated_rules>
        {
            using type             = type_list<>;
            using result_generator = generator;
            using result_rules     = generated_rules;
        };
        template <typename T,
                  typename rules     = type_list<>,
                  typename generator = identifier_generator<'0'>>
        struct for_each_rule
        {
        private:
            using current_rule = type_list_top<T>;
            using other_rules  = type_list_tail<T>;
            using identifier   = type_list_top<current_rule>;
            template <typename TL>
            struct to_type_list
            {
                using type = TL;
            };
            template <char... chs>
            struct to_type_list<Impl::definitions_list<t_string<chs...>>>
            {
                using type =
                    typename Impl::definitions_list<t_string<chs...>>::symbol;
            };
            using definitions =
                typename to_type_list<type_list_top<type_list_tail<
                    current_rule>>>::type; //! type_list<single_definition>
            using result =
                for_each_definition<definitions, generator, type_list<>>;
            using result_rule = type_list<identifier, typename result::type>;

        public:
        private:
            using new_rules =
                type_list_push_back_m<other_rules,
                                      typename result::result_rules>;
            using next = for_each_rule<new_rules,
                                       type_list_push_back<rules, result_rule>,
                                       typename result::result_generator>;

        public:
            using type           = typename next::type;
            using next_generator = typename next::next_generator;
        };
        template <typename rules, typename generator>
        struct for_each_rule<type_list<>, rules, generator>
        {
            using next_generator = generator;
            using type           = rules;
        };
        // phase 3: generate identifier-rule map
        template <typename Map, typename Identifier, typename Defs>
        using merge_multiple_defs = tavl::tavl_update_t<
            Map,
            Identifier,
            type_list_push_back_m<
                typename tavl::tavl_find_t<Map, Identifier>::value,
                Defs>>;
        template <typename T, int Placeholder = 0>
        struct generate_map
        {
            using rule        = type_list_top<T>;
            using others      = type_list_tail<T>;
            using identifier  = type_list_top<rule>;
            using definitions = type_list_top<type_list_tail<rule>>;
            using map         = typename generate_map<others>::type;
            using type        = typename std::conditional_t<
                tavl::tavl_contain_v<map, identifier>,
                lazy_template<merge_multiple_defs,
                              map,
                              identifier,
                              definitions>,
                lazy_template<tavl::tavl_insert_t,
                              map,
                              identifier,
                              definitions>>::type;
        };
        template <int Placeholder>
        struct generate_map<type_list<>, Placeholder>
        {
            using type = tavl::empty_node;
        };
    } // namespace impl_phase_2_3
    template <const char* CODE,
              typename generator = impl_phase_2_3::identifier_generator<'0'>>
    struct rule_to_map
    {
    private:
        static_assert(slen(CODE) > 0);
        make_builder(input, CODE);
        using input = typename Impl::remove_leading_space_and_comment<
            typename builder_input<slen(CODE) - 1>::type>::next;
        // phase 1: translate string to list of rules
        template <typename T, typename Result>
        struct rule_parser;
        template <typename Result, char... chars>
        struct rule_parser<t_string<chars...>, Result>
        {
        private:
            using last_start = t_string<chars...>;
            using rule       = Impl::rule<t_string<chars...>>;
            using current_next =
                typename Impl::remove_leading_space_and_comment<
                    typename rule::next>::next;
            static_assert(!std::is_same_v<last_start, current_next> ||
                              std::is_same_v<current_next, t_string<>>,
                          "Grammar Error!");

        public:
            using type = typename rule_parser<
                current_next,
                type_list_push_back<Result, typename rule::symbol>>::type;
        };
        template <typename Result>
        struct rule_parser<t_string<>, Result>
        {
            using type = Result;
        };
        using rule_list = typename rule_parser<input, type_list<>>::
            type; //! type_list<type_list<t_string, definitions_list>,...>

        // phase 2: rewrite some rules
        using rewrite_rules = typename impl_phase_2_3::
            for_each_rule<rule_list, type_list<>, generator>::type;
        // phase 3: generate identifier-rule map
        using identifier_rule_map =
            typename impl_phase_2_3::generate_map<rewrite_rules>::type;

    public:
        using type           = identifier_rule_map;
        using next_generator = typename impl_phase_2_3::
            for_each_rule<rule_list, type_list<>, generator>::next_generator;
    };
    template <const char* CODE,
              typename generator = impl_phase_2_3::identifier_generator<'0'>>
    using rule_to_map_t = typename rule_to_map<CODE, generator>::type;
    template <const char* CODE,
              typename generator = impl_phase_2_3::identifier_generator<'0'>>
    using rule_to_map_g = typename rule_to_map<CODE, generator>::next_generator;

    template <typename IdToRulesMap>
    class undefined_checker
    {
    private:
        // phase 4: check for undefined identifier
        using identifier_rule_map = IdToRulesMap;
        template <typename T>
        struct undefined_identifier
        {};
        template <typename T>
        using check_id = typename std::conditional_t<
            tavl::tavl_contain_v<identifier_rule_map, typename T::second_type>,
            tavl::lazy_template<identity, int>,
            tavl::lazy_template<undefined_identifier,
                                typename T::second_type>>::type::type;
        template <typename T>
        struct check_exist
        {
            using type = typename std::conditional_t<
                std::is_same_v<typename T::first_type,
                               Impl::meta_identifier_flag>,
                lazy_template<check_id, T>,
                identity<int>>::type;
        };
        template <typename T>
        struct for_each_rule
        {
            using type = type_list_for_each<T, check_exist>;
        };
        template <typename Identifier, typename Rules>
        struct for_each_defined_idenfitier
        {
            using type = type_list_for_each<Rules, for_each_rule>;
        };
        using check_undefined_identifier =
            tavl::tavl_for_each_t<identifier_rule_map,
                                  for_each_defined_idenfitier>;

    public:
        using type =
            std::conditional_t<std::is_same_v<check_undefined_identifier, void>,
                               identifier_rule_map,
                               identifier_rule_map>;
    };
    template <typename IdToRulesMap>
    using undefined_checker_t = typename undefined_checker<IdToRulesMap>::type;
    // template <typename IdToRulesMap>
    // struct generated_rule_merger
    // {
    // private:
    //     // phase 5: merge generated same rules (which may be generated from
    //     // different rules)
    //     // @1 -> abc;
    //     // @2 -> abc;
    //     using identifier_rule_map = IdToRulesMap;
    //     template <typename Identifier, typename Rules>
    //     struct for_each_idenfitier
    //     {
    //         template <typename Id2, typename Rules2>
    //         struct for_each_another_identifier
    //         {
    //             static constexpr bool same_rule =
    //                 std::is_same_v<Rules, Rules2> &&
    //                 (first_character_s<Id2>::value == '@') &&
    //                 !std::is_same_v<Identifier, Id2>;
    //             using type = std::conditional_t<same_rule, Id2, int>;
    //         };
    //         template <typename Previous, typename Current>
    //         struct for_each_merger
    //         {
    //             using type = typename std::conditional_t<
    //                 std::is_same_v<Current, int>,
    //                 identity<Previous>,
    //                 tavl::lazy_template<type_list_push_back,
    //                                     Previous,
    //                                     Current>>::type;
    //         };
    //         using type =
    //             type_pair<Identifier,
    //                       typename std::conditional_t<
    //                           first_character_s<Identifier>::value == '@',
    //                           identity<tavl::tavl_for_each_middle_order_t<
    //                               identifier_rule_map,
    //                               for_each_another_identifier,
    //                               for_each_merger,
    //                               type_list<>>>,
    //                           identity<type_list<>>>::type>;
    //     };
    //     template <typename Tree, typename List>
    //     struct merge_all;
    //     template <typename Tree, typename Identifier>
    //     struct merge_all<Tree, type_pair<Identifier, type_list<>>>
    //     {
    //         using type = Tree;
    //     };
    //     template <typename Tree, typename Identifier, typename List>
    //     struct merge_all<Tree, type_pair<Identifier, List>>
    //     {
    //         using type = tavl::tavl_insert_t<Tree, Identifier, List>;
    //     };
    //     using check_mergable_result =
    //         tavl::tavl_for_each_middle_order_t<identifier_rule_map,
    //                                            for_each_idenfitier,
    //                                            merge_all,
    //                                            tavl::empty_node>;
    //     template <typename Identifier, typename RuleList>
    //     struct for_each_identifier_for_merging
    //     {
    //         template <typename List, typename Tree>
    //         struct replace_and_merge
    //         {
    //         private:
    //             using to_be_removed = type_list_top<List>;
    //             using others        = type_list_tail<List>;
    //             static constexpr bool skip =
    //                 tavl::compare_v<Identifier, to_be_removed> > 0;
    //             using removed_tree = typename std::conditional_t<
    //                 !skip,
    //                 tavl::
    //                     lazy_template<tavl::tavl_remove_t, Tree,
    //                     to_be_removed>,
    //                 identity<Tree>>::type;
    //             template <typename T>
    //             struct for_each_term
    //             {
    //                 using type = T;
    //             };
    //             template <typename Flag, typename Str>
    //             struct for_each_term<type_pair<Flag, Str>>
    //             {
    //                 using type = std::conditional_t<
    //                     std::is_same_v<Flag, Impl::meta_identifier_flag> &&
    //                         std::is_same_v<Str, to_be_removed>,
    //                     type_pair<Impl::meta_identifier_flag, Identifier>,
    //                     type_pair<Flag, Str>>;
    //             };
    //             template <typename Definition>
    //             struct for_each_definition
    //             {
    //                 using type = type_list_for_each<Definition,
    //                 for_each_term>;
    //             };
    //             template <typename Id, typename Rules>
    //             struct for_each_identifier_for_replacing_meta_id
    //             {
    //                 using type = tavl::kv_pair<
    //                     Id,
    //                     type_list_for_each<Rules, for_each_definition>>;
    //             };
    //             using replaced = tavl::tavl_for_each_middle_order<
    //                 removed_tree,
    //                 for_each_identifier_for_replacing_meta_id,
    //                 tavl::tavl_for_each_middle_order_tree_merger,
    //                 tavl::empty_node>;

    //         public:
    //             using type = typename std::
    //                 conditional_t<skip, identity<Tree>, replaced>::type;
    //         };

    //         using type =
    //             typename replace_and_merge<RuleList,
    //             identifier_rule_map>::type;
    //     };
    //     template <typename Previous, typename Current>
    //     struct merge_trees
    //     {
    //         template <typename Key, typename Value>
    //         struct for_each_node
    //         {
    //             using type = std::conditional_t<
    //                 std::is_same_v<
    //                     typename tavl::tavl_find_t<Current, Key>::value,
    //                     typename tavl::tavl_find_t<identifier_rule_map,
    //                                                Key>::value>,
    //                 tavl::kv_pair<
    //                     Key,
    //                     typename tavl::tavl_find_t<Previous, Key>::value>,
    //                 tavl::kv_pair<
    //                     Key,
    //                     typename tavl::tavl_find_t<Current, Key>::value>>;
    //         };
    //         using type = tavl::tavl_for_each_middle_order_t<
    //             tavl::tavl_intersect_t<Current, Previous>,
    //             for_each_node,
    //             tavl::tavl_for_each_middle_order_tree_merger,
    //             tavl::empty_node>;
    //     };
    //     using replaced_result = typename tavl::tavl_for_each_middle_order<
    //         check_mergable_result,
    //         for_each_identifier_for_merging,
    //         merge_trees,
    //         identifier_rule_map>::type;

    // public:
    //     using type = typename std::conditional_t<
    //         std::is_same_v<replaced_result, identifier_rule_map>,
    //         tavl::lazy_template<identity, replaced_result>,
    //         tavl::lazy_template<generated_rule_merger,
    //                             replaced_result>>::type::type;
    // };
    template <typename IdToRulesMap>
    using generated_rule_merger_t = IdToRulesMap;

    template <typename generator, const char*... CODE>
    struct multiple_rules;
    template <typename generator,
              const char* CODE1,
              const char* CODE2,
              const char*... CODES>
    struct multiple_rules<generator, CODE1, CODE2, CODES...>
    {
        using r2m1 = rule_to_map_t<CODE1, generator>;
        using r2m2 = rule_to_map_t<CODE2, rule_to_map_g<CODE1, generator>>;
        using g    = rule_to_map_g<CODE2, rule_to_map_g<CODE1, generator>>;
        using type =
            tavl::tavl_union_t<r2m1,
                               r2m2,
                               typename multiple_rules<g, CODES...>::type>;
    };
    template <typename generator, const char* CODE>
    struct multiple_rules<generator, CODE>
    {
        using type = rule_to_map_t<CODE, generator>;
    };
    template <typename generator>
    struct multiple_rules<generator>
    {
        using type = tavl::empty_node;
    };
    template <const char*... CODE>
    using multiple_rules_t =
        typename multiple_rules<impl_phase_2_3::identifier_generator<'0'>,
                                CODE...>::type;
    template <const char*... CODE>
    using code_to_rules_t = undefined_checker_t<generated_rule_merger_t<
        undefined_checker_t<multiple_rules_t<CODE...>>>>;
} // namespace pis

#endif
