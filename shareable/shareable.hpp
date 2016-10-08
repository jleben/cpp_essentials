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

#include <mutex>

namespace essentials {

// An instance of exclusive<T> is constructed
// with a pointer to an instance of some type T,
// and a reference to a mutex.
// The mutex is kept locked as long as this
// instance of exclusive<T> exists
// (until it is destroyed).

// The handy overloads of the -> and * operators
// make it easy to access the data in T.
// For example:
// Declare some struct X:
//    struct X { int a; float b; };
// Create an X and a mutex.
//    X x;
//    mutex m;
// Create an exclusive<X>:
//    p = exclusive<X>(&x, m);
// Access the members of x:
//    int aa = p->a;
//    float bb = p->b;
// Access the entire X:
//    X & xx = *p;

template <typename T>
class exclusive
{
public:
  exclusive(T * d, std::mutex & m):
    _data(d),
    _guard(m)
  {}

  T * operator->() { return _data; }
  T & operator*() { return *_data; }

private:
  T * _data;
  std::unique_lock<std::mutex> _guard;
};

// An instance of shareable<T> provides
// safely shareable access to an instance
// of T.
// Access to the T is only provided
// via an instance of exclusive<T>,
// which ensures that only one party
// can access the T at once.

template <typename T>
class shareable
{
public:
  shareable(T * d): _data(d) {}
  exclusive<T> lock() { return exclusive<T>(_data, _mutex); }

private:
  T * _data;
  std::mutex _mutex;
};

}

