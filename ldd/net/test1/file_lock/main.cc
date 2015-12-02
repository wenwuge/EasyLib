#include <boost/interprocess/sync/file_lock.hpp>

int main()
{
    //This throws if the file does not exist or it can't
    //open it with read-write access!
    boost::interprocess::file_lock flock("my_file");

    return 0;
}
