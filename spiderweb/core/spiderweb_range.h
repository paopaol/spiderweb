#pragma once

#include <algorithm>
#include <utility>

namespace spiderweb {

struct Range {
  template <typename Con>
  using Iter = decltype(std::declval<Con>().begin());

  template <typename Con>
  using DifferenceType = typename std::iterator_traits<Iter<Con>>::difference_type;
};

template <typename Con, typename F>
static bool AllOf(Con&& con, F&& f) {
  return std::all_of(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                     std::forward<F>(f));
}

template <typename Con, typename F>
static bool AnyOf(Con&& con, F&& f) {
  return std::any_of(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                     std::forward<F>(f));
}

template <typename Con, typename F>
static bool NoneOf(Con&& con, F&& f) {
  return std::none_of(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                      std::forward<F>(f));
}

template <typename Con, typename F>
static void ForEach(Con&& con, F&& f) {
  std::for_each(std::forward<Con>(con).begin(), std::forward<Con>(con).end(), std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> Find(Con&& con, F&& f) {
  return std::find(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                   std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> FindIf(Con&& con, F&& f) {
  return std::find_if(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                      std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> FindIfNot(Con&& con, F&& f) {
  return std::find_if_not(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                          std::forward<F>(f));
}

template <typename Con, typename U>
static Range::DifferenceType<Con> Count(Con&& con, U&& value) {
  return std::count(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                    std::forward<U>(value));
}

template <typename Con, typename F>
static Range::DifferenceType<Con> CountIf(Con&& con, F&& f) {
  return std::count_if(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                       std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> SortHeap(Con&& con, F&& f) {
  return std::sort_heap(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                        std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> Sort(Con&& con, F&& f) {
  return std::sort(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                   std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> LowerBound(Con&& con, F&& f) {
  return std::lower_bound(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                          std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> UpperBound(Con&& con, F&& f) {
  return std::upper_bound(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                          std::forward<F>(f));
}

template <typename Con, typename F>
static Range::Iter<Con> RemoveIf(Con&& con, F&& f) {
  return std::remove_if(std::forward<Con>(con).begin(), std::forward<Con>(con).end(),
                        std::forward<F>(f));
}

}  // namespace spiderweb
