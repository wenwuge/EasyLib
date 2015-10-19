#include "Thread.h"
#include "EventLoopThread.h"

int main(int argc, char** argv)
{
    //just one example, introduce the Thread's usage
    EventLoopThread event_thread;
    event_thread.Start(true);

    string input;
    while(input != "end"){
        getline(cin, input);
    }

    event_thread.Stop();
    while(input != "realend"){
        getline(cin, input);
    }
}

