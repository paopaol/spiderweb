#pragma once

#include <unordered_map>

namespace spiderweb {

template <typename T, typename K>
struct GenRightValueUniqueKey {
  K operator()(const T &v) {
    return v;
  }
};

template <typename K, typename V, typename RightValueKey = GenRightValueUniqueKey<V, V>>
class UnorderedBiMap {
 public:
  using left_key_type = K;
  using right_value_type = V;
  using right_key_type = decltype(std::declval<RightValueKey>()(std::declval<V>()));
  using element_type = std::pair<left_key_type, right_key_type>;

  struct StoreValue {
    StoreValue(left_key_type k, right_value_type v)
        : left_key(std::move(k)), right_value(std::move(v)) {
    }

    left_key_type    left_key;
    right_value_type right_value;
  };

  using left_map_type = std::unordered_map<left_key_type, StoreValue *>;
  using left_const_iterator = typename left_map_type::const_iterator;

  using right_map_type = std::unordered_map<right_key_type, const StoreValue *>;
  using right_const_iterator = typename right_map_type::const_iterator;

  class InversedMap {
   public:
    explicit InversedMap(UnorderedBiMap *m) : view_(m) {
    }

    InversedMap(const InversedMap &) = delete;

    InversedMap &operator=(const InversedMap &) = delete;

    InversedMap(InversedMap &&other) noexcept : view_(other.view_) {
      other.view_ = nullptr;
    }

    InversedMap &operator=(InversedMap &&other) noexcept {
      if (&other == this) {
        return *this;
      }

      view_ = other.view_;
      other.view_ = nullptr;
      return *this;
    }

    std::size_t Size() const;

    bool Empty() const;

    void Clear();

    void Erase(const right_key_type &key);

    right_const_iterator Find(const right_key_type &right_key) const;

    right_const_iterator begin() const;

    right_const_iterator end() const;

   private:
    UnorderedBiMap *view_ = nullptr;

    friend class UnorderedBiMap;
  };

  UnorderedBiMap() = default;

  ~UnorderedBiMap();

  UnorderedBiMap(const UnorderedBiMap &other) = delete;

  UnorderedBiMap &operator=(const UnorderedBiMap &other) = delete;

  void Set(left_key_type key, right_value_type &&value);

  std::size_t Size() const;

  bool Empty() const;

  void Clear();

  void Erase(const left_key_type &key);

  left_const_iterator Find(const left_key_type &left_key) const;

  left_const_iterator begin() const;

  left_const_iterator end() const;

  InversedMap Inserse();

 private:
  StoreValue *InsertLeft(left_key_type key, right_value_type &&value);

  void InsertRight(StoreValue *value);

  left_map_type  left_;
  right_map_type right_;
};

template <typename K, typename V, typename RightValueKey>
UnorderedBiMap<K, V, RightValueKey>::~UnorderedBiMap() {
  Clear();
}

template <typename K, typename V, typename RightValueKey>
std::size_t UnorderedBiMap<K, V, RightValueKey>::InversedMap::Size() const {
  return view_->Size();
}

template <typename K, typename V, typename RightValueKey>
bool UnorderedBiMap<K, V, RightValueKey>::InversedMap::Empty() const {
  return view_->Empty();
}

template <typename K, typename V, typename RightValueKey>
void UnorderedBiMap<K, V, RightValueKey>::InversedMap::Clear() {
  view_->Clear();
}

template <typename K, typename V, typename RightValueKey>
void UnorderedBiMap<K, V, RightValueKey>::InversedMap::Erase(const right_key_type &key) {
  auto it = view_->right_.find(key);
  if (it != view_->right_.end()) {
    const auto left_key = it->second->left_key;
    view_->Erase(left_key);
  }
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::right_const_iterator
UnorderedBiMap<K, V, RightValueKey>::InversedMap::Find(const right_key_type &right_key) const {
  return view_->right_.find(right_key);
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::right_const_iterator
UnorderedBiMap<K, V, RightValueKey>::InversedMap::begin() const {
  return view_->right_.begin();
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::right_const_iterator
UnorderedBiMap<K, V, RightValueKey>::InversedMap::end() const {
  return view_->right_.end();
}

template <typename K, typename V, typename RightValueKey>
void UnorderedBiMap<K, V, RightValueKey>::Set(left_key_type key, right_value_type &&value) {
  InsertRight(InsertLeft(key, std::forward<right_value_type>(value)));
}

template <typename K, typename V, typename RightValueKey>
std::size_t UnorderedBiMap<K, V, RightValueKey>::Size() const {
  return left_.size();
}
template <typename K, typename V, typename RightValueKey>
bool UnorderedBiMap<K, V, RightValueKey>::Empty() const {
  return left_.empty();
}

template <typename K, typename V, typename RightValueKey>
void UnorderedBiMap<K, V, RightValueKey>::Clear() {
  for (const auto &it : left_) {
    delete it.second;
  }
  left_.clear();
  right_.clear();
}

template <typename K, typename V, typename RightValueKey>
void UnorderedBiMap<K, V, RightValueKey>::Erase(const left_key_type &key) {
  auto it = left_.find(key);
  if (it != left_.end()) {
    right_.erase(RightValueKey()(it->second->right_value));

    delete it->second;
  }
  left_.erase(it);
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::left_const_iterator
UnorderedBiMap<K, V, RightValueKey>::Find(const left_key_type &left_key) const {
  return left_.find(left_key);
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::left_const_iterator
UnorderedBiMap<K, V, RightValueKey>::begin() const {
  return left_.begin();
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::left_const_iterator
UnorderedBiMap<K, V, RightValueKey>::end() const {
  return left_.end();
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::InversedMap
UnorderedBiMap<K, V, RightValueKey>::Inserse() {
  return InversedMap(this);
}

template <typename K, typename V, typename RightValueKey>
typename UnorderedBiMap<K, V, RightValueKey>::StoreValue *
UnorderedBiMap<K, V, RightValueKey>::InsertLeft(left_key_type left_key, right_value_type &&value) {
  auto it = left_.insert({left_key, nullptr});
  if (it.second) {
    it.first->second = new StoreValue(left_key, std::forward<right_value_type>(value));
    return it.first->second;
  }

  right_.erase(RightValueKey()(it.first->second->right_value));
  *it.first->second = StoreValue(left_key, std::forward<right_value_type>(value));
  return it.first->second;
}

template <typename K, typename V, typename RightValueKey>
void UnorderedBiMap<K, V, RightValueKey>::InsertRight(StoreValue *v) {
  auto it = right_.insert({RightValueKey()(v->right_value), v});
  if (!it.second) {
    const auto left_key = it.first->second->left_key;
    delete it.first->second;
    left_.erase(left_key);
    it.first->second = v;
  }
}

}  // namespace spiderweb
