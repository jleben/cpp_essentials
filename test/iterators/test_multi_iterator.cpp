#include "../../iterators/multi_iterator.hpp"

#include <vector>
#include <list>
#include <iostream>

using namespace essentials;
using namespace std;

int main()
{
  vector<int> v = { 101, 102, 103 };
  const list<int> l = { 201, 202, 203 };

  int end = l.size();

  for(const auto & e : merged(v,l))
  {
    if (!end--)
    {
      cerr << "Failure: did not stop at end." << endl;
      break;
    }

    cout << "v: " << e.get<0>()
         << " l: " << e.get<1>()
         << endl;
  }

}
