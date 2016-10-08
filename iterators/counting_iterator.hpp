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

#include <cstddef>
#include <iterator>

namespace essentials {

template <typename T>
struct indexed
{
  size_t index;
  T value;
};

template <typename Iterator>
class counting_iterator
{
public:
  typedef indexed<typename std::iterator_traits<Iterator>::reference> value_type;

  counting_iterator(Iterator it, size_t c):
    m_it(it), m_index(c) {}

  counting_iterator & operator++()
  {
    ++m_it;
    ++m_index;
    return *this;
  }
  value_type operator*()
  {
    return value_type { m_index, *m_it };
  }
  bool operator==(const counting_iterator<Iterator> & other) const
  {
    return m_it == other.m_it;
  }
  bool operator!=(const counting_iterator<Iterator> & other) const
  {
    return m_it != other.m_it;
  }

private:
  Iterator m_it;
  size_t m_index;
};

template <typename Container>
class countable_c
{
public:
  typedef counting_iterator<typename Container::iterator> iterator;

  countable_c(Container & c): c(c) {}

  iterator begin()
  {
    return iterator(c.begin(), 0);
  }
  iterator end()
  {
    return iterator(c.end(), -1);
  }

private:
  Container & c;
};

template <typename Container>
class countable_c<const Container>
{
public:
  typedef counting_iterator<typename Container::const_iterator> const_iterator;

  countable_c(const Container & c): c(c) {}

  const_iterator begin() const
  {
    return const_iterator(c.begin(), 0);
  }

  const_iterator end() const
  {
    return const_iterator(c.end(), -1);
  }

private:
  const Container & c;
};

template <typename Container>
inline
countable_c<Container> countable(Container & c)
{
  return countable_c<Container>(c);
}

}
