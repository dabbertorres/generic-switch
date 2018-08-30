#pragma once

#include <functional>
#include <type_traits>
#include <utility>

/*
Example use:
struct foo
{
    int v;
};

bool operator==(const foo& lhs, const foo& rhs)
{
    return lhs.v == rhs.v;
}

...

foo bar = {42};

switch_on(bar,
    foo{41}, []() { std::cout << "it's 41!"; },
    foo{42}, [](auto& f) { std::cout << f.v << ": Answer to the Ultimate Question of Life, the Universe and Everything"; },
    foo{43}, []() { std::cout << "it's 43!"; },
    [](auto f) { std::cout << "something else: " << f.v; });
*/

// recurse through cases until we find a match (or don't)
template<typename T, typename Y, typename Callable, typename... Rest>
constexpr void switch_on(T&& val, Y&& _case, Callable&& do_if, Rest&&... rest)
{
    static_assert(std::is_same_v<std::remove_reference_t<std::remove_cv_t<T>>, std::remove_reference_t<std::remove_cv_t<Y>>>, "types do not match");

    if (val == _case)
    {
        if constexpr (std::is_invocable_v<Callable, T>)
            std::invoke(do_if, std::forward<T>(val));
        else
            std::invoke(do_if);
    }
    else
    {
        switch_on(std::forward<T>(val), std::forward<Rest>(rest)...);
    }
}

// default case if there is a Callable at the end with no matching case
template<typename T, typename Callable>
constexpr void switch_on(T&& val, Callable&& do_default)
{
    if constexpr (std::is_invocable_v<Callable, T>)
        std::invoke(do_default, std::forward<T>(val));
    else
        std::invoke(do_default);
}

// no more cases
template<typename T>
constexpr void switch_on(const T&) {}
