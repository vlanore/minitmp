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

#include <type_traits>
#include "doctest.h"

namespace minimpl {

    // type tags
    struct Error {};  // tag for error types
    template <class T>
    struct NotA : Error {};  // error : wrong type
    struct NoError {};       // to used in place of error when no error
    struct Invalid {};       // to build default values

    // type traits
    template <class Tag, class T>
    using has_tag = std::is_base_of<Tag, T>;

    template <class T>
    using is_error = has_tag<Error, T>;

    // maybe class : represents either a value or an error
    template <class Tag, class DefaultValue>
    struct maybe_builder {
        template <class T, bool has_tag = has_tag<Tag, T>::value>
        struct make {
            using error = NotA<Tag>;
            using value = DefaultValue;
        };

        template <class T>
        struct make<T, true> {
            using error = NoError;
            using value = T;
        };
    };

    // maybe class operations
    template <class MX>
    using is_valid = std::is_same<NoError, typename MX::error>;

    template <class MX>
    using get_value = typename MX::value;

    template <class MX>
    using get_error = typename MX::error;

    template <class MX>
    using get = std::conditional_t<is_valid<MX>::value, get_value<MX>, get_error<MX>>;

    template <template <class...> class F, class MX, class... Args>
    using apply = std::conditional_t<is_valid<MX>::value,
                                     typename F<get_value<MX>, Args...>::result, get_error<MX>>;

};  // namespace minimpl

//==================================================================================================
// TESTS
namespace testing {
    using namespace minimpl;

    struct MyType {};

    template <class T>
    struct my_type : MyType {
        using type = T;
    };

    template <class T>
    struct f {
        using result = my_type<T>;
        using return_type = MyType;
    };

    template <class T>
    using maybe_mytype = maybe_builder<MyType, my_type<Invalid>>::make<T>;
};  // namespace testing

TEST_CASE("Maybe test") {
    using namespace minimpl;
    using namespace testing;

    using t1 = maybe_mytype<double>;
    using t2 = maybe_mytype<my_type<double>>;
    CHECK(not is_valid<t1>::value);
    CHECK(is_valid<t2>::value);
    CHECK(std::is_same<apply<f, t1>, NotA<MyType>>::value);
    CHECK(std::is_same<apply<f, t2>, my_type<my_type<double>>>::value);
}