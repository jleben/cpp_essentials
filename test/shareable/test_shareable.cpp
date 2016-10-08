#include "../../shareable/shareable.hpp"

#include <thread>
#include <chrono>

using namespace essentials;
using namespace std;

struct X { int a; float b; };

int f(shareable<X> * p)
{
  auto d = p->lock();
  printf("thread: a=%d, b=%f\n", d->a, d->b);
}

int main()
{
  X x;
  x.a = 1;
  x.b = 2.3;

  shareable<X> p(&x);

  thread t;

  {
    printf("main: locking\n");

    auto d = p.lock();

    printf("main: creating thread\n");

    t = thread(f, &p);

    printf("main: sleeping...\n");

    this_thread::sleep_for(chrono::seconds(1));

    printf("main: a=%d, b=%f\n", d->a, d->b);

    d->a = -1;
    d->b = -2.3;

    printf("main: unlocking\n");
  }

  t.join();
}
