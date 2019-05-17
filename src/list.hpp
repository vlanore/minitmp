/*Copyright or © or Copr. CNRS (2019). Contributors:
- Vincent Lanore. vincent.lanore@gmail.com

This software is a computer program whose purpose is to provide a header-only library with simple
template metaprogramming datastructures (list, map) and utilities.

This software is governed by the CeCILL-C license under French law and abiding by the rules of
distribution of free software. You can use, modify and/ or redistribute the software under the terms
of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy, modify and redistribute
granted by the license, users are provided only with a limited warranty and the software's author,
the holder of the economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated with loading, using,
modifying and/or developing or reproducing the software by the user in light of its specific status
of free software, that may mean that it is complicated to manipulate, and that also therefore means
that it is reserved for developers and experienced professionals having in-depth computer knowledge.
Users are therefore encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or data to be ensured and,
more generally, to use and operate it in the same conditions as regards security.

The fact that you are presently reading this means that you have had knowledge of the CeCILL-C
license and that you accept its terms.*/

#pragma once

#include <tuple>
#include "box.hpp"
#include "doctest.h"

namespace minimpl {
    // type tags
    struct List {};         // is a list
    struct NotAList {};     // passed a param that should have been a list but was not
    struct OutOfBounds {};  // requested index is out of bounds

    template <class... Elements>
    struct list : List {
        using boxes = std::tuple<box<Elements>...>;
        static constexpr size_t size = sizeof...(Elements);
    };

    template <class... Elements>
    constexpr size_t list<Elements...>::size;

    template <class T>
    using is_list = std::is_base_of<List, T>;

    template <class L, size_t index>
    struct get_element {
        static_assert(is_list<L>::value, "L is not a list");
        static_assert(index < L::size, "index out of bounds");
        using type = unbox_t<std::tuple_element_t<index, typename L::boxes>>;
    };

    template <class T, size_t index>
    using get_element_t = typename get_element<T, index>::type;

    template <class L, class ToFind>
    struct find_element {
        static_assert(is_list<L>::value, "parameter L is not a list");

        template <size_t index>
        static constexpr auto helper(std::tuple<>) {
            return index;  // return list size if not found
        }

        template <size_t index, class... Rest>
        static constexpr auto helper(std::tuple<box<ToFind>, Rest...>) {
            return index;
        }

        template <size_t index, class First, class... Rest>
        static constexpr auto helper(std::tuple<First, Rest...>) {
            return helper<index + 1>(std::tuple<Rest...>());
        }

        static constexpr size_t value = helper<0>(typename L::boxes());
        static_assert(value < L::size, "type not foud in list");
    };

    template <class L, class ToFind>
    constexpr size_t find_element<L, ToFind>::value;

};  // namespace minimpl

TEST_CASE("List tests") {
    using namespace minimpl;
    using l = list<int, double, char>;
    struct l2 {};  // not a list
    CHECK(l::size == 3);
    CHECK(std::is_same<get_element_t<l, 0>, int>::value);
    CHECK(std::is_same<get_element_t<l, 1>, double>::value);
    CHECK(std::is_same<get_element_t<l, 2>, char>::value);
    // CHECK(std::is_same<element_t<l, 3>, OutOfBounds>::value);
    // CHECK(std::is_same<element_t<l2, 1>, NotA<List>>::value);
    CHECK(find_element<l, int>::value == 0);
    CHECK(find_element<l, double>::value == 1);
    CHECK(find_element<l, char>::value == 2);
    // CHECK(find_element<l, long>::value == 3);
    // CHECK(find_element<l2, long>::value == 3);
}