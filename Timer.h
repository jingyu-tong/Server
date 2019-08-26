//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef TIMER_H_
#define TIMER_H_

#include "base/noncopyable.h"

#include <memory>
#include <queue>
#include <deque>

//定时器
class Timer {
    public:
        Timer(int timeout);
        ~Timer();
        
        void update(int timeout);
        bool isValid();
        void setDelete() { delete_ = false; }
        bool isDelete() { return delete_; }
        size_t getExpTime() const { return expired_time_; } 

    private:
        bool delete_; //待删除标志
        size_t expired_time_; //到期时间
};

//用于Timer到期之间的比较
struct TimerCmp {
        typedef std::shared_ptr<Timer> TimerPointer;
        bool operator()(TimerPointer a, TimerPointer b) {
            return a->getExpTime() > b->getExpTime();
        }
};

//管理Timer
class TimerManager {
    public:
        TimerManager();
        ~TimerManager();
        void addTimer(int timeout); //添加timer
        void handleExpiredEvent(); //处理到期事件

    private:
         typedef std::shared_ptr<Timer> TimerPointer;
         std::priority_queue<TimerPointer,  std::deque<TimerPointer>, TimerCmp> timer_queue_;

};



#endif