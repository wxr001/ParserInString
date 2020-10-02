#pragma once
#include "../include/Common/Identity.hpp"
#include <type_traits>
using Compiler::identity;
using Compiler::lazy_template;
template <typename...>
struct type_list;
template <typename T>
struct type_list_top_helper;
template <typename E, typename... Es>
struct type_list_top_helper<type_list<E, Es...>>
{
    using type = E;
};
template <typename T>
using type_list_top = typename type_list_top_helper<T>::type;
template <typename T>
struct type_list_tail_helper;
template <typename E, typename... Es>
struct type_list_tail_helper<type_list<E, Es...>>
{
    using type = type_list<Es...>;
};
template <typename T>
using type_list_tail = typename type_list_tail_helper<T>::type;
template <typename, typename>
struct type_list_push_back_helper;
template <typename... Elems, typename T>
struct type_list_push_back_helper<type_list<Elems...>, T>
{
    using type = type_list<Elems..., T>;
};
template <typename L, typename T>
using type_list_push_back = typename type_list_push_back_helper<L, T>::type;
template <typename, typename>
struct type_list_push_front_helper;
template <typename... Elems, typename T>
struct type_list_push_front_helper<type_list<Elems...>, T>
{
    using type = type_list<T, Elems...>;
};
template <typename L, typename T>
using type_list_push_front = typename type_list_push_front_helper<L, T>::type;
template <typename L, typename V, int N>
struct type_list_push_back_n_helper:
    type_list_push_back_n_helper<type_list_push_back<L, V>, V, N - 1>
{};
template <typename L, typename V>
struct type_list_push_back_n_helper<L, V, 0>
{
    using type = L;
};
template <typename L, typename V, int N>
using type_list_push_back_n =
    typename type_list_push_back_n_helper<L, V, N>::type;
template <typename L, typename V>
struct type_list_push_back_m_helper;
template <typename... Elems, typename V>
struct type_list_push_back_m_helper<type_list<Elems...>, V>
{
    using type = type_list<Elems..., V>;
};
template <typename... Elems, typename... Vs>
struct type_list_push_back_m_helper<type_list<Elems...>, type_list<Vs...>>
{
    using type = type_list<Elems..., Vs...>;
};
template <typename L, typename T>
using type_list_push_back_m = typename type_list_push_back_m_helper<L, T>::type;
template <typename L>
struct type_list_size_helper;
template <typename... Elems>
struct type_list_size_helper<type_list<Elems...>>
{
    static constexpr auto value = sizeof...(Elems);
};
template <typename L>
inline constexpr auto type_list_size = type_list_size_helper<L>::value;

template <typename T, template <typename> typename Apply>
struct type_list_for_each_helper;
template <typename... Elems, template <typename> typename Apply>
struct type_list_for_each_helper<type_list<Elems...>, Apply>
{
    using type = type_list<typename Apply<Elems>::type...>;
};
template <template <typename> typename Apply>
struct type_list_for_each_helper<type_list<>, Apply>
{
    using type = type_list<>;
};
template <typename T, template <typename> typename Apply>
using type_list_for_each = typename type_list_for_each_helper<T, Apply>::type;

template <typename T, typename val, int idx = 0>
struct type_list_find_helper;
template <typename... Elems, typename V, int idx>
struct type_list_find_helper<type_list<V, Elems...>, V, idx>
{
    static constexpr int value = idx;
};
template <typename E, typename... Elems, typename V, int idx>
struct type_list_find_helper<type_list<E, Elems...>, V, idx>:
    type_list_find_helper<type_list<Elems...>, V, idx + 1>
{};
template <typename val, int idx>
struct type_list_find_helper<type_list<>, val, idx>
{
    static constexpr int value = -1; // not fount
};
template <typename T, typename val>
inline constexpr int type_list_find = type_list_find_helper<T, val>::value;

template <typename T, int N>
struct type_list_get_helper;
template <typename E, typename... Elems, int N>
struct type_list_get_helper<type_list<E, Elems...>, N>:
    type_list_get_helper<type_list<Elems...>, N - 1>
{};
template <typename E, typename... Elems>
struct type_list_get_helper<type_list<E, Elems...>, 0>
{
    using type = E;
};
template <int N>
struct type_list_get_helper<type_list<>, N>
{};
template <>
struct type_list_get_helper<type_list<>, 0>
{};
template <typename L, int N>
using type_list_get = typename type_list_get_helper<L, N>::type;

template <typename Result, typename T, int N>
struct type_list_first_n_helper;
template <typename... Results, typename E, typename... Elems, int N>
struct type_list_first_n_helper<type_list<Results...>,
                                type_list<E, Elems...>,
                                N>
{
    using type = typename type_list_first_n_helper<type_list<Results..., E>,
                                                   type_list<Elems...>,
                                                   N - 1>::type;
};
template <typename... R, typename... E>
struct type_list_first_n_helper<type_list<R...>, type_list<E...>, 0>
{
    using type = type_list<R...>;
};
template <typename... R, int N>
struct type_list_first_n_helper<type_list<R...>, type_list<>, N>
{
    using type = type_list<R...>;
};
template <typename... R>
struct type_list_first_n_helper<type_list<R...>, type_list<>, 0>
{
    using type = type_list<R...>;
};
template <typename T, int n>
using type_list_first_n =
    typename type_list_first_n_helper<type_list<>, T, n>::type;
template <typename T, typename Val>
using type_list_try_push_front =
    std::conditional_t<type_list_find<T, Val> == -1,
                       type_list_push_front<T, Val>,
                       T>;
template <typename L, typename V>
struct type_list_push_front_m_helper;
template <typename... Elems, typename V>
struct type_list_push_front_m_helper<type_list<Elems...>, V>
{
    using type = type_list<V, Elems...>;
};
template <typename... Elems, typename... Vs>
struct type_list_push_front_m_helper<type_list<Elems...>, type_list<Vs...>>
{
    using type = type_list<Vs..., Elems...>;
};
template <typename L, typename T>
using type_list_push_front_m =
    typename type_list_push_front_m_helper<L, T>::type;

template <typename S, typename S2>
struct tlist_set_not_in_s_impl
{
    using current = type_list_top<S2>;
    using type    = typename std::conditional_t<
        type_list_find<S, current> == -1,
        lazy_template<
            type_list_push_front,
            typename tlist_set_not_in_s_impl<S, type_list_tail<S2>>::type,
            current>,
        tlist_set_not_in_s_impl<S, type_list_tail<S2>>>::type;
};
template <typename S>
struct tlist_set_not_in_s_impl<S, type_list<>>
{
    using type = type_list<>;
};
template <typename S, typename S2>
using tlist_set_not_in_s = typename tlist_set_not_in_s_impl<S, S2>::type;
namespace impl
{
    template <typename T, template <typename> typename Apply>
    struct type_list_for_each_helper;
    template <typename elem,
              template <typename>
              typename Apply,
              typename... tails>
    struct type_list_for_each_helper<type_list<elem, tails...>, Apply>
    {
        using type = type_list_push_front<
            typename type_list_for_each_helper<type_list<tails...>,
                                               Apply>::type,
            typename Apply<elem>::type>;
    };
    template <template <typename> typename Apply>
    struct type_list_for_each_helper<type_list<>, Apply>
    {
        using type = type_list<>;
    };
    template <typename T, template <typename> typename Apply>
    using type_list_for_each =
        typename type_list_for_each_helper<T, Apply>::type;
} // namespace impl