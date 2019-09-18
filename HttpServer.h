//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "base/noncopyable.h"
#include "Server.h"
#include "Connection.h"

class HttpServer : noncopyable {
    public:
        HttpServer(EventLoop* loop, int port, int thread_num);
        ~HttpServer() {}

        //Server的两个回调
        void onMessage(const ConnectionPointer& conn, Buffer& msg);
        void onConnection(const ConnectionPointer& conn);

        //解析和分析request
        void parseRequest(const ConnectionPointer& conn, HttpInformation* info, Buffer& msg);
        std::string analyzeRequest(const ConnectionPointer& conn, HttpInformation* info, Buffer& msg);

        //错误处理
        void handleError(const ConnectionPointer& conn, int err_num, std::string str);
    private:
        EventLoop* loop_;
        Server server_;
};












#endif