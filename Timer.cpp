//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Timer.h"
#include "Channel.h"

#include <sys/time.h>
#include <queue>
#include <utility>
#include <sys/timerfd.h>
#include <memory.h>

//创建timerfd
int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0)
  {
    //error
  }
  return timerfd;
}

//读取timerfd
void readTimerfd(int timerfd)
{
    uint64_t howmany;
    struct timeval now;
    gettimeofday(&now, NULL);
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  
    if (n != sizeof howmany)
    {
    //error
    }
}

void resetTimerfd(int timerfd, TimerManager::TimerPointer timer)
{
  // wake up loop by timerfd_settime()
    struct itimerspec new_value;
    memset(&new_value, 0, sizeof(new_value));

    struct timeval now;
    gettimeofday(&now, NULL);
    size_t now_ms = (((now.tv_sec) * 1000) + (now.tv_usec / 1000));

    int micro_seconds = timer->getExpTime() - now_ms;

    if(micro_seconds < 1000) 
        micro_seconds = 1000; //防止负数或者过小
    
    struct timespec ts;
    ts.tv_sec = micro_seconds / 1000;
    ts.tv_nsec = (micro_seconds % 1000) * 1000;
    new_value.it_value = ts;

    int ret = timerfd_settime(timerfd, 0, &new_value, nullptr);
    if (ret) {
        //right
    }
}

Timer::Timer(TimerCallback callBack, int timeout)
    :   timer_callback_(std::move(callBack)),
        delete_(false)
 {
    struct timeval now;
    gettimeofday(&now, NULL);
    // 以毫秒计
    expired_time_ = (((now.tv_sec) * 1000) + (now.tv_usec / 1000)) + timeout;
}

Timer::~Timer() {

}

void Timer::update(int timeout) {
    struct timeval now;
    gettimeofday(&now, NULL);
    // 以毫秒计
    expired_time_ = (((now.tv_sec) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool Timer::isValid() {
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t time_now = (((now.tv_sec) * 1000) + (now.tv_usec / 1000));
    
    if(time_now < expired_time_)
        return true;
    else {
        this->setDelete();
        return false;
    }
}


//TimerManager管理所有定时器，一个reactor只有一个
//定时器采用timerfd通知，reactor即使在epoll阻塞也能被唤醒
TimerManager::TimerManager(EventLoop* loop) 
    :   loop_(loop),
        timerfd_(createTimerfd()),
        timer_channel_(new Channel(loop_, timerfd_))
{
    timer_channel_->setReadCallback(std::bind(&TimerManager::handleExpiredEvent, this));
    timer_channel_->enableReading();
}

TimerManager::~TimerManager() {
    timer_channel_->disableAll();
}

void TimerManager::addTimer(TimerCallback callback, int timeout) {
    TimerPointer new_timer(new Timer(callback, timeout));
    timer_queue_.push(new_timer);
    resetTimerfd(timerfd_, new_timer);
}

//处理超时连接
//采用小顶堆对定时器进行管理
void TimerManager::handleExpiredEvent() {
    printf("timer handler\n");
    readTimerfd(timerfd_);
    while(!timer_queue_.empty()) {
        TimerPointer early_timer = timer_queue_.top();
       
        //被删除或到期
        if(early_timer->isDelete()) 
            timer_queue_.pop();
        else if(early_timer->isValid() == false) {
            timer_queue_.pop();
            early_timer->run();
        }   
        else
            break; //都没有，下次触发再处理
    }
}
