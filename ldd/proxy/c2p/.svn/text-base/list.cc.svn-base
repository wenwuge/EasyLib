#include <glog/logging.h>
#include <ldd/net/channel.h>

#include "ldd/proxy/proxy.h"
#include "ldd/proxy/c2p/list.h"
#include "ldd/proxy/p2s/list_ex.h"
#include "ldd/proxy/proxy_type.h"

using namespace ldd::net;

C2PList::C2PList(Proxy *proxy) 
    : proxy_(proxy)
      , hash_val_(0)
      , namespace_id_(0)
      , bucket_id_(0)
      , cur_index_(-1)
      , send_count_(0),
      code_(kOk){
}

C2PList::~C2PList(){
}

void C2PList::Init(const Payload& request, Action* next){
    //(1).处理请求
    LOG(INFO) << "C2PList::Init() recv list mesasge, len=" << request.body().size(); 
    if (!req_.ParseFrom(request.body().ptr(), request.body().size())) {
        LOG(ERROR) << "C2PList::Init() parse body fo get message error";
        return ;
    }
    else {
        LOG(INFO) << "C2PList::Init() Parse Ok" 
            << ", namespace=" << req_.name_space_.ToString()
            << ", key=" << req_.key_.ToString()
            << ", limit=" << req_.u32limit_
            << ", position=" << req_.u8position_;
    }

    namespace_id_ = proxy_->GetNameSpaceID(req_.name_space_.ToString());
    if (namespace_id_ < 0) {
        LOG(ERROR) << "C2PList::Init() get namespace(" 
            << req_.name_space_.ToString() << "id error";
        code_ = kNotValidNameSpace;
        *next = kDone;
        return ;
    }
    LOG(INFO) << "C2PList::Init() namespace id=" << namespace_id_;

    hash_val_ = proxy_->GetHashValue(req_.key_);
    bucket_id_ = proxy_->GetBucketID(hash_val_);
    if (bucket_id_ == -1) {
        LOG(ERROR) << "C2PList::Init() get bucket id error, key=" 
            << req_.key_.ToString();
        code_ = kNotValidBucketId;
        *next = kDone;
        return ;
    }

    SendRequest();
    *next = kWait;
    return ;
}

void C2PList::SendRequest(){
    std::vector<int> farm_list = *(proxy_->GetFarmList());
    if (farm_list.empty()) {
        LOG(FATAL) << "C2PList::SendRequest() farm list can not be null";
        return;
    }

    int farm_size = farm_list.size();
    for (int i = 0; i < farm_size; i++) {
        //p2s_list_info_.push_back(P2SListInfo());
        //boost::shared_ptr<P2SList> list(new P2SList(this ,i, 
        //proxy_->get_recv_timeout(), proxy_->get_done_timeout() ));
        //p2s_list_info_.back().p2s_list = list;
        //list->req_.u16Ns_ = namespace_id_;
        //list->req_.key_ = req_.key_;
        //list->req_.u32limit_ = req_.u32limit_;
        //list->req_.u8position_ = req_.u8position_;

        ////设置发送超时时间

        //boost::shared_ptr<Channel> channel = 
        //                    proxy_->GetReadChannel(hash_val_,farm_list[i]);
        //CHECK(channel.get() != NULL) << "get read channel failed";
        //channel->Post(list, ldd::util::TimeDiff::Milliseconds(200), true);
    }
}

void C2PList::Emit(Payload* response, Action* next){
    *next = kWait;
    *response = response_;
    LOG(INFO)<<"C2PList::Emit(), id="<<id()
        <<" response size"<<response_.body().size();
}

void C2PList::Done(Code* code){
    *code = 0;
    LOG(INFO)<<"C2PList::Done()";
}

void C2PList::Cancel(){
    LOG(INFO) << "C2PList::Cancel() HandleCancel called";
    int size = p2s_list_info_.size();
    for (int i = 0 ; i < size; i++) {
        if (p2s_list_info_[i].p2s_list != NULL) {
            p2s_list_info_[i].p2s_list->Cancel();
        }
    }
}

void C2PList::SendResult(ldd::protocol::GetOrListResponse *res, 
                         ldd::net::Payload result, int index){
    int i = 0;
    ldd::util::MutexLock lock(&mutex_);

    LOG(INFO) << "C2PList::SendResult() send result: p2s list index=" << index
                << ", key=" << res->key_.ToString()
                << ", value=" << res->value_.ToString()
                << ", version=" << res->s64Version_;

    p2s_list_info_[index].response.push_back(res);
    p2s_list_info_[index].response_buff.push_back(result);
    p2s_list_info_[index].has_new_result = true;

    LOG(INFO) << "C2PList::SendResult() cur_index="<< cur_index_;

    //执行归并排序的条件是，所有farm节点都至少返回一个结果
    bool has_new_result = true;
    if (cur_index_ != -1) {
        has_new_result = p2s_list_info_[cur_index_].has_new_result;
    }
    else {
        int count = p2s_list_info_.size();
        for (i = 0; i < count; i++) {
            if (p2s_list_info_[i].complete) {
                continue;
            }

            if (!p2s_list_info_[i].has_new_result) {
                has_new_result = false;
                break;
            }
        }
    }

    if (!has_new_result) {
        LOG(INFO) << "C2PList::SendResult() there is no resutl to send, and continue";
        return;
    }

    LOG(INFO) << "C2PList::SendResult() has new result, merge and send result to client";

    bool complete = false;

    //排序并向客户端发送结果
    while (has_new_result) {
        ldd::util::Slice min_key;
        if (cur_index_ != -1) {
            min_key = p2s_list_info_[cur_index_].response.front()->key_;
        }
        else {
            min_key = p2s_list_info_[0].response.front()->key_;
            cur_index_ = 0;
        }

        int count = p2s_list_info_.size();
        for (i = 0; i < count; i++) {
            if (i == cur_index_ || p2s_list_info_[i].complete) {
                continue;
            }

            if (p2s_list_info_[i].response.size() <= 0) {
                continue;
            }

            if (min_key.Compare(p2s_list_info_[i].response.front()->key_) > 0) {
                min_key = p2s_list_info_[i].response.front()->key_;
                cur_index_ = i;
            }
        }

        //找到最小key，将该key对应的结果返回给客户端
        //构造响应包
        ldd::protocol::GetOrListResponse *res 
                            = p2s_list_info_[cur_index_].response.front();
        //移除已经发送的结果
        p2s_list_info_[cur_index_].response.pop_front();
        p2s_list_info_[cur_index_].has_new_result = false;

        Buffer data(res->MaxSize());
        int len = res->SerializeTo(data.ptr());
        data.ReSize(len);
        delete res;

        // add by zhuyangbo
        response_.SetBody(data);
        Notify();  // call emit();
        complete = true;
        break;
        // end add 

        

        ////发送响应
        //bool ret = Respond(data);
        //if (!ret) {
        //    LOG(ERROR) << "send response to client failed";
        //    complete = true;
        //    break;
        //}

        send_count_++;

        if(send_count_ >= req_.u32limit_) {
            complete = true;
            break;
        }

        //退出本次循环，等待下次store的返回结果
        if (p2s_list_info_[cur_index_].response.size() <= 0) {
            break;
        }
    }  //end while (has_new_result)

    if (complete) {
        LOG(INFO) << "C2PList::SendResult() cancel all uncomplete request";
        //撤销未完成的请求
        int count = p2s_list_info_.size();
        for (int i = 0; i < count; i++) {
            if (!p2s_list_info_[i].complete) {
                if (p2s_list_info_[i].p2s_list != NULL) {
                    p2s_list_info_[i].p2s_list->Cancel();
                }
            } // if
        } // for

    }// if(complete)
}


void C2PList::Complete(){
    LOG(INFO) << "C2PList::Complete()all request is complete and release response";
    //释放资源
    for (uint32_t i = 0; i < p2s_list_info_.size(); i++) {
        std::list<ldd::protocol::GetOrListResponse *>::iterator it;
        for (it = p2s_list_info_[i].response.begin(); 
                it != p2s_list_info_[i].response.end() ; it++) {
            LOG(INFO) << "C2PList::Complete()delete one response";
            delete *it;
        }
    }

    LOG(INFO) << "C2PList::Complete() call finalize";
    //结束生存期
    //Finalize();
    Notify(true);
}

