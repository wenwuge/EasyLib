
#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/format.hpp>

using boost::format;

using namespace std;

void* configRechecker(void* params);


struct ConfigInt
{
	char key[64];
	int64_t max;
	int64_t min;
};

/// note : this class is not multi-threads safe
class Config
{

public:
	Config();
	virtual ~Config();

	bool init(const char *confFile);

	///start a thread to check config file changes each timeDeday
	bool startRecheck(const char* confRecheckTimeKey);

	void stop();

	void join();

	/// notes:no copy,you can't hold this pointer for long time
    //	if key not exit or map doesn't be initalized, return null
	const char* getConfStr(const char* key) const;

    //	if key not exit or map doesn't be initalized, return -1
	int64_t getConfInt(const char* key) const;

	bool checkIsKeyExist(int newIdx,const char* key);

	bool checkIsInRange(int newIdx,const char* key,int64_t max,int64_t min);

	void rechecking();

protected:
	/// each module extend this interface to implement config's check
	virtual bool confCheck(int newIdx);

private:
    // just for the convenience of unittest, never try to use these
    //   two methods for any other purpose
    void setConfItem(const char*key, const char*value);
    void setConfItem(const char*key, int64_t value);

private:
	string      mConfFn;
	int         mIdx;
	pthread_t   mRecheckThread;

	string      mConfRecheckKey;
	bool        mKeepCheck;
	time_t      mModifiedTime;

	map<string,string> mConfMap[2];
};

#endif
