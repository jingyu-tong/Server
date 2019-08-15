//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef EPOLL_H_
#define EPOLL_H_

#include "Channel.h"
#include "base/noncopyable.h"
#include "EventLoop.h"

#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>

class Epoll : noncopyable
{
    public:
        typedef std::vector<epoll_event> EventList;
        typedef std::map<int, Channel*> ChannelMap;
        typedef Channel* ChannelPtr;
        
        Epoll(Eventloop* loop);
        ~Epoll();

        void epollAdd(ChannelPtr request); //注册新描述符
        void epollMod(ChannelPtr request); //修改描述符
        void epollDel(ChannelPtr request); //删除描述符

        std::vector<ChannelPtr>  poll(int timeoutMs); //poll
        std::vector<ChannelPtr> getActiveChannels(int num_events); //返回活动channel的列表，内部使用


    private:
        Eventloop* ownerloop_; //归属loop
        EventList events_; //关心事件
        ChannelMap fd_to_channel_; //fd转换为Channel
        int epollfd_;
};






#endif