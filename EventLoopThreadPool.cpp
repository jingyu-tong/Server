//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop, int num_threads) 
    :   base_loop_(base_loop),
        started_(false),
        num_threads_(num_threads),
        next_(0)
{
    if(num_threads_ < 0) {
        //LOG << "num threads < 0";
        abort();
    }
}

void EventLoopThreadPool::start() {
    base_loop_->assertInLoopThread();
    started_ = true;

    for(int i = 0; i < num_threads_; ++i) {
        std::shared_ptr<EventLoopThread> new_event_thread(new EventLoopThread());
        threads_.push_back(new_event_thread);
        loops_.push_back(new_event_thread->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    base_loop_->assertInLoopThread();
    assert(started_);

    EventLoop *loop = base_loop_;
    if(loops_.empty()) { //线程池个数为0
        return loop;
    }
    loop = loops_[next_];
    next_ = (next_ + 1) % num_threads_;
    return loop;
}