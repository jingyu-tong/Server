//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Logging.h"
#include "base/Thread.h"
#include "AsyncLogging.h"

#include <assert.h>
#include <string>
#include <sys/time.h>

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging* AsyncLogger_;

std::string Logger::base_name_ = "./Jingyu_Server.log";

//初始化一个AsyncLogger，只执行一次
void once_init() {
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}

void output(const char* msg, int len) {
    pthread_once(&once_control_, once_init);
    AsyncLogger_->append(msg, len);
}

Logger::Impl::Impl(const char* base_name, int line) 
    :   stream_(),
        line_(line),
        base_name_(base_name)
{
    formatTime();
}

void Logger::Impl::formatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char* base_name, int line) 
    :   impl_(base_name, line)
{

}

Logger::~Logger() {
    impl_.stream_ << " -- " << impl_.base_name_ << ':' << impl_.line_ << '\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}