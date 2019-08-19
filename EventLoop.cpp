//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "Epoll.h"

#include <poll.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

const int kPollTimeMs = 10000;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop ::EventLoop() 
:   looping_(false),
    threadID_(CurrentThread::tid()), 
    poller_(new Epoll(this))
    {
        //只允许一个EventLoop
        if(t_loopInThisThread) {

        } else {
            t_loopInThisThread = this;
        }
    }

EventLoop :: ~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}

void EventLoop :: loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    
    while(!quit_) {
        active_channels_.clear();
        active_channels_ = poller_->poll(kPollTimeMs);

        //分发给handler
        for(auto it = active_channels_.begin(); it != active_channels_.end(); ++it ) {
            (*it)->handleEvent();
        }
    }
    looping_ = false;
}
//更新channel
void EventLoop::updateChannel(Channel* channel) {
    assertInLoopThread(); //只允许IO线程更改channel
    poller_->updateChannel(channel);
}