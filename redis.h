#ifndef __REDIS_H
#define __REDIS_H
#include "hiredis/hiredis.h"
#include <string>
#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include <vector>

using namespace std;
#define MAX_TRY_COUNT 3
enum {
    RESULT_OK =0,
    RESULT_ERROR =1
};

class Redis{

    public:
        Redis();
        ~Redis();
        
        bool Connect(string& host, int port, int timeout,
                string passwd="", bool auth_enable=false);
        bool Get(string& key, string &value);
        bool Set(string& key, string& value, int expiretime = 0);
        bool Lrange(string& key, vector<string>& result);
        bool Rpush(string& key , string& value, int expiretime = 0);
        bool Delete(string& key);
        bool Expire(string& key, int expiretime);
        void Close();
		bool ReConnect();
         
    private:
        redisContext * connect_;    
	    string host;
	    int port;
	    string passwd;
	    int timeout;
	    bool auth_enable;
};

#endif
