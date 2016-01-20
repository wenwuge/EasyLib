#include "filemanager.h"
#include <pthread_t.h>

FileManager& FileManager::Instance(size_t inter_val)
{
    static FileManager instance(inter_val);

    return FileManager;
}

void FileManager::RegisterFileMonitor(FileMonitor& monitor)
{
    monitor_map_[monitor.monitor_name_] = monitor;
}

bool FileManager::Start()
{
    pthread_create();

}


