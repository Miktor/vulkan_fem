#pragma once

#include <tuple>

namespace impl {

template <typename Iter>
struct IteratorWrapper {
  Iter iterator;
  size_t pos{0};

  constexpr auto &operator++() {
    ++pos;
    ++iterator;
    return *this;
  }
  constexpr std::tuple<const size_t, decltype(*iterator)> operator*() const { return {pos, *iterator}; }
  constexpr std::tuple<const size_t, decltype(*iterator)> operator*() { return {pos, *iterator}; }

  constexpr bool operator==(const IteratorWrapper &other) const { return iterator == other.iterator; }
  constexpr bool operator!=(const IteratorWrapper &other) const { return !(iterator == other.iterator); }
};

template <typename Container, typename Iter = decltype(std::begin(std::declval<Container>())),
          typename = decltype(std::end(std::declval<Container>()))>
struct ContainerWrapper {
  using Iterator = IteratorWrapper<Iter>;
  Container container;

  constexpr auto begin() { return Iterator{std::begin(container), 0}; }
  constexpr auto end() { return Iterator{std::end(container), 0}; }
  constexpr auto begin() const { return Iterator{std::begin(container), 0}; }
  constexpr auto end() const { return Iterator{std::end(container), 0}; }
};
}  // namespace impl

template <typename Container>
constexpr auto enumerate(Container &&iterable) {
  return impl::ContainerWrapper<Container>{std::forward<Container>(iterable)};
}
