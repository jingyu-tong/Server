//@author Jingyu Tong
//@email: jingyutong0806@gmail.com
#ifndef CURRENTTHREAD_H_
#define CURRENTTHREAD_H_

#include <stdint.h>

//存放一些当前线程相关的信息，利用__thread关键字
namespace CurrentThread {
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;

    //缓存tid
    void cacheTid();
    inline int tid() {
        if(__builtin_expect(t_cachedTid == 0, 0)) {
            cacheTid();
        }
        return t_cachedTid;
    }
    inline const char* tidString() {
        return t_tidString;
    }
    inline int tidStringLength() {
        return t_tidStringLength;
    }
    inline const char* name() {
        return t_threadName;
    }
}



#endif