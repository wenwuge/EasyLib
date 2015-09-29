#include "MysqlPool.h"

    
MysqlPool::MysqlPool()
{
	pthread_mutex_init(&qlock,NULL);
	pthread_cond_init(&qcond,NULL);
}

MysqlPool::MysqlPool( const string& ip, int port, const string& user, const string& passwd, const string& dbname )
{
	pthread_mutex_init(&qlock,NULL);
	pthread_cond_init(&qcond,NULL);

	mIp = ip;
	mPort = port;
	mUser = user;
	mPasswd = passwd;
	mDbname = dbname;
}

MysqlPool::~MysqlPool()
{
	pthread_mutex_destroy(&qlock);
	pthread_cond_destroy(&qcond);
	while(!mQueue.empty())
    {
		MYSQL* p = mQueue.front();
		mQueue.pop();
		mysql_close(p);
	}
}

bool MysqlPool::init( u_int capacity )
{
	for(int i = 0; i < (int)capacity; i++)
    {
		MYSQL* mysql = mysql_init(NULL);
		if(mysql == NULL)
        {
			return false;
		}
		if(!mysql_real_connect(mysql,mIp.c_str(),mUser.c_str(),mPasswd.c_str(),mDbname.c_str(),mPort,NULL,0))
        {
			return false;
		}
		char value = 1;
		mysql_options(mysql,MYSQL_OPT_RECONNECT,(char*)&value);
		mQueue.push(mysql);
	}
	return true;
}

bool MysqlPool::init( u_int capacity, const string& ip, int port, const string& user, const string& passwd, const string& dbname )
{
	mIp = ip;
	mPort = port;
	mUser = user;
	mPasswd = passwd;
	mDbname = dbname;
	return init(capacity);
}

MYSQL* MysqlPool::get()
{
	MYSQL* ret = NULL;
	pthread_mutex_lock(&qlock);
	while(mQueue.empty())
		pthread_cond_wait(&qcond,&qlock);
	ret = mQueue.front();
	mQueue.pop();
	pthread_mutex_unlock(&qlock);
	return ret;
}

void MysqlPool::release( MYSQL* mysql )
{
	pthread_mutex_lock(&qlock);
	mQueue.push(mysql);
	pthread_mutex_unlock(&qlock);
	pthread_cond_signal(&qcond);
}

