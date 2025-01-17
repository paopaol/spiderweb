#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stack>
#include <unordered_map>
#include <vector>

namespace spiderweb {

enum class TreeWalkPolicy : uint8_t {
  kLevelOrder,
  kPreOrder,
};

enum class TreeWalkStep : uint8_t {
  kContinueWalk,
  kStopWalk,
};

namespace detail {
template <typename T, TreeWalkPolicy Policy>
struct WalkPolicyImpl;
};

template <typename T>
class TreeIterator;

template <typename T, TreeWalkPolicy Policy>
class TreeWalker;

template <typename T>
class Tree {
 public:
  struct Node {
    Node *child = nullptr;
    Node *next = nullptr;
    Node *prev = nullptr;
    Node *parent = nullptr;

    T v;
  };

  Tree();

  explicit Tree(T v);

  ~Tree();

  Tree(const Tree &) = delete;

  Tree &operator=(const Tree &) = delete;

  Tree(Tree &&other) noexcept;

  Tree &operator=(Tree &&other) noexcept;

  void SetRootValue(T &&v);

  TreeIterator<T> Root();

 private:
  void FreeNode(Node **node);

  Node *root_ = nullptr;

  friend class TreeIterator<T>;
};

template <typename T>
class TreeIterator {
 public:
  using NodeType = typename Tree<T>::Node;

  TreeIterator() = default;

  template <typename U>
  TreeIterator AddChild(U &&v);

  template <typename U>
  TreeIterator AddChild(Tree<U> &&tree);

  bool IsNull();

  bool HasChild() const;

  TreeIterator Parent() const;

  const NodeType *GetNode() const;

  T *operator->();

  const T *operator->() const;

  T *Get();

  const T *Get() const;

  bool operator==(const TreeIterator &other) const;

 private:
  explicit TreeIterator(NodeType *n);

  TreeIterator AddChildNode(NodeType *n);

  NodeType *node = nullptr;

  friend class Tree<T>;

  template <typename U, TreeWalkPolicy Policy>
  friend class TreeWalker;
};

template <typename T, TreeWalkPolicy Policy>
class TreeWalker {
 public:
  using Iterator = TreeIterator<T>;
  using NodeType = typename Tree<T>::Node;

  struct WalkNode {
    uint32_t  depth = 0;
    NodeType *node = nullptr;
  };

  template <typename F>
  void Walk(const Iterator &iter, F f) const;

  template <typename F>
  Iterator FindIf(const Iterator &iter, F f) const;
};

template <typename T>
Tree<T>::Tree() : root_(new Node) {
  root_->next = root_;
  root_->prev = root_;
}

template <typename T>
Tree<T>::Tree(T v) : root_(new Node) {
  root_->v = std::move(v);
  root_->next = root_;
  root_->prev = root_;
}

template <typename T>
Tree<T>::~Tree() {
  if (root_) {
    FreeNode(&root_);
  }
}

template <typename T>
Tree<T>::Tree(Tree &&other) noexcept {
  root_ = other.root_;
  other.root_ = nullptr;
}

template <typename T>
Tree<T> &Tree<T>::operator=(Tree &&other) noexcept {
  if (this == &other) {
    return *this;
  }

  root_ = other.root_;
  other.root_ = nullptr;

  return *this;
}

template <typename T>
void Tree<T>::SetRootValue(T &&v) {
  root_->v = std::forward<T>(v);
}

template <typename T>
TreeIterator<T> Tree<T>::Root() {
  return TreeIterator<T>(root_);
}

template <typename T>
void Tree<T>::FreeNode(Node **node) {
  TreeWalker<T, TreeWalkPolicy::kLevelOrder> walker;

  std::vector<TreeIterator<T>> nodes;
  walker.Walk(TreeIterator<T>(*node), [&](uint32_t, TreeIterator<T> &iter) {
    nodes.push_back(iter);
    return TreeWalkStep::kContinueWalk;
  });
  for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
    delete it->node;
  }
  *node = nullptr;
}

template <typename T>
TreeIterator<T>::TreeIterator(NodeType *n) : node(n) {
}

template <typename T>
template <typename U>
TreeIterator<T> TreeIterator<T>::AddChild(U &&v) {
  assert(node);

  auto *n = new NodeType;
  n->v = std::forward<U>(v);

  return AddChildNode(n);
}

template <typename T>
template <typename U>
TreeIterator<T> TreeIterator<T>::AddChild(Tree<U> &&tree) {
  auto *n = tree.root_;
  tree.root_ = nullptr;
  auto iter = AddChildNode(n);
  return iter;
}

template <typename T>
TreeIterator<T> TreeIterator<T>::AddChildNode(NodeType *n) {
  assert(node);

  n->parent = node;

  auto **head = &(node->child);
  if (!*head) {
    *head = n;
  } else {
    auto *last = (*head)->prev;

    last->next = n;
    n->prev = last;
  }

  (*head)->prev = n;
  n->next = *head;

  return TreeIterator(n);
}

template <typename T>
bool TreeIterator<T>::IsNull() {
  return node == nullptr;
}

template <typename T>
bool TreeIterator<T>::HasChild() const {
  return node->child != nullptr;
}

template <typename T>
TreeIterator<T> TreeIterator<T>::Parent() const {
  return TreeIterator<T>(node->parent);
}

template <typename T>
const typename Tree<T>::Node *TreeIterator<T>::GetNode() const {
  return node;
}

template <typename T>
T *TreeIterator<T>::operator->() {
  return &node->v;
}

template <typename T>
const T *TreeIterator<T>::operator->() const {
  return &node->v;
}

template <typename T>
T *TreeIterator<T>::Get() {
  return &node->v;
}

template <typename T>
const T *TreeIterator<T>::Get() const {
  return &node->v;
}

template <typename T>
bool TreeIterator<T>::operator==(const TreeIterator &other) const {
  return node == other.node;
}

namespace detail {
template <typename T, TreeWalkPolicy Policy>
struct WalkPolicyImpl {
  using WalkNode = typename TreeWalker<T, Policy>::WalkNode;

  using Iterator = typename TreeWalker<T, Policy>::Iterator;

  void PushNode(WalkNode &&node);

  void PushChild(uint32_t child_detph, Iterator child);

  WalkNode TakeFirst();

  bool Empty() const;
};
}  // namespace detail

template <typename T, TreeWalkPolicy Policy>
template <typename F>
void TreeWalker<T, Policy>::Walk(const Iterator &iter, F f) const {
  detail::WalkPolicyImpl<T, Policy> policy;

  policy.PushNode(WalkNode{0, iter.node});

  while (!policy.Empty()) {
    WalkNode current_node = policy.TakeFirst();
    Iterator current(current_node.node);

    auto state = f(current_node.depth, current);
    if (state == TreeWalkStep::kStopWalk) {
      return;
    }

    if (!current.HasChild()) {
      continue;
    }

    policy.PushChild(current_node.depth, current);
  }
}

template <typename T, TreeWalkPolicy Policy>
template <typename F>
typename TreeWalker<T, Policy>::Iterator TreeWalker<T, Policy>::FindIf(const Iterator &iter,
                                                                       F               f) const {
  Iterator found(nullptr);

  Walk(iter, [&](uint32_t, Iterator &it) {
    if (f(it)) {
      found = it;
      return TreeWalkStep::kStopWalk;
    }
    return TreeWalkStep::kContinueWalk;
  });
  return found;
}

namespace detail {
template <typename T>
struct WalkPolicyImpl<T, TreeWalkPolicy::kLevelOrder> {
  using WalkNode = typename TreeWalker<T, TreeWalkPolicy::kLevelOrder>::WalkNode;
  using Iterator = typename TreeWalker<T, TreeWalkPolicy::kLevelOrder>::Iterator;

  std::deque<WalkNode> queue;

  void PushNode(WalkNode node) {
    queue.push_back(std::move(node));
  }

  WalkNode TakeFirst() {
    auto node = queue.front();
    queue.pop_front();
    return std::move(node);
  }

  bool Empty() const {
    return queue.empty();
  }

  void PushChild(uint32_t child_detph, Iterator child) {
    auto *first = child.GetNode()->child;
    auto *next = first;

    do {
      queue.push_back({child_detph + 1, next});
      next = next->next;
    } while (next != first);
  }
};

template <typename T>
struct WalkPolicyImpl<T, TreeWalkPolicy::kPreOrder> {
  using WalkNode = typename TreeWalker<T, TreeWalkPolicy::kPreOrder>::WalkNode;
  using Iterator = typename TreeWalker<T, TreeWalkPolicy::kPreOrder>::Iterator;

  std::stack<WalkNode> stack;

  void PushNode(WalkNode node) {
    stack.push(std::move(node));
  }

  WalkNode TakeFirst() {
    auto node = stack.top();
    stack.pop();
    return std::move(node);
  }

  bool Empty() const {
    return stack.empty();
  }

  void PushChild(uint32_t child_detph, Iterator child) {
    auto *last = child.GetNode()->child->prev;
    auto *prev = last;

    do {
      stack.push({child_detph + 1, prev});
      prev = prev->prev;
    } while (prev != last);
  }
};
}  // namespace detail

}  // namespace spiderweb

template <typename T>
struct std::hash<spiderweb::TreeIterator<T>> {
  std::size_t operator()(const spiderweb::TreeIterator<T> &iter) const noexcept {
    return std::hash<decltype(iter.GetNode())>()(iter.GetNode());
  }
};
