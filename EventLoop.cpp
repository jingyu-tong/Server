//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "Epoll.h"
#include "Timer.h"
#include "Logging.h"

#include <poll.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/eventfd.h>

const int kPollTimeMs = 10000;

__thread EventLoop* t_loopInThisThread = 0;

//创建eventfd，返回对应fd
int createEventfd()
{
  int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    //LOG << "Failed in creating eventfd";
  }
  return evtfd;
}

EventLoop ::EventLoop() 
:   looping_(false),
    threadID_(CurrentThread::tid()), 
    poller_(new Epoll(this)),
    timer_queue_(new TimerManager(this)),
    calling_pending_(false),
    wakeup_fd_(createEventfd()),
    wakeup_channel_(new Channel(this, wakeup_fd_))
    {
        //只允许一个EventLoop
        if(t_loopInThisThread) {

        } else {
            t_loopInThisThread = this;
        }
        wakeup_channel_->setReadCallback(std::bind(&EventLoop::handleEventfd, this));
        wakeup_channel_->enableReading();
    }

EventLoop :: ~EventLoop() {
    assert(!looping_);
    //处理eventfd和channel
    wakeup_channel_->disableAll();
    close(wakeup_fd_);
    t_loopInThisThread = NULL;
}

void EventLoop :: loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    
    while(!quit_) {
        active_channels_.clear();
        active_channels_ = poller_->poll(kPollTimeMs);

        //分发给handler
        for(auto it = active_channels_.begin(); it != active_channels_.end(); ++it ) {
            (*it)->handleEvent();
        }
        doPendingFunctors();
    }
    looping_ = false;
}

//退出
//在IO线程，则不用唤醒，下次loop循环将自动退出
//不在，则需要唤醒
void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}

//更新channel
void EventLoop::updateChannel(Channel* channel) {
    assertInLoopThread(); //只允许IO线程更改channel
    poller_->updateChannel(channel);
}

//提供定时器封装
//过一段时间运行callback
//将新生产的Timer指针返回，用于更新timer
TimerManager::TimerPointer EventLoop::runAfter(Timer::TimerCallback callback, int timeout) {
    TimerManager::TimerPointer new_timer = timer_queue_->addTimer(std::move(callback), timeout);
    return new_timer;
}

//更新runafter的时间，用来推迟或延后调用
void EventLoop::updateTimer(std::shared_ptr<Timer>& timer, int timeout) {
    timer_queue_->updateTimer(timer, timeout);
}

//跨线程调用
//若在当前线程调用，直接进行回调
//否则，会唤醒IO线程，进行回调
void EventLoop::runInLoop(Functor callback) {
    if (isInLoopThread()) {
        callback();
    }
    else {
        queueInLoop(std::move(callback));
    }
}

//将回调放入vector中，并在必要时唤醒
void EventLoop::queueInLoop(Functor callback) {
    {
        MutexLockGuard lock(mutex_);
        pending_functors_.push_back(std::move(callback));
    }

    //唤醒情况：
    //1. 不是IO线程
    //2. 如果在调用用户回调了，那么必须唤醒，否则不能及时被调用
    //因为线程重新阻塞epoll中，如果不唤醒，将继续阻塞，不会调用用户回调
    if(!isInLoopThread() || calling_pending_) {
        wakeup();
    }
}

//执行vector中的各个回调
//将回调列表移入临时对象，然后再依次调用，这样可以:
//1. 减小临界区长度
//2. 避免死锁(用户回调可能再次调用queueInLoop)
void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    calling_pending_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pending_functors_);
    }
    for(const Functor& functor : functors) {
        functor();
    }
    calling_pending_ = false;
}

//处理eventfd唤醒
void EventLoop::handleEventfd() {
    uint64_t one = 1;
    ssize_t n = read(wakeup_fd_, &one, sizeof one);
    if(n != sizeof one) {
        //LOG << "EventLoop::handleEventfd() reads" << n << "bytes";
    }
}
//唤醒线程
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof one);
    if(n != sizeof one) {
        //LOG << "EventLoop::wakeup() writes" << n << "bytes";
    }
}
