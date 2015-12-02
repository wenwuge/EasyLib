// a.cc (2013-05-29)
// Li Xinjie (xjason.li@gmail.com)

#include "a.h"
#include <boost/ref.hpp>
#include <stdio.h>

struct A : public Msg {
  A() {}
  void Test() {
    printf("A::Test()\n");
  }
};

struct B : public Msg {
  B(int x) : x_(x) {}
  void Test() {
    printf("B::Test() %d\n", x_);
  }
  int x_;
};

struct C : public Msg {
  C(int x, int y) : x_(x), y_(y) {}
  void Test() {
    printf("C::Test() %d %d\n", x_, y_);
  }
  int x_;
  int y_;
};

int main() {
  int i = 5, j = 6, k = 7;
  Factory* fa = Factory::NewFactory<A>();
  Msg* ma = fa->New();

  Factory* fb = Factory::NewFactory<B>(boost::ref(i));
  Msg* mb = fb->New();

  Factory* fc = Factory::NewFactory<C>(j, k);
  Msg* mc = fc->New();

  ma->Test();
  mb->Test();
  mc->Test();
}
