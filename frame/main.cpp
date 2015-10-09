#include "skeleton.h"

int main(int argc , char ** argv)
{
    Skel skel;
    if(skel.Init(argc, argv) >=0 )
        skel.Start();
    return 0;
}

