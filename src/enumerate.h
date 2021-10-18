#pragma once

#include <tuple>

namespace impl {

template <typename Iter>
struct IteratorWrapper {
  Iter iterator_;
  size_t pos_{0};

  constexpr auto &operator++() {
    ++pos_;
    ++iterator_;
    return *this;
  }
  constexpr std::tuple<const size_t, decltype(*iterator_)> operator*() const { return {pos_, *iterator_}; }
  constexpr std::tuple<const size_t, decltype(*iterator_)> operator*() { return {pos_, *iterator_}; }

  constexpr bool operator==(const IteratorWrapper &other) const { return iterator_ == other.iterator_; }
  constexpr bool operator!=(const IteratorWrapper &other) const { return !(iterator_ == other.iterator_); }
};

template <typename Container, typename Iter = decltype(std::begin(std::declval<Container>())),
          typename = decltype(std::end(std::declval<Container>()))>
struct ContainerWrapper {
  using Iterator = IteratorWrapper<Iter>;
  Container container_;

  constexpr auto begin() { return Iterator{std::begin(container_), 0}; }
  constexpr auto end() { return Iterator{std::end(container_), 0}; }
  constexpr auto begin() const { return Iterator{std::begin(container_), 0}; }
  constexpr auto end() const { return Iterator{std::end(container_), 0}; }
};
}  // namespace impl

template <typename Container>
constexpr auto Enumerate(Container &&iterable) {
  return impl::ContainerWrapper<Container>{std::forward<Container>(iterable)};
}
