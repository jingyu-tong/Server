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
        timer_channel_(new Channel(loop_, timerfd_)),
        timer_calling_(false)
{
    timer_channel_->setReadCallback(std::bind(&TimerManager::handleExpiredEvent, this));
    timer_channel_->enableReading();
}

TimerManager::~TimerManager() {
    timer_channel_->disableAll();
}

//将更改列表转移到IO线程完成
TimerManager::TimerPointer TimerManager::addTimer(TimerCallback callback, int timeout) {
    TimerPointer new_timer(new Timer(callback, timeout));
    loop_->runInLoop(std::bind(&TimerManager::addTimerInLoop, this, new_timer));
    return new_timer;
}

//添加timer的用户回调
void TimerManager::addTimerInLoop(TimerPointer timer) {
    timer_queue_.push(timer);
    resetTimerfd(timerfd_, timer);
}

//更新timer
void TimerManager::updateTimer(TimerPointer timer, int timeout) {
    loop_->runInLoop(std::bind(&TimerManager::updateTimerInLoop, this, timer, timeout));
}

//更新某一个timer的时间
void TimerManager::updateTimerInLoop(TimerPointer timer, int timeout) {
    if(timer.use_count() != 0){
        
        if(timer_calling_ == false) {
            timer->update(timeout);
            //if(add_timers_.count(timer) == 0)
                timer_queue_.push(timer);
            resetTimerfd(timerfd_, timer);
        } else {
            add_timers_[timer] = timeout;
        }
    }
}

//处理超时连接
//采用小顶堆对定时器进行管理
void TimerManager::handleExpiredEvent() {
    // printf("timer handler\n");
    readTimerfd(timerfd_); //避免再次触发
    std::vector<TimerPointer> on_timers;
    while(!timer_queue_.empty()) {
        TimerPointer early_timer = timer_queue_.top();
       
        //被删除或到期
        if(early_timer->isDelete()) 
            timer_queue_.pop();
        else if(early_timer->isValid() == false) {
            timer_queue_.pop();
            on_timers.push_back(early_timer);
        }   
        else
            break; //都没有，下次触发再处理
    }
    timer_calling_ = true;
    add_timers_.clear();
    for(const auto& on_timer : on_timers) {
        on_timer->run();
    }
    timer_calling_ = false;

    //更新到时的timer,要重新加入小顶堆
    for(const auto& on_timer : on_timers) {
        if(add_timers_.count(on_timer)) {
            on_timer->update(add_timers_[on_timer]);
            timer_queue_.push(on_timer);
            resetTimerfd(timerfd_, on_timer);
            add_timers_.erase(on_timer);
        }
    }

    //有可能没到时，但是也被更新，下次自动没清楚，不用释放
    for(const auto& timer : add_timers_) {
            timer.first->update(timer.second);
    }
    // printf("handler out\n");
}
