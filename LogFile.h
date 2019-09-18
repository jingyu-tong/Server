//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef LOGFILE_H_
#define LOGFILE_H_

#include "FileUtil.h"
#include "base/MutexLock.h"

#include <memory>
#include <string>

class LogFile : noncopyable {
    public:
        LogFile(const std::string base_name, int flush_times = 1024);
        ~LogFile();

        void append(const char* logline, int len); //写文件，带缓冲
        void flush(); //冲刷
        bool rollFile(); 

    private:
        void append_unlock(const char* logline, int len);
        
        std::string base_name_;
        int flush_times_;
        int count_;
        std::unique_ptr<MutexLock> mutex_;
        std::unique_ptr<AppendFile> file_;
};





#endif