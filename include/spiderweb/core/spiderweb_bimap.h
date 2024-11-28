#pragma once

#include <unordered_map>

namespace spiderweb {

template <typename K, typename V>
class UnorderedBiMap {
 public:
  using map_type = std::unordered_map<K, V>;
  using inversed_map_type = std::unordered_map<V, K>;

  using iterator = typename map_type::iterator;
  using const_iterator = typename map_type::const_iterator;
  using size_type = typename map_type::size_type;

  using inverse_iterator = typename inversed_map_type::iterator;
  using inverse_const_iterator = typename inversed_map_type::const_iterator;
  using inverse_size_type = typename inversed_map_type::size_type;

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

    inverse_const_iterator begin() const {
      return view_->inversed_map_.begin();
    }

    inverse_const_iterator end() const {
      return view_->inversed_map_.end();
    }

    inverse_const_iterator find(const K &key) const {
      return view_->inversed_map_.find(key);
    }

    void erase(inverse_iterator pos) {
      if (pos != view_->inversed_map_.end()) {
        view_->erase(pos->second);
      }
    }

    void erase(inverse_const_iterator pos) {
      if (pos != view_->inversed_map_.end()) {
        view_->erase(pos->second);
      }
    }

    void erase(const V &v) {
      erase(view_->inversed_map_.find(v));
    }

    size_type size() const {
      return view_->inversed_map_.size();
    }

    bool empty() const {
      return view_->inversed_map_.empty();
    }

    template <typename U>
    void insert(const K &key, U &&v) {
      view_->map_[v] = key;
      view_->inversed_map_[key] = std::forward<U>(v);
    }

   private:
    UnorderedBiMap *view_ = nullptr;

    friend class UnorderedBiMap;
  };

  UnorderedBiMap();

  ~UnorderedBiMap();

  const_iterator begin() const {
    return map_.begin();
  }

  const_iterator end() const {
    return map_.end();
  }

  iterator find(const K &key) {
    return map_.find(key);
  }

  const_iterator find(const K &key) const {
    return map_.find(key);
  }

  void erase(iterator pos) {
    if (pos != map_.end()) {
      inversed_map_.erase(pos->second);
      map_.erase(pos);
    }
  }

  void erase(const_iterator pos) {
    if (pos != map_.end()) {
      inversed_map_.erase(pos->second);
      map_.erase(pos);
    }
  }

  void erase(const K &k) {
    erase(map_.find(k));
  }

  size_type size() const {
    return map_.size();
  }

  bool empty() const {
    return map_.empty();
  }

  template <typename U>
  void insert(const K &key, U &&v) {
    inversed_map_[v] = key;
    map_[key] = std::forward<U>(v);
  }

  InversedMap Iverse() {
    return InversedMap(this);
  }

 private:
  map_type          map_;
  inversed_map_type inversed_map_;

  friend class InversedMap;
};

template <typename K, typename V>
UnorderedBiMap<K, V>::UnorderedBiMap() = default;

template <typename K, typename V>
UnorderedBiMap<K, V>::~UnorderedBiMap() = default;

}  // namespace spiderweb
