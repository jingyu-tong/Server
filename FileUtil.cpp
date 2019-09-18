//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "FileUtil.h"

AppendFile::AppendFile(std::string filename) 
    :   fp_(fopen(filename.c_str(), "ae")) //追加，exec后关闭
{
    setbuffer(fp_, buffer_, sizeof buffer_); //使用缓冲区，尽可能减少read和write调用的次数
}

AppendFile::~AppendFile() {
    fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len) {
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while(remain > 0) {
        size_t x = this->write(logline + n, remain);
        if(x == 0) {
            int err = ferror(fp_);
            if(err)
                fprintf(stderr, "AppendFile::append() failed! \n");
            break;
        }
        n += x;
        remain = len - x;
    }
}

//将未写的数据输出，并清空缓冲区
void AppendFile::flush() {
    fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len) {
    return fwrite_unlocked(logline, 1, len, fp_); //不加锁
}