//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef LOGGING_H_
#define LOGGING_H_

#include "LogStream.h"

#include <string>

class Logger {
    public:
        Logger(const char* base_name, int line);
        ~Logger();
        LogStream& stream() {
            return impl_.stream_;
        }

        static void setLogFileName(std::string filename) {
            base_name_ = filename;
        }
        static std::string getLogFileName() {
            return base_name_;
        }

    private:
        class Impl {
            public:
                Impl(const char* base_name, int line);
                void formatTime();

                LogStream stream_; //用来格式化输出
                int line_;
                std::string base_name_;
        };

        Impl impl_;
        static std::string base_name_;
};

//采用临时对象，这样会立即析构
//用局部变量会造成输出顺序倒叙
#define LOG Logger(__FILE__, __LINE__).stream()













#endif