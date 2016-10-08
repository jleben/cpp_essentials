/*
shareable: guaranteed mutually exclusive access to data

Copyright (C) 2016  Jakob Leben <jakob.leben@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include <iterator>
#include <tuple>

namespace essentials {

template<typename F, size_t I>
struct for_each
{
  static void apply(F & f)
  {
    for_each<F,I-1>::apply(f);
    f.template apply<I-1>();
  }
};

template<typename F>
struct for_each<F,1>
{
  static void apply(F & f)
  {
    f.template apply<0>();
  }
};


template<typename F, typename ... Ts>
struct mapper;

template<typename F, size_t I, typename ... Ts>
struct element_mapper
{
  using result_type = typename mapper<F,Ts...>::result_type;
  using elem_type = typename std::tuple_element<I,std::tuple<Ts...>>::type;

  static void map(F & f, const std::tuple<Ts...> & in, result_type & out)
  {
    std::get<I>(out) = f(std::get<I>(in));
    element_mapper<F,I-1,Ts...>::map(f, in, out);
  }
};

template<typename F, typename ... Ts>
struct element_mapper<F, 0, Ts...>
{
  using result_type = typename mapper<F,Ts...>::result_type;
  using elem_type = typename std::tuple_element<0,std::tuple<Ts...>>::type;

  static void map(F & f, const std::tuple<Ts...> & in, result_type & out)
  {
    std::get<0>(out) = f(std::get<0>(in));
  }
};

template<typename F, typename ... Ts>
struct mapper
{
  typedef std::tuple<typename std::result_of<F(Ts)>::type...> result_type;

  static result_type map(F f, const std::tuple<Ts...> & in)
  {
    result_type out;
    element_mapper<F, sizeof...(Ts)-1, Ts...>::map(f, in, out);
    return out;
  }
};

template <typename ...Iterators>
class multi_iterator
{
public:
  typedef std::tuple<Iterators...> iterator_tuple_type;
  typedef std::tuple<typename std::iterator_traits<Iterators>::reference ...> real_value_type;

  struct value_type
  {
    value_type(iterator_tuple_type & its): its(its) {}
    iterator_tuple_type & its;
    template <size_t I>
    decltype(*std::get<I>(its)) get()
    { return *std::get<I>(its); }
    template <size_t I>
    decltype(*std::get<I>(its)) get() const
    { return *std::get<I>(its); }
  };

  struct iterator_value
  {
    template <typename I>
    typename std::iterator_traits<I>::reference operator()(const I & i) const { return *i; }
  };

  struct iterator_increment
  {
    iterator_tuple_type & its;
    iterator_increment(iterator_tuple_type & its): its(its) {}
    template <size_t I>
    void apply() { ++std::get<I>(its); }
  };

  multi_iterator(const std::tuple<Iterators...> & its): its(its) {}

  value_type operator*()
  {
    return its;
  }

  multi_iterator<Iterators...> operator++()
  {
    iterator_increment inc(its);
    for_each<iterator_increment, sizeof...(Iterators)>::apply(inc);
    return *this;
  }

  struct any_equal
  {
    const iterator_tuple_type & lhs;
    const iterator_tuple_type & rhs;
    any_equal(const iterator_tuple_type & l, const iterator_tuple_type & r): lhs(l), rhs(r) {}
    bool value = false;
    template<size_t I>
    void apply() { value |= std::get<I>(lhs) == std::get<I>(rhs); }
  };

  bool operator==(const multi_iterator<Iterators...> & other) const
  {
    any_equal e(its, other.its);
    for_each<any_equal, sizeof...(Iterators)>::apply(e);
    return e.value;
  }

  bool operator!=(const multi_iterator<Iterators...> & other) const
  {
    return !(this->operator==(other));
  }

private:
  std::tuple<Iterators...> its;
};

template <typename ... Containers>
class multi_iterable
{
public:
  typedef multi_iterator<typename Containers::iterator...> iterator;
  typedef std::tuple<Containers&...> containers;

  multi_iterable(Containers& ... cs): cs(cs...) {}

  struct get_begin
  {
    template<typename C>
    typename C::iterator operator()(C & c) const { return c.begin(); }
  };

  struct get_end
  {
    template<typename C>
    typename C::iterator operator()(C & c) const { return c.end(); }
  };

  iterator begin()
  {
    return mapper<get_begin, Containers &...>::map(get_begin(), cs);
  }

  iterator end()
  {
    return mapper<get_end, Containers &...>::map(get_end(), cs);
  }

private:
  containers cs;
};

#if 0
template <typename ... Containers>
class multi_iterable<const Containers...>
{
public:
  typedef multi_iterator<typename Containers::const_iterator...> iterator;
  typedef std::tuple<const Containers&...> containers;

  multi_iterable(const Containers& ... cs): cs(cs...) {}

  struct get_begin
  {
    template<typename C>
    typename C::iterator operator()(C & c) const { return c.begin(); }
  };

  struct get_end
  {
    template<typename C>
    typename C::iterator operator()(C & c) const { return c.end(); }
  };

  iterator begin()
  {
    return mapper<get_begin, Containers &...>::map(get_begin(), cs);
  }

  iterator end()
  {
    return mapper<get_end, Containers &...>::map(get_end(), cs);
  }

private:
  containers cs;
};
#endif

template <typename ... Cs>
inline
multi_iterable<Cs...> merged(Cs & ... cs)
{
  return multi_iterable<Cs...>(cs...);
}

}
