#include "Config.h"
#include <stdlib.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/stat.h>

using boost::format;

void* configRechecker(void* params)
{
	Config* config=(Config*)(params);
	config->rechecking();
	return NULL;
}


Config::Config()
{
//	-1 means no config file have been successfully loaded
	mIdx=-1;
	mKeepCheck=false;
	mRecheckThread=0;
}

Config::~Config()
{
	stop();
	join();
}

bool Config::init(const char *confFile)
{
	int newIdx=(mIdx==0)?1:0;

	mConfFn=string(confFile);
	ifstream infile(mConfFn.c_str());
	if(!infile)
	{
		return false;
	}

	mConfMap[newIdx].clear();

	string key,value,str_in;
	while(getline(infile,str_in))
	{
		if(str_in.length()==0)
        {
			continue;
		}
		str_in.erase(0, str_in.find_first_not_of(" \t\n\r"));
		str_in.erase(str_in.find_last_not_of(" \t\n\r") + 1);

		if(str_in.length()==0 || str_in[0]=='#')
        {
			continue;
		}

		int pos=str_in.find('=');
		if(pos==-1 || pos==0 || pos==(int)(str_in.length()-1))
        {
			continue;
		}

		key=str_in.substr(0,pos);
		value=str_in.substr(pos+1);
		key.erase(0, key.find_first_not_of(" \t\n\r"));
		key.erase(key.find_last_not_of(" \t\n\r") + 1);
		value.erase(0, value.find_first_not_of(" \t\n\r"));
		value.erase(value.find_last_not_of(" \t\n\r") + 1);

		mConfMap[newIdx][key]=value;
	}

	infile.close();

	if(confCheck(newIdx))
    {
		mIdx=newIdx;

		struct stat confFileStat;
		stat(mConfFn.c_str(),&confFileStat);
		mModifiedTime=confFileStat.st_mtime;

		return true;
	}
    else
    {
		return false;
	}
}

void Config::rechecking()
{
    struct stat confFileStat;

    while(mKeepCheck)
    {
        int sleepSeconds = 0;
        while (true) 
        {
            if (!mKeepCheck) 
            {
                return;
            }
            int recheckTime = getConfInt(mConfRecheckKey.c_str());
            if (recheckTime <= 0) 
            {
                recheckTime = 60;
            }
            if (sleepSeconds >= recheckTime) 
            {
                break;
            }
            else 
            {
                sleep(1);
                sleepSeconds++;
            }
        }
        stat(mConfFn.c_str(),&confFileStat);
        if(confFileStat.st_mtime > mModifiedTime)
        {
            init(mConfFn.c_str());
        }
    }
}

bool Config::startRecheck(const char* confRecheckTimeKey)
{

	if(confRecheckTimeKey == NULL || confRecheckTimeKey[0] == '\0')
    {
		return false;
	}

	if(mRecheckThread!=0)
    {
		return false;
	}

	mConfRecheckKey = confRecheckTimeKey;
	mKeepCheck=true;

	int res = pthread_create(&mRecheckThread, NULL, configRechecker, this);
	if (res != 0)
    {
		return false;
	}

	return true;
}

void Config::stop()
{
	mKeepCheck=false;
}

void Config::join()
{
	if(mRecheckThread!=0)
    {
		pthread_join(mRecheckThread, NULL);
	}
	mRecheckThread=0;
}

const char* Config::getConfStr(const char* key) const
{
	if(mIdx==-1)
    {
		return NULL;
	}
	map<string,string>::const_iterator ptr=mConfMap[mIdx].find(key);
	if(ptr == mConfMap[mIdx].end())
    {
		return NULL;
	}
    else
    {
		return ptr->second.c_str();
	}
}

int64_t Config::getConfInt(const char* key) const
{
	if(mIdx==-1)
    {
		return -1;
	}
	map<string,string>::const_iterator ptr=mConfMap[mIdx].find(key);
	if(ptr==mConfMap[mIdx].end())
    {
		return -1;
	}else
    {
		return atol(ptr->second.c_str());
	}
}

bool Config::confCheck(int newIdx)
{
	return true;
}

bool Config::checkIsKeyExist(int newIdx,const char* key)
{
	map<string,string>::const_iterator ptr=mConfMap[newIdx].find(key);
	if(ptr==mConfMap[newIdx].end())
    {
		return false;
	}else
    {
		return true;
	}
}

bool Config::checkIsInRange(int newIdx,const char* key,int64_t max,int64_t min)
{
	map<string,string>::const_iterator ptr=mConfMap[newIdx].find(key);
	if(ptr==mConfMap[newIdx].end())
    {
		return false;
	}
	int64_t value = atol(ptr->second.c_str());
	if(value<=max && value>=min)
    {
		return true;
	}
    else
    {
		return false;
	}
}

void Config::setConfItem(const char*key, const char* value)
{
    mConfMap[mIdx][key] = value;
}

void Config::setConfItem(const char*key, int64_t value)
{
    ostringstream oss;
    oss << value;
    mConfMap[mIdx][key] = oss.str();
}

