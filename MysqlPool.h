#ifndef MYSQL_POOL_H_
#define MYSQL_POOL_H_

#include <mysql.h>
#include <pthread.h>

#include <queue>
#include <vector>
#include <iostream>

using namespace std;

class MysqlPool
{

public:

    MysqlPool();
    MysqlPool( const string& ip, int port, const string& user, const string& passwd, const string& dbname);

    ~MysqlPool();

    MYSQL* get();

    void release( MYSQL* mysql );

    bool init(u_int s);
    bool init(u_int s, const string& ip, int port, const string& user, const string& passwd, const string& dbname);

private:

    queue<MYSQL*> 		mQueue;
    pthread_mutex_t 	qlock;
    pthread_cond_t		qcond;

    string	mIp;
    int		mPort;
    string	mUser;
    string	mPasswd;
    string	mDbname;
};


#endif
