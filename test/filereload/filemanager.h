#ifndef __FILEMANAGER
#define __FILEMANAGER
#include <string>
using namespace std;

typedef boost::function<void(string&)>  UpdateCallback;
struct FileMonitor{
    FileMonitor(const string& monitor_name, const string& file_path,
            UpdateCallback callback);
    void Update();
    string monitor_name_;
    string file_path_;
    UpdateCallback callback_;
};

class FileManager{
public:

    void RegisterFileMonitor(FileMonitor& monitor);
    bool Start();
    FileManager& Instance(); 
private:
    static void CheckandUpdate();//use Instance() and do
private:
    FileManager(size_t inter_val):inter_val_(inter_val){}
    FileManager(const FileManager& copy){}
    typedef boost::shared_ptr<FileMonitor> FileMonitorPtr;
    map<string, FileMonitorPtr> monitor_map_;
    size_t inter_val_;
    pthread_t pid_;
};
#endif
