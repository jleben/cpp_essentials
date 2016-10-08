#include "../../iterators/counting_iterator.hpp"

#include <vector>
#include <list>
#include <iostream>

using namespace essentials;
using namespace std;


int main()
{
  vector<int> v = { 101, 102, 103 };
  list<int> l = { 201, 202, 203 };

  {
    cout << "mutable l:" << endl;

    int end = l.size();

    for(const auto & e : countable(l))
    {
      if (!end--)
      {
        cerr << "Failure: did not stop at end." << endl;
        break;
      }

      cout << "l: " << e.value
           << " v: " << v[e.index]
           << endl;
    }
  }

  const auto & cl = l;

  {
    cout << "const l:" << endl;

    int limit = l.size();

    for(const auto & e : countable(cl))
    {
      if (!limit--)
      {
        cerr << "Failure: did not stop at limit." << endl;
        break;
      }

      cout << "l: " << e.value
           << " v: " << v[e.index]
           << endl;
    }
  }
}
