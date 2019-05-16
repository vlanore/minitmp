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
    struct NotFound {};     // could not find element

    template <class... Elements>
    struct list : List {
        using boxes = std::tuple<box<Elements>...>;
        static constexpr size_t size = sizeof...(Elements);
    };

    template <>
    struct default_value<List> {
        using type = list<>;
    };

    template <class... Elements>
    constexpr size_t list<Elements...>::size;

    template <class T>
    using is_list = has_tag<List, T>;

    using maybe_list = maybe<List>;

    template <class L, size_t index, bool within_bounds = (index < L::size)>
    struct get_element : Function<RawType> {
        using result = OutOfBounds;
    };

    template <class L, size_t index>
    struct get_element<L, index, true> : Function<RawType> {
        using result = unbox<std::tuple_element_t<index, typename L::boxes>>;
    };

    template <class T, size_t index>
    using element_t =
        std::conditional_t<is_valid<maybe_list::make<T>>::value,
                           typename get_element<get_value<maybe_list::make<T>>, index>::result,
                           get_error<maybe_list::make<T>>>;

};  // namespace minimpl

TEST_CASE("List tests") {
    using namespace minimpl;
    using l = list<int, double, char>;
    struct l2 {};  // not a list
    CHECK(l::size == 3);
    CHECK(std::is_same<element_t<l, 0>, int>::value);
    CHECK(std::is_same<element_t<l, 1>, double>::value);
    CHECK(std::is_same<element_t<l, 2>, char>::value);
    CHECK(std::is_same<element_t<l, 3>, OutOfBounds>::value);
    CHECK(std::is_same<element_t<l2, 1>, NotA<List>>::value);
}