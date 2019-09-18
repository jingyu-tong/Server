//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "AsyncLogging.h"
#include "LogFile.h"

AsyncLogging::AsyncLogging(const std::string base_name, int flush_interval) 
    :   flush_interval_(flush_interval),
        running_(false),
        base_name_(base_name),
        thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
        mutex_(),
        cond_(mutex_),
        cur_buffer_(new Buffer),
        next_buffer_(new Buffer),
        buffers_(),
        latch_(1)
{
    assert(base_name_.size() > 1);
    cur_buffer_->bzero();
    next_buffer_->bzero();
    buffers_.reserve(16); //申请一点
}

void AsyncLogging::append(const char* logline, int len) {
    MutexLockGuard lock(mutex_); //临界区
    if(cur_buffer_->avail() > len) {
        cur_buffer_->append(logline, len);
    } else {
        buffers_.push_back(std::move(cur_buffer_)); //不够，装入buffers
        if(next_buffer_) {
            cur_buffer_ = std::move(next_buffer_);
        } else {
            cur_buffer_.reset(new Buffer);
        }

        cur_buffer_->append(logline, len);
        cond_.notify(); //有装满的，通知后台线程
    }

}

//子线程函数
//也常备2块缓冲区
void AsyncLogging::threadFunc() {
    assert(running_);
    latch_.countdown(); //什么用？？？
    LogFile output(base_name_);
    BufferPtr buffer1(new Buffer);
    BufferPtr buffer2(new Buffer);
    buffer1->bzero();
    buffer2->bzero();

    BufferVector buffers_towrite;
    buffers_towrite.reserve(16);
    while(running_) {
        assert(buffer1 && buffer1->length() == 0);
        assert(buffer2 && buffer2->length() == 0);
        assert(buffers_towrite.empty());

        //需要操作前台共有的两块以及vector中的数据
        {
            MutexLockGuard lock(mutex_);
            if(buffers_.empty()) { //被唤醒，但没有可写的
                cond_.waitForSeconds(flush_interval_);
            }
            buffers_.push_back(std::move(cur_buffer_)); //最后一个可能有没写满的
            buffers_towrite.swap(buffers_);

            cur_buffer_ = std::move(buffer1);
            if(!next_buffer_) {
                next_buffer_ = std::move(buffer2);
            }
        }
          
        assert(!buffers_towrite.empty());

        //太大，drop
        if(buffers_towrite.size() > 25) {
            buffers_towrite.erase(buffers_towrite.begin() + 2, buffers_towrite.end()); //留两块
        }

        for(size_t i = 0; i < buffers_towrite.size(); ++i) {
            output.append(buffers_towrite[i]->data(), buffers_towrite[i]->length());
        }

        if(buffers_towrite.size() > 2) {
            buffers_towrite.resize(2);
        }

        if(!buffer1) {
            assert(!buffers_towrite.empty());
            buffer1 = std::move(buffers_towrite.back());
            buffers_towrite.pop_back();
            buffer1->reset();
        }

        if(!buffer2) {
            assert(!buffers_towrite.empty());
            buffer2 = std::move(buffers_towrite.back());
            buffers_towrite.pop_back();
            buffer2->reset();
        }

        buffers_towrite.clear();
        output.flush();
    }
    output.flush();
}

