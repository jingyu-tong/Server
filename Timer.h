//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef TIMER_H_
#define TIMER_H_

#include "base/noncopyable.h"
#include "Channel.h"

#include <memory>
#include <queue>
#include <deque>
#include <map>
#include <functional>

class EventLoop;
class Channel;

//定时器
class Timer {
    public:
        typedef std::function<void()> TimerCallback;

        Timer(TimerCallback callBack, int timeout);
        ~Timer();
        
        void update(int timeout);
        bool isValid();
        size_t getExpTime() const { return expired_time_; } 
        void run() {timer_callback_(); }
        void cancel() {deleted_ = true; }
        bool isDeleted() {return deleted_; }
    private:
        size_t expired_time_; //到期时间
        TimerCallback timer_callback_; //到期回调，被删除不执行
        bool deleted_;
};

//管理Timer
class TimerManager {
    public:
        typedef std::function<void()> TimerCallback;
        typedef std::shared_ptr<Timer> TimerPointer;
        typedef std::multimap<size_t, TimerPointer> TimerMap; 
        
        TimerManager(EventLoop* loop);
        ~TimerManager();
        TimerPointer addTimer(TimerCallback callback, int timeout); //添加timer
        void updateTimer(TimerPointer& timer, int timeout);
        void updateTimerInLoop(TimerPointer& timer, int timeout);
        void handleExpiredEvent(); //处理到期事件
        void addTimerInLoop(TimerPointer& timer); //把修改列表转移到IO线程
        void deleteTimer(TimerPointer& timer); //删除timer
        void deleteTimerInLoop(TimerPointer& timer);
        int getExpiredTime();
    private:
        EventLoop* loop_;
        TimerMap timers_;
        int timerfd_; //timerfd
        std::unique_ptr<Channel> timer_channel_; //根据timerfd构建channel
        bool timer_calling_;
};



#endif