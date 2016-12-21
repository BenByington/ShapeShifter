/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   pack.h
 * Author: bbyington
 *
 * Created on November 16, 2016, 8:53 PM
 */

#ifndef RENDERING_SHADERS_PACK_H
#define RENDERING_SHADERS_PACK_H

// Simply class used to keep different variadic template packs separate
template <class ... Ts>
class Pack {};

namespace detail {

// Sees if class A exists in the variadic pack B.
template <class A, class B> struct subset_helper;
template <class A, class... B>
struct subset_helper<A, Pack<B...>> {
  static constexpr bool value() {
    const std::array<bool, sizeof...(B)> check = { std::is_same<A,B>::value... };
    auto ret = false;
    for (size_t i = 0; i < sizeof...(B); ++i) {
      ret |= check[i];
    }
    return ret;
  }
};

}

// Sees if all of the classes in the pack A exist in the pack B
template <class A, class B> struct is_subset;
template <class... A, class... B>
struct is_subset<Pack<A...>, Pack<B...>> {
  static constexpr bool value() {
    using other = Pack<B...>;
    const std::array<bool, sizeof...(A)> check = { detail::subset_helper<A, other>::value()... };
    auto ret = true;
    for (size_t i = 0; i < sizeof...(A); ++i) {
      ret &= check[i];
    }
    return ret;
  }
};

template <class A, class B> struct is_permutation {
  static constexpr bool value =
    is_subset<A,B>::value() && is_subset<B,A>::value();
};

namespace detail {

template <class Head, class Tail> struct unique_helper;
template <class... Head>
struct unique_helper<Pack<Head...>, Pack<>> {
  using type = Pack<Head...>;
};
template <class... Head, class Current, class... Tail>
struct unique_helper<Pack<Head...>, Pack<Current, Tail...>> {
  using type = typename std::conditional<
      is_subset<Pack<Current>, Pack<Head...>>::value(),
      typename unique_helper<Pack<Head...>, Pack<Tail...>>::type,
      typename unique_helper<Pack<Head..., Current>, Pack<Tail...>>::type
  >::type;
};

}

template <class Pack> struct unique;
template <class A>
struct unique<Pack<A>> {
  using type = Pack<A>;
};
template <class A, class B, class... N>
struct unique<Pack<A, B, N...>> {
  using type = typename detail::unique_helper<Pack<A>, Pack<B, N...>>::type;
};

template <class... Sets> struct set_union;
template <class... Set1>
struct set_union<Pack<Set1...>> {
  using type = typename unique<Pack<Set1...>>::type;
};
template <class... Set1, class... Set2, class... SetN>
struct set_union<Pack<Set1...>, Pack<Set2...>, SetN...> {
  using type = typename set_union<Pack<Set1..., Set2...>, SetN...>::type;
};

#endif /* RENDERING_SHADERS_PACK_H */
