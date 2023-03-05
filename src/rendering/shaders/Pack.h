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

#include <array>
#include <cstddef>

// TODO move to another place?

// Simply class used to keep different variadic template packs separate
template <class... Ts>
class Pack {};

template <class A, class... B>
concept OneOf = (std::same_as<A, B> || ...);

namespace detail {

template <class A, class B>
struct is_subset {
  static constexpr bool val = false;
};
template <class... B, OneOf<B...>... A>
struct is_subset<Pack<A...>, Pack<B...>> {
  static constexpr bool val = true;
};

} // namespace detail

template <typename A, typename B>
concept PackSubset = detail::is_subset<A, B>::val;

template <typename A, typename B>
concept PackSuperset = detail::is_subset<B, A>::val;

template <typename A, typename B>
concept PackPermutation = requires {
                            requires PackSubset<A, B>;
                            requires PackSubset<B, A>;
                          };

namespace detail {

template <class Head, class Tail>
struct unique_helper;
template <class... Head>
struct unique_helper<Pack<Head...>, Pack<>> {
  using type = Pack<Head...>;
};
template <class... Head, class Current, class... Tail>
struct unique_helper<Pack<Head...>, Pack<Current, Tail...>> {
  using type = typename std::conditional<
      is_subset<Pack<Current>, Pack<Head...>>::val,
      typename unique_helper<Pack<Head...>, Pack<Tail...>>::type,
      typename unique_helper<Pack<Head..., Current>, Pack<Tail...>>::type>::type;
};

} // namespace detail

template <class Pack>
struct unique;
template <class A>
struct unique<Pack<A>> {
  using type = Pack<A>;
};
template <class A, class B, class... N>
struct unique<Pack<A, B, N...>> {
  using type = typename detail::unique_helper<Pack<A>, Pack<B, N...>>::type;
};

template <class... Sets>
struct set_union;
template <class... Set1>
struct set_union<Pack<Set1...>> {
  using type = typename unique<Pack<Set1...>>::type;
};
template <class... Set1, class... Set2, class... SetN>
struct set_union<Pack<Set1...>, Pack<Set2...>, SetN...> {
  using type = typename set_union<Pack<Set1..., Set2...>, SetN...>::type;
};

#endif /* RENDERING_SHADERS_PACK_H */
