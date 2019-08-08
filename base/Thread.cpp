//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Thread.h"
#include "CurrentThread.h"


#include <stdint.h>
#include <assert.h>
#include <sys/prctl.h>


//封装thread数据成员，传给线程
struct ThreadData {
    typedef Thread :: ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid, CountDownLatch* latch)
    :   func_(func),
        name_(name),
        tid_(tid),
        latch_(latch)
        {

        }

    void runInThread() {
        *tid_ = CurrentThread::tid();
        tid_ = NULL; //给当前线程储存tid，然后将指针设空
        
        latch_->countdown();
        latch_ = NULL; //进入子线程

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread :: t_threadName); //给线程命名
        
        func_(); 
        CurrentThread :: t_threadName = "finished";
    }
};

//pthread线程入口
//通过传递进来的ThreadData调用run，进行func的调用
void* startThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

Thread ::  Thread(const ThreadFunc& func, const std::string& name)
    :   started_(false),
        joined_(false),
        pthreadID_(0),
        tid_(0),
        func_(func),
        name_(name),
        latch_(1)
{
    setDefaultName();
}

Thread :: ~Thread() {
    if(started_ && !joined_)
        pthread_detach(pthreadID_);
}

void Thread :: setDefaultName() {
    if(name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread");
        name_ = buf;
    }
}

//创建线程相关
void Thread :: start() {
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    if(pthread_create(&pthreadID_, NULL, &startThread, data)) {
        started_ = false;
        delete data; //失败
    } else {
        latch_.wait(); //成功创建，等待进入信号
        assert(tid_ > 0);
    }
}

int Thread :: join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadID_, NULL);
}