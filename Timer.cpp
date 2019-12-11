//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Timer.h"
#include "Channel.h"
#include "Logging.h"

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
        deleted_(false)
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
    if(deleted_) return false;
    
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t time_now = (((now.tv_sec) * 1000) + (now.tv_usec / 1000));
    
    if(time_now < expired_time_)
        return true;
    else {
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
    //timer_channel_->setReadCallback(std::bind(&TimerManager::handleExpiredEvent, this));
    timer_channel_->setReadCallback([this]() {this->handleExpiredEvent(); });
    timer_channel_->enableReading();
}

TimerManager::~TimerManager() {
    timer_channel_->disableAll();
}

//将更改列表转移到IO线程完成
TimerManager::TimerPointer TimerManager::addTimer(TimerCallback callback, int timeout) {
    TimerPointer new_timer(new Timer(std::move(callback), timeout));
    //loop_->runInLoop(std::bind(&TimerManager::addTimerInLoop, this, new_timer));
    loop_->runInLoop([this, &new_timer]() {this->addTimerInLoop(new_timer); });
    return new_timer;
}

//添加timer的用户回调
void TimerManager::addTimerInLoop(TimerPointer& timer) {
    auto iter = timers_.insert({timer->getExpTime(), timer});
    //timer->setPos(iter);
    resetTimerfd(timerfd_, timer);
}

//更新timer
void TimerManager::updateTimer(TimerPointer& timer, int timeout) {
    //loop_->runInLoop(std::bind(&TimerManager::updateTimerInLoop, this, timer, timeout));
    loop_->runInLoop([this, &timer, timeout]() {this->updateTimerInLoop(timer, timeout); });
}

//更新某一个timer的时间
void TimerManager::updateTimerInLoop(TimerPointer& timer, int timeout) {
    auto iter = timers_.find(timer->getExpTime());
    while(iter != timers_.end() && iter->first == timer->getExpTime()) {
        if(iter->second == timer) {
            timers_.erase(iter++);
            timer->update(timeout);
            timers_.insert({timer->getExpTime(), timer});
            resetTimerfd(timerfd_, timer);
            break;
        }
        ++iter;  
    }
}

//删除timer
void TimerManager::deleteTimer(TimerPointer& timer) {
    //loop_->runInLoop(std::bind(&TimerManager::deleteTimerInLoop, this, timer));
    loop_->runInLoop([this, &timer]() {this->deleteTimerInLoop(timer); });
}
void TimerManager::deleteTimerInLoop(TimerPointer& timer) {
    //LOG << "erase timer in timers " << timer->getExpTime(); 
    // auto iter = timers_.find(timer->getExpTime());
    // while(iter != timers_.end() && iter->first == timer->getExpTime()) {
    //     if(iter->second == timer) {
    //         timers_.erase(iter++);
    //         break;
    //     }
    //     ++iter;  
    // }
    //timers_.erase(timer->getPos());
    timer->cancel();
}
//处理超时连接
void TimerManager::handleExpiredEvent() {
    // LOG << "Timer is expired";
    readTimerfd(timerfd_); //避免再次触发
    std::vector<std::pair<size_t, TimerPointer>> on_timers;
    
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t now_ms = (((now.tv_sec) * 1000) + (now.tv_usec / 1000));

    TimerMap::iterator end = timers_.lower_bound(now_ms);
    // if(end == timers_.end()) 
    //     LOG << "Timer has already been deleted";
    std::copy(timers_.begin(), end, back_inserter(on_timers));
    timers_.erase(timers_.begin(), end);

    // for(auto iter = timers_.begin(); iter != timers_.end(); ) { 
    //     TimerPointer early_timer = iter->second;
       
    //     //到期
    //     if(early_timer->isValid() == false) {
    //         timers_.erase(iter++); //迭代器，注意失效
    //         on_timers.push_back(early_timer);
    //     }   
    //     else
    //         break; //都没有，下次触发再处理
    // }

    for(const auto& on_timer : on_timers) {
        if(on_timer.second->isValid())
            on_timer.second->run();
    }
}
