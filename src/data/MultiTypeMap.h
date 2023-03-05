/*
 * File:   MultiTypeMape.h
 * Author: bbyington
 *
 * Created on Jan 5, 2019, 12:29 PM
 */

#include "rendering/shaders/Pack.h"

namespace detail {

template <typename KeyType, typename ValueType>
class KVPair {
public:
  KVPair() = default;
  KVPair(const KVPair&) = delete;
  KVPair(KVPair&&) = default;
  KVPair& operator=(const KVPair&) = delete;
  KVPair& operator=(KVPair&&) = default;

  const ValueType& Val() const { return data_; }
  ValueType& Val() { return data_; }
  const KeyType& Key() const { return key_; }
  KeyType& Key() { return key_; }

private:
  ValueType data_;
  KeyType key_;
};

} // namespace detail

template <typename KeyPack, typename ValuePack>
class MultiTypeMap;
template <typename... Keys, typename... Values>
class MultiTypeMap<Pack<Keys...>, Pack<Values...>> : public detail::KVPair<Keys, Values>... {
public:
  template <typename K, typename V>
    requires std::derived_from<MultiTypeMap, detail::KVPair<K, V>>
  V& Val() {
    return static_cast<detail::KVPair<K, V>*>(this)->Val();
  }

  template <typename K, typename V>
    requires std::derived_from<MultiTypeMap, detail::KVPair<K, V>>
  const V& Val() const {
    return static_cast<const detail::KVPair<K, V>*>(this)->Val();
  }

  template <typename K, typename V>
    requires std::derived_from<MultiTypeMap, detail::KVPair<K, V>>
  K& Key() {
    return static_cast<detail::KVPair<K, V>*>(this)->Key();
  }

  template <typename K, typename V>
    requires std::derived_from<MultiTypeMap, detail::KVPair<K, V>>
  const K& Key() const {
    return static_cast<const detail::KVPair<K, V>*>(this)->Key();
  }

  template <typename Functor>
  void ForEachVal(Functor&& func) {
    auto worker = {(func(Val<Keys, Values>()), 1)...};
    (void)worker;
  }

  template <typename Functor>
  void ForEachKeyVal(Functor&& func) {
    auto worker = {(func(Key<Keys, Values>(), Val<Keys, Values>()), 1)...};
    (void)worker;
  }
};
