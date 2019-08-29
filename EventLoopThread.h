//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef EVENTLOOPTHREAD_H_
#define EVENTLOOPTHREAD_H_

#include "base/Condition.h"
#include "base/Thread.h"
#include "base/noncopyable.h"
#include "base/MutexLock.h"
#include "EventLoop.h"

class EventLoopThread : noncopyable {
    public:
        EventLoopThread();
        ~EventLoopThread();

        EventLoop* startLoop(); //开始线程

    private:
        void threadFunc(); //线程循环
        EventLoop* loop_;
        bool exiting_; //退出标志
        Thread thread_; 
        MutexLock mutex_;
        Condition cond_;
};









#endif
