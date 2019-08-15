//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"

#include <sys/epoll.h>
#include <map>

Epoll::Epoll(Eventloop* loop)
    :   ownerloop_(loop),
        epollfd_(epoll_create1(EPOLL_CLOEXEC))
{

}
Epoll::~Epoll() 
{

}

void Epoll::epollAdd(ChannelPtr request) {
    int fd = request->getFd();
    
    //设置epoll_event相应信息
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        //fail
    } else {
        fd_to_channel_[fd] = request; //构建映射表
    }
}

//更改等待的事件，和添加类似
void Epoll::epollMod(ChannelPtr request) {
        int fd = request->getFd();
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();

        if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0)
        {
            //fail
        }
}

//删除事件，同时更新映射关系
void Epoll::epollDel(ChannelPtr request) {
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    if(epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event) < 0)
    {
       //fail
    }
    fd_to_channel_.erase(fd);
}

//poll, 返回活跃的channel
std::vector<Channel*> Epoll::poll(int timeoutMs) {
    while (true)
    {
        int num_events = epoll_wait(epollfd_, &*events_.begin(), events_.size(), timeoutMs);
        if (num_events < 0)
            ; //fail
        
        std::vector<ChannelPtr> active_channels = getActiveChannels(num_events); //返回更新，并返回活动channel
        return active_channels;
    }
}

std::vector<Channel*> Epoll::getActiveChannels(int num_events) {
    std::vector<ChannelPtr> active_channels;

    //num_events个事件到达
    for(int i = 0; i < num_events; ++i) {
        
        // 获取有事件产生的描述符
        int fd = events_[i].data.fd;

        ChannelPtr cur_channel = fd_to_channel_[fd];
        if(cur_channel) {
            cur_channel->setRevents(events_[i].events);
            cur_channel->setEvents(0);
            active_channels.push_back(cur_channel);
        }
        return active_channels;
    }
}
