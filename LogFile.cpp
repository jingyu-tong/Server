//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "LogFile.h"

LogFile::LogFile(const std::string base_name, int flush_times) 
    :   base_name_(base_name),
        flush_times_(flush_times),
        count_(0),
        mutex_(new MutexLock())
{
    file_.reset(new AppendFile(base_name));
}

LogFile::~LogFile() {

}

void LogFile::append(const char* logline, int len) {
    MutexLockGuard lock(*mutex_);
    append_unlock(logline, len);
}

void LogFile::flush() {
    MutexLockGuard lock(*mutex_);
    file_->flush();
}

void LogFile::append_unlock(const char* logline, int len) {
    file_->append(logline, len);
    ++count_;
    if(count_ >= flush_times_) {
        count_ = 0;
        file_->flush();
    }
}