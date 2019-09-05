//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() 
    :   loop_(nullptr),
        exiting_(false),
        thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
        mutex_(),
        cond_(mutex_)
    {
        
    }

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) //如果还没退出，发出quit，等待结束
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        // 一直等到threadFun在Thread里真正跑起来
        while (loop_ == nullptr)
            cond_.wait();
    }
    printf("thread id[%d] is started\n", thread_.tid());
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop; //创建成功，通知，可以返回loop_
        cond_.notify();
    }

    loop_->loop();
    loop_ = nullptr;
}