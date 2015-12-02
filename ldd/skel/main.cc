// main.cc (2013-07-23)
// Li Xinjie (xjason.li@gmail.com)

#include "internal/skel.h"

int main(int argc, char** argv) {
    ldd::skel::Skel skel;
    if (skel.Init(argc, argv)) {
        skel.Run();
    }
}

