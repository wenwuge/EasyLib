#include "redis.h"
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
Redis::Redis():connect_(NULL)
{
}

Redis::~Redis()
{
    if (connect_)
        redisFree(connect_);

    connect_ = NULL;
}

bool Redis::Connect(string &host, int port, int timeout, string passwd, bool auth_enable)
{

	this->host = host;
	this->port = port;
	this->timeout = timeout;
	this->passwd = passwd;
	this->auth_enable = auth_enable;

    struct timeval timeout_tv = {0, 0};
    
    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    
    timeout_tv.tv_sec = timeout;

    for(int i = 0; i < MAX_TRY_COUNT; i++){
        connect_ = redisConnectWithTimeout(host.c_str(), port, timeout_tv); 

        if(!connect_ || connect_->err){
            continue;
        }

        //Maybe,the auth phase will be needed
        if(auth_enable){
            redisReply* reply = NULL;

            redisSetTimeout(connect_, timeout_tv);

            reply = (redisReply*)redisCommand(connect_, "AUTH %s", passwd.c_str());
            if (NULL == reply ||  REDIS_REPLY_ERROR == reply->type) {
                _logger.error("redis AUTH failed");
                if (NULL != reply) {
                    freeReplyObject(reply);
                }    
                redisFree(connect_);
                connect_ = NULL;
                continue;
            } else {
                _logger.notice(" redis auth success");
                freeReplyObject(reply);
            }    
        }
        return RESULT_OK;
        
    }

    
    _logger.error("connect redis error");
    return RESULT_ERROR;
}

bool Redis::ReConnect()
{

	if(NULL != connect_){
		redisFree(connect_);
		connect_ = NULL;
	}

    struct timeval timeout_tv = {0, 0};
    
    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    
    timeout_tv.tv_sec = timeout;

    for(int i = 0; i < MAX_TRY_COUNT; i++){
        connect_ = redisConnectWithTimeout(host.c_str(), port, timeout_tv); 

        if(!connect_ || connect_->err){
            continue;
        }

        //Maybe,the auth phase will be needed
        if(auth_enable){
            redisReply* reply = NULL;

            redisSetTimeout(connect_, timeout_tv);

            reply = (redisReply*)redisCommand(connect_, "AUTH %s", passwd.c_str());
            if (NULL == reply ||  REDIS_REPLY_ERROR == reply->type) {
		_logger.error("redis AUTH failed");
                if (NULL != reply) {
                    freeReplyObject(reply);
                }    
                redisFree(connect_);
                connect_ = NULL;
                continue;
            } else {
                _logger.notice(" redis auth success");
                freeReplyObject(reply);
            }    
        }   
        return RESULT_OK;
    
    }   

    
    _logger.error("connect redis error");
    return RESULT_ERROR;
}

bool Redis::Get(string& key, string& value)
{
    
    redisReply* reply = NULL;

    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    
    reply = (redisReply*)redisCommand(connect_, "Get %s", key.c_str());

    if (!reply){
        ReConnect();

        reply = (redisReply*)redisCommand(connect_, "Get %s", key.c_str());
        if (!reply){
            _logger.error("Get key %s from redis error", key.c_str());
            return RESULT_ERROR;
        }
    }
    
    if (!reply->str){
        _logger.error("Get key %s from redis error: Not exsit!", key.c_str());
        freeReplyObject(reply);
        return RESULT_ERROR;
    }
    value = reply->str; 
    _logger.info("Get key %s from redis OK", key.c_str());

    freeReplyObject(reply);
    return RESULT_OK;
}

bool Redis::Set(string &key, string& value, int expiretime)
{
    redisReply* reply = NULL;

    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    
    reply = (redisReply*)redisCommand(connect_, "Set %s %s", key.c_str(), value.c_str());

    if (!reply){
        ReConnect();
        reply = (redisReply*)redisCommand(connect_, "Set %s %s", key.c_str(), value.c_str());
        if (!reply){
            _logger.error("Set key %s %s into redis error", key.c_str(),value.c_str());
            return RESULT_ERROR;
        }
    }

    _logger.info("Set key %s %s into redis OK", key.c_str(), value.c_str());

    freeReplyObject(reply);

    if(expiretime > 0){
        Expire(key, expiretime);
    }
    
    return RESULT_OK;
}

bool Redis::Expire(string &key, int expiretime)
{
    redisReply* reply = NULL;

    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    
    reply = (redisReply*)redisCommand(connect_, "Expire %s %d", key.c_str(), expiretime);

    if (!reply){
        ReConnect();

        reply = (redisReply*)redisCommand(connect_, "Expire %s %d", key.c_str(), expiretime);
        if (!reply){
            _logger.error("Set key %s expire error", key.c_str());
            return RESULT_ERROR;
        }
    }

    _logger.info("Set key %s expire OK", key.c_str());

    freeReplyObject(reply);
    return RESULT_OK;
}

bool Redis::Lrange(string &key, vector<string>& result)
{
    redisReply* reply = NULL;

    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    
    reply = (redisReply*)redisCommand(connect_, "Lrange %s %d %d", key.c_str(), 0, -1);


    if (!reply){
	    ReConnect();
	    reply = (redisReply*)redisCommand(connect_, "Lrange %s %d %d", key.c_str(), 0, -1);	
	    if(!reply){
		    _logger.error("get key %s from redis error", key.c_str());
		    return RESULT_ERROR;
	    }
    }

    if (reply->elements){
        for (unsigned int i = 0 ; i < reply->elements; i ++){
           result.push_back(reply->element[i]->str);    
        }
    }else{
        _logger.error("Lrange key %s from redis error, elements : %d", key.c_str(), reply->elements);
        return RESULT_ERROR;
    }
     
    _logger.info("Lrange key %s from redis OK, elements : %d", key.c_str(), reply->elements);
    freeReplyObject(reply);

    return RESULT_OK;
}

bool Redis::Rpush(string &key, string& value, int expiretime)
{
    redisReply* reply = NULL;
    reply = (redisReply*)redisCommand(connect_, "Rpush %s %s", key.c_str(), value.c_str());

    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    if (reply){
        freeReplyObject(reply);
    }else{
        ReConnect();
        reply = (redisReply*)redisCommand(connect_, "Rpush %s %s", key.c_str(), value.c_str());
        if(!reply){ 
            _logger.error("Rpush key %s into redis error", key.c_str());
            return RESULT_ERROR;
        }
    }

    _logger.info("Rpush key %s into redis OK", key.c_str());
    if(expiretime > 0){
        Expire(key, expiretime);
    }
    return RESULT_OK; 
}

bool Redis::Delete(string &key)
{
    redisReply* reply = NULL;

    log4cpp::Category& _logger = log4cpp::Category::getInstance("MAIN_LOG");
    
    reply = (redisReply*)redisCommand(connect_, "del %s", key.c_str());

    if (!reply){
        ReConnect();

        reply = (redisReply*)redisCommand(connect_, "del %s", key.c_str());
        if (!reply){
            _logger.error("delete key %s from redis error", key.c_str());
            return RESULT_ERROR;
        }
    }

    _logger.info("delete key %s from redis OK", key.c_str());
    freeReplyObject(reply);
    return RESULT_OK;
}

void Redis::Close()
{
    if(connect_)
        redisFree(connect_);
    connect_ = NULL;

}
