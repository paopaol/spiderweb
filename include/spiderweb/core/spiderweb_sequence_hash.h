#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

namespace spiderweb {

template <typename T>
struct SequenceHashNode {
  T                 v;
  SequenceHashNode* p = nullptr;
  SequenceHashNode* n = nullptr;
};

///////////////

template <typename N, typename T, bool IsConst = false>
struct SequenceHashIterator {
  using element_type = typename std::conditional<IsConst, const N, N>::type;
  using value_type = typename std::conditional<IsConst, const T, T>::type;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;
  using difference_type = std::ptrdiff_t;

  explicit SequenceHashIterator(element_type* p);

  SequenceHashIterator& operator=(const SequenceHashIterator& o);

  bool operator==(const SequenceHashIterator& o) const;

  bool operator!=(const SequenceHashIterator& o) const;

  SequenceHashIterator& operator++();

  SequenceHashIterator operator++(int);

  SequenceHashIterator& operator--();

  SequenceHashIterator operator--(int);

  reference operator*();

  pointer operator->();

  pointer operator->() const;

 private:
  element_type* ptr = nullptr;

  template <typename K, typename V, template <typename, typename, typename...> class Map>
  friend class SequenceHash;
};

/**
 * @brief
 *
 * SequenceHash is a container data structure that combines hash
 *
 * table and linear ordering. It can be thought of as a non-cyclic
 *
 * doubly linked list, meaning that elements are sorted in the order
 *
 * of insertion. However, unlike traditional linked lists, SequenceHash
 *
 * requires a unique key for each element, allowing for an internal
 *
 * hash table (for fast lookups, modifications, and deletions). When
 *
 * traversing a SequenceHash, elements are output in the order they
 *
 * were inserted.
 *
 * Note: Do not use sorting; sorting behavior is currently undefined.
 */
template <typename K, typename T,
          template <typename, typename, typename...> class Map = std::unordered_map>
class SequenceHash {
 public:
  using key_type = typename std::decay<K>::type;
  using node_type = SequenceHashNode<T>;
  using value_type = std::pair<const key_type, T>;

  using Iterator = SequenceHashIterator<node_type, T>;

  using ConstIterator = SequenceHashIterator<node_type, T, true>;

  SequenceHash();

  SequenceHash(std::initializer_list<value_type> list);

  SequenceHash(SequenceHash&&) noexcept;

  SequenceHash& operator=(SequenceHash&&) noexcept;

  SequenceHash(const SequenceHash&) = delete;

  SequenceHash& operator=(const SequenceHash&) = delete;

  ~SequenceHash();

  bool PushBack(key_type key, T v);

  std::pair<Iterator, bool> TryEmplace(key_type key, T v);

  bool PushAfter(key_type at, key_type key, T v);

  bool PushFront(key_type key, T v);

  bool PushBefore(key_type at, key_type key, T v);

  void Remove(const key_type& key);

  void Clear();

  void Reserve(uint32_t size);

  std::size_t Size() const;

  bool Empty() const;

  ConstIterator Find(const key_type& key) const;

  Iterator Find(const key_type& key);

  Iterator Forward(const key_type& current);

  ConstIterator Forward(const key_type& current) const;

  Iterator Backward(const key_type& current);

  ConstIterator Backward(const key_type& current) const;

  Iterator Front();

  ConstIterator Front() const;

  Iterator Back();

  ConstIterator Back() const;

  ConstIterator begin() const;

  ConstIterator end() const;

  Iterator begin();

  Iterator end();

  void Swap(SequenceHash& rh);

 private:
  void PushNode(node_type* node);

  void PushNode(node_type* before, node_type* node);

  template <typename M, typename U>
  node_type* InsertHash(M&& key, U&& v);

  Map<key_type, node_type*> nodes_;
  node_type*                tail_ = nullptr;
  node_type*                head_ = nullptr;
};

template <typename N, typename T, bool IsConst>
SequenceHashIterator<N, T, IsConst>::SequenceHashIterator(element_type* p) : ptr(p) {
}

template <typename N, typename T, bool IsConst>
SequenceHashIterator<N, T, IsConst>& SequenceHashIterator<N, T, IsConst>::operator=(
    const SequenceHashIterator& o) {
  if (this == &o) {
    return *this;
  }
  ptr = o.ptr;
  return *this;
}

template <typename N, typename T, bool IsConst>
bool SequenceHashIterator<N, T, IsConst>::operator==(const SequenceHashIterator& o) const {
  return this->ptr == o.ptr;
}

template <typename N, typename T, bool IsConst>
bool SequenceHashIterator<N, T, IsConst>::operator!=(const SequenceHashIterator& o) const {
  return this->ptr != o.ptr;
}

template <typename N, typename T, bool IsConst>
SequenceHashIterator<N, T, IsConst>& SequenceHashIterator<N, T, IsConst>::operator++() {
  if (!ptr) {
    throw std::runtime_error("out of range");
  }

  this->ptr = this->ptr->n;
  return *this;
}

template <typename N, typename T, bool IsConst>
SequenceHashIterator<N, T, IsConst> SequenceHashIterator<N, T, IsConst>::operator++(int) {
  auto tmp = *this;
  ++(*this);
  return tmp;
}

template <typename N, typename T, bool IsConst>
SequenceHashIterator<N, T, IsConst>& SequenceHashIterator<N, T, IsConst>::operator--() {
  if (!ptr) {
    throw std::runtime_error("out of range");
  }

  this->ptr = this->ptr->p;
  return *this;
}

template <typename N, typename T, bool IsConst>
SequenceHashIterator<N, T, IsConst> SequenceHashIterator<N, T, IsConst>::operator--(int) {
  auto tmp = *this;
  --(*this);
  return tmp;
}

template <typename N, typename T, bool IsConst>
typename SequenceHashIterator<N, T, IsConst>::reference
SequenceHashIterator<N, T, IsConst>::operator*() {
  if (!ptr) {
    throw std::runtime_error("out of range");
  }

  return this->ptr->v;
}

template <typename N, typename T, bool IsConst>
typename SequenceHashIterator<N, T, IsConst>::pointer
SequenceHashIterator<N, T, IsConst>::operator->() {
  if (!ptr) {
    throw std::runtime_error("out of range");
  }
  return &this->ptr->v;
}

template <typename N, typename T, bool IsConst>
typename SequenceHashIterator<N, T, IsConst>::pointer
SequenceHashIterator<N, T, IsConst>::operator->() const {
  if (!ptr) {
    throw std::runtime_error("out of range");
  }
  return &this->ptr->v;
}

//////////////////////////////////
template <typename K, typename T, template <typename, typename, typename...> class Map>
SequenceHash<K, T, Map>::SequenceHash() = default;

template <typename K, typename T, template <typename, typename, typename...> class Map>
SequenceHash<K, T, Map>::SequenceHash(std::initializer_list<value_type> list) {
  for (auto& v : list) {
    PushBack(v.first, std::move(v.second));
  }
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
SequenceHash<K, T, Map>::SequenceHash(SequenceHash&& other) noexcept {
  Swap(other);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
SequenceHash<K, T, Map>& SequenceHash<K, T, Map>::operator=(SequenceHash&& other) noexcept {
  if (this != &other) {
    Clear();
    Swap(other);
  }

  return *this;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
inline SequenceHash<K, T, Map>::~SequenceHash() {
  Clear();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
bool SequenceHash<K, T, Map>::PushBack(key_type key, T v) {
  auto* node = InsertHash(std::move(key), std::move(v));
  if (node) {
    PushNode(node);
  }
  return !!node;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
std::pair<typename SequenceHash<K, T, Map>::Iterator, bool> SequenceHash<K, T, Map>::TryEmplace(
    key_type key, T v) {
  auto result = nodes_.try_emplace(std::move(key), nullptr);
  if (!result.second) {
    return std::make_pair(Iterator(result.first->second), false);
  }

  auto* node = new node_type{std::forward<T>(v)};
  result.first->second = node;

  PushNode(node);
  return std::make_pair(Iterator(node), true);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
bool SequenceHash<K, T, Map>::PushAfter(key_type at, key_type key, T v) {
  auto before = Find(at);
  if (before == end()) {
    return false;
  }

  auto* node = InsertHash(std::move(key), std::move(v));
  if (node) {
    PushNode(before.ptr, node);
    return true;
  }
  return false;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
bool SequenceHash<K, T, Map>::PushFront(key_type key, T v) {
  //   head_
  // n head_
  auto* node = InsertHash(std::move(key), std::move(v));
  if (!node) {
    return false;
  }

  if (head_) {
    PushNode(node, head_);
    head_ = node;
  } else {
    PushNode(node);
  }

  return true;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
bool SequenceHash<K, T, Map>::PushBefore(key_type at, key_type key, T v) {
  //   before
  // n before
  auto before = Find(at);
  if (before == end()) {
    return false;
  }

  auto* node = InsertHash(std::move(key), std::move(v));
  if (!node) {
    return false;
  }

  if (before.ptr->p) {
    PushNode(before.ptr->p, node);
  } else {
    PushNode(node, head_);
    head_ = node;
  }

  return true;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
void SequenceHash<K, T, Map>::Remove(const key_type& key) {
  auto it = Find(key);
  if (it == end()) {
    return;
  }

  // p current n
  // p         n
  auto* current = it.ptr;

  if (current == head_) {
    head_ = current->n;
  }
  if (current == tail_) {
    tail_ = current->p;
  }

  if (current->p) {
    current->p->n = current->n;
  }
  if (current->n) {
    current->n->p = current->p;
  }

  nodes_.erase(key);
  delete current;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
void SequenceHash<K, T, Map>::PushNode(node_type* node) {
  if (head_) {
    node->p = tail_;
    node->n = tail_->n;

    tail_->n = node;
  } else {
    assert(!node->p && !node->n);

    head_ = node;
  }
  tail_ = node;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
void SequenceHash<K, T, Map>::PushNode(node_type* before, node_type* node) {
  assert(before && node);

  // before   next
  // before n next

  auto* next = before->n;
  node->n = next;
  if (next) {
    next->p = node;
  }

  before->n = node;
  node->p = before;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
template <typename M, typename U>
typename SequenceHash<K, T, Map>::node_type* SequenceHash<K, T, Map>::InsertHash(M&& key, U&& v) {
  node_type* node = nullptr;

  auto result = nodes_.try_emplace(std::forward<M>(key), nullptr);
  if (!result.second) {
    return nullptr;
  }

  node = new node_type{std::forward<U>(v)};
  result.first->second = node;
  return node;
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
void SequenceHash<K, T, Map>::Clear() {
  node_type* current = head_;
  while (current != nullptr) {
    node_type* next = current->n;
    delete current;
    current = next;
  }
  head_ = nullptr;
  tail_ = nullptr;
  nodes_.clear();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
void SequenceHash<K, T, Map>::Reserve(uint32_t size) {
  nodes_.reserve(size);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
std::size_t SequenceHash<K, T, Map>::Size() const {
  return nodes_.size();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
bool SequenceHash<K, T, Map>::Empty() const {
  return nodes_.empty();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::ConstIterator SequenceHash<K, T, Map>::Find(
    const key_type& key) const {
  auto it = nodes_.find(key);
  return it == nodes_.end() ? ConstIterator(nullptr) : ConstIterator(it->second);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::Iterator SequenceHash<K, T, Map>::Find(const key_type& key) {
  auto it = nodes_.find(key);
  return it == nodes_.end() ? Iterator(nullptr) : Iterator(it->second);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::Iterator SequenceHash<K, T, Map>::Forward(
    const key_type& current) {
  auto it = Find(current);
  return it != end() ? ++it : end();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::ConstIterator SequenceHash<K, T, Map>::Forward(
    const key_type& current) const {
  auto it = Find(current);
  return it != end() ? ++it : end();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::Iterator SequenceHash<K, T, Map>::Backward(
    const key_type& current) {
  auto it = Find(current);
  return it != end() ? --it : end();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::ConstIterator SequenceHash<K, T, Map>::Backward(
    const key_type& current) const {
  auto it = Find(current);
  return --it;
  return it != end() ? --it : end();
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::Iterator SequenceHash<K, T, Map>::Front() {
  return Iterator(head_);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::ConstIterator SequenceHash<K, T, Map>::Front() const {
  return ConstIterator(head_);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::Iterator SequenceHash<K, T, Map>::Back() {
  return Iterator(tail_);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::ConstIterator SequenceHash<K, T, Map>::Back() const {
  return ConstIterator(tail_);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::ConstIterator SequenceHash<K, T, Map>::begin() const {
  return ConstIterator(head_);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::ConstIterator SequenceHash<K, T, Map>::end() const {
  return ConstIterator(nullptr);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::Iterator SequenceHash<K, T, Map>::begin() {
  return Iterator(head_);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
typename SequenceHash<K, T, Map>::Iterator SequenceHash<K, T, Map>::end() {
  return Iterator(nullptr);
}

template <typename K, typename T, template <typename, typename, typename...> class Map>
void SequenceHash<K, T, Map>::Swap(SequenceHash& rh) {
  std::swap(nodes_, rh.nodes_);
  std::swap(head_, rh.head_);
  std::swap(tail_, rh.tail_);
}

}  // namespace spiderweb
