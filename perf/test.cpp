// #include "../include/Common/Tlist.hpp"
#include <type_traits>
#if __has_include("target.hpp")
#    include "target.hpp"
#else
#    include "Tlist_variadic.hpp"
using target           = type_list<int, int, float, int, int, double>;
inline constexpr int N = 10;
inline constexpr int S = 0;
inline constexpr int M = 2;
inline constexpr int E = 4;
#endif
template <typename T>
struct id
{
    using type = T;
};
int main()
{
    static_assert(std::is_same_v<type_list_top<target>, int>);
    static_assert(!std::is_same_v<type_list_tail<target>, int>);
    static_assert(!std::is_same_v<type_list_push_back<target, int>, int>);
    static_assert(!std::is_same_v<type_list_push_front<target, int>, int>);
    static_assert(!std::is_same_v<type_list_push_back_n<target, int, N>, int>);
    static_assert(!std::is_same_v<type_list_push_back_m<target, target>, int>);
    static_assert(!std::is_same_v<type_list_push_back_m<target, int>, int>);
    static_assert(type_list_size<target> >= 0);
    static_assert(std::is_same_v<type_list_for_each<target, id>, target>);
    static_assert(type_list_find<target, int> >= 0);
    static_assert(type_list_find<target, float> >= 0);  // middle
    static_assert(type_list_find<target, double> >= 0); // end
    static_assert(type_list_find<target, long> < 0);    // not exist
    static_assert(!std::is_same_v<type_list_get<target, S>, void>);
    static_assert(!std::is_same_v<type_list_get<target, M>, void>);
    static_assert(!std::is_same_v<type_list_get<target, E>, void>);
}