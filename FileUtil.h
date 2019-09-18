//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef FILEUTIL_H_
#define FILEUTIL_H_

#include "base/noncopyable.h"

#include <string>

class AppendFile : noncopyable {
    public:
        explicit AppendFile(std::string filename);
        ~AppendFile();
        void append(const char* logline, const size_t len);
        void flush();

    private:
        size_t write(const char* logline, size_t len);
        FILE* fp_;
        char buffer_[64 * 1024];
};

#endif