//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Timer.h"

#include <sys/time.h>
#include <queue>

Timer::Timer(int timeout)
    :   delete_(false)
 {
    struct timeval now;
    gettimeofday(&now, NULL);
    // 以毫秒计
    expired_time_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

Timer::~Timer() {

}

void Timer::update(int timeout) {
    struct timeval now;
    gettimeofday(&now, NULL);
    // 以毫秒计
    expired_time_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool Timer::isValid() {
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t time_now = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    
    if(time_now < expired_time_)
        return true;
    else {
        this->setDelete();
        return false;
    }
}


TimerManager::TimerManager() {

}

TimerManager::~TimerManager() {

}

void TimerManager::addTimer(int timeout)
{
    TimerPointer new_timer(new Timer(timeout));
    timer_queue_.push(new_timer);
}

//处理超时连接
void TimerManager::handleExpiredEvent() {
    while(!timer_queue_.empty()) {
        TimerPointer early_timer = timer_queue_.top();
       
        //被删除或到期
        if(early_timer->isDelete()) 
            timer_queue_.pop();
        else if(early_timer->isValid() == false) 
            timer_queue_.pop();
        else
            break; //都没有，下次再循环到再处理
    }
}
