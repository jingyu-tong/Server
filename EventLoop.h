//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include "base/Thread.h"
#include "base/CurrentThread.h"
#include "base/noncopyable.h"

#include <assert.h>

//封装Reactor 
class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    //main loop
    void loop();
    bool isInLoopThread() {
        return threadID_ == CurrentThread :: tid();
    }
    void assertInLoopThread() {
        assert(isInLoopThread());
    }
private:
    bool looping_;
    const pid_t threadID_;
};


#endif