#pragma once
struct null_t;
template <typename H = null_t, typename T = null_t>
struct type_list;
template <typename T>
struct type_list_content;
template <typename H, typename T>
struct type_list_content<type_list<H, T>>
{
    using top  = H;
    using tail = T;
};
template <>
struct type_list_content<null_t>
{
    using top  = null_t;
    using tail = null_t;
};
template <typename T>
using type_list_top = typename type_list_content<T>::top;
template <typename T>
using type_list_tail = typename type_list_content<T>::tail;
template <typename L, typename T>
struct type_list_push_back_helper;
template <>
struct type_list_push_back_helper<null_t, null_t>
{
    using type = null_t;
};
template <typename T>
struct type_list_push_back_helper<null_t, T>
{
    using type = type_list<T, null_t>;
};
template <typename Head, typename Tail, typename T>
struct type_list_push_back_helper<type_list<Head, Tail>, T>
{
    using type =
        type_list<Head, typename type_list_push_back_helper<Tail, T>::type>;
};
template <typename L, typename T>
using type_list_push_back = typename type_list_push_back_helper<L, T>::type;
template <typename L, typename T>
using type_list_push_front = type_list<T, L>;
template <typename L, typename T, int N>
struct type_list_push_back_n_helper;
template <int N>
struct type_list_push_back_n_helper<null_t, null_t, N>
{
    using type = null_t;
};
template <typename T, int N>
struct type_list_push_back_n_helper<null_t, T, N>
{
    using type = type_list<
        T,
        typename type_list_push_back_n_helper<null_t, T, N - 1>::type>;
};
template <typename T>
struct type_list_push_back_n_helper<null_t, T, 0>
{
    using type = null_t;
};
template <typename Head, typename Tail, typename T, int N>
struct type_list_push_back_n_helper<type_list<Head, Tail>, T, N>
{
    using type =
        type_list<Head,
                  typename type_list_push_back_n_helper<Tail, T, N>::type>;
};
template <typename L, typename V, int N>
using type_list_push_back_n =
    typename type_list_push_back_n_helper<L, V, N>::type;
template <typename L, typename T>
struct type_list_push_back_m_helper;
template <>
struct type_list_push_back_m_helper<null_t, null_t>
{
    using type = null_t;
};
template <typename T>
struct type_list_push_back_m_helper<null_t, T>
{
    using type = type_list<T, null_t>;
};
template <typename H, typename T>
struct type_list_push_back_m_helper<null_t, type_list<H, T>>
{
    using type = type_list<H, T>;
};
template <typename Head, typename Tail, typename T>
struct type_list_push_back_m_helper<type_list<Head, Tail>, T>
{
    using type =
        type_list<Head, typename type_list_push_back_m_helper<Tail, T>::type>;
};
template <typename L, typename T>
using type_list_push_back_m = typename type_list_push_back_m_helper<L, T>::type;
template <typename L>
struct type_list_size_helper;
template <typename H, typename T>
struct type_list_size_helper<type_list<H, T>>
{
    static constexpr int value = 1 + type_list_size_helper<T>::value;
};
template <>
struct type_list_size_helper<null_t>
{
    static constexpr int value = 0;
};
template <typename L>
inline constexpr auto type_list_size = type_list_size_helper<L>::value;
template <typename T, template <typename> typename Apply>
struct type_list_for_each_helper;
template <typename H, typename T, template <typename> typename Apply>
struct type_list_for_each_helper<type_list<H, T>, Apply>
{
    using type = type_list<typename Apply<H>::type,
                           typename type_list_for_each_helper<T, Apply>::type>;
};
template <template <typename> typename Apply>
struct type_list_for_each_helper<null_t, Apply>
{
    using type = null_t;
};
template <typename T, template <typename> typename Apply>
using type_list_for_each = typename type_list_for_each_helper<T, Apply>::type;

template <typename T, typename val, int idx = 0>
struct type_list_find_helper;
template <typename Tail, typename V, int idx>
struct type_list_find_helper<type_list<V, Tail>, V, idx>
{
    static constexpr int value = idx;
};
template <typename H, typename T, typename V, int idx>
struct type_list_find_helper<type_list<H, T>, V, idx>:
    type_list_find_helper<T, V, idx + 1>
{};
template <typename val, int idx>
struct type_list_find_helper<null_t, val, idx>
{
    static constexpr int value = -1; // not fount
};
template <typename T, typename val>
inline constexpr int type_list_find = type_list_find_helper<T, val>::value;

template <typename T, int N>
struct type_list_get_helper;
template <typename H, typename T, int N>
struct type_list_get_helper<type_list<H, T>, N>: type_list_get_helper<T, N - 1>
{};
template <typename H, typename T>
struct type_list_get_helper<type_list<H, T>, 0>
{
    using type = H;
};
template <int N>
struct type_list_get_helper<null_t, N>
{};
template <>
struct type_list_get_helper<null_t, 0>
{};
template <typename L, int N>
using type_list_get = typename type_list_get_helper<L, N>::type;