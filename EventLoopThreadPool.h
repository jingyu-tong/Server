//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef EVENTLOOPTHREADPOOL_H_
#define EVENTLOOPTHREADPOOL_H_

#include "base/noncopyable.h"
#include "EventLoopThread.h"
#include "base/Thread.h"

#include <vector>
#include <memory>

//线程池
//含有一个basethread和若干个子线程
class EventLoopThreadPool : noncopyable {
    public:
        EventLoopThreadPool(EventLoop* base_loop, int num_threads);
        ~EventLoopThreadPool() {};

        void start();
        EventLoop* getNextLoop();




    private:
        EventLoop* base_loop_;
        bool started_;
        int num_threads_;
        int next_;
        std::vector<std::shared_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop*> loops_;
};













#endif