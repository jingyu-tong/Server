//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "HttpServer.h"
#include "Connection.h"
#include "Timer.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

const int kAliveTime = 60 * 1000; //ms
const int kShortTime  = 2 * 1000; //ms

char favicon[555] = {
  '\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
  '\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
  '\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
  '\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
  'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
  't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
  'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
  'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
  'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
  '\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
  '\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
  '\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
  '\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
  'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
  'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
  'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
  'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
  'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
  'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
  'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
  'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
  '\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
  '\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
  '\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
  '\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
  '\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
  '9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
  '\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
  '\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
  'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
  '\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
  '\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
  '\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
  'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
  '\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
  '3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
  '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
  '\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
  '\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
  '\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
  '\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
  'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
  '\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
  '\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
  'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
  'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
  '\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
  '\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
  '\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
  '\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
  'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
  '\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
  '\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
  '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
  '\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
  '\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
  '\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
  '\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
  '\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
  '\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
  '\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
  'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
  'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
  '\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
  '\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
  '\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
  '\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
  '\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
  '\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
  'B', '\x60', '\x82',
};

std::map<std::string, std::string> mime = {
    {".html",  "text/html"},
    {".avi", "video/x-msvideo"},
    {".bmp", "image/bmp"},
    {".c", "text/plain"},
    {".doc", "application/msword"},
    {".gif", "image/gif"},
    {".gz", "application/x-gzip"},
    {".htm", "text/html"},
    {".ico", "image/x-icon"},
    { ".jpg", "image/jpeg"},
    {".png", "image/png"},
    {".txt", "text/plain"},
    {".mp3", "audio/mp3"},
    {"default", "text/html"},
};

HttpServer::HttpServer(EventLoop* loop, int port, int thread_num) 
    :   loop_(loop),
        server_(loop_, port, thread_num)
{
    //server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setConnectionCallback([this](const ConnectionPointer& conn) {this->onConnection(conn); });
    //server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    server_.setMessageCallback([this](const ConnectionPointer& conn, Buffer& msg) {this->onMessage(conn, msg); });
    server_.start();
}

void HttpServer::onConnection(const ConnectionPointer& conn) {
    if(conn->getState() == Connection::kconnected) {
        // HttpInformation* info = conn->getHttpinfo();
        // TimerManager::TimerPointer timer = info->getTimer();
        // timer = conn->getLoop()->runAfter(std::bind(&Connection::foceClose, conn), kShortTime); //长时间没收到消息，关闭该链接
        //LOG << "Connection fd = " << conn->getFd() << " is connected"; 
    }
    //TODO(jingyu):HttpServer需要一个自己的删除函数，在里头调用forceclose
}

void HttpServer::onMessage(const ConnectionPointer& conn, Buffer& msg) {
    HttpInformation* info = conn->getHttpinfo();
    if(info->getState() != HttpInformation::kFinished && !info->getError()) {
        parseRequest(conn, info, msg);
    } 
    if(info->getState() == HttpInformation::kFinished) {  //处理请求并发送响应
        std::string out = analyzeRequest(conn, info, msg);
        conn->send(out);
        std::string conn_state = info->getHeaders("Connection");
        if(conn_state == "close" || conn_state == "Close" ) {
            conn->forceClose(); //短连接，直接关闭
        } else {
            info->setState(HttpInformation::kExpectRequest);
        }
    }
}

void HttpServer::parseRequest(const ConnectionPointer& conn, HttpInformation* info, Buffer& msg) {
    //处理请求，这里需要用while，因为有可能一次收完多个部分
    //处理完前一个部分，需要在下一次循环检查下一部分，通过改变state实现
    while(true) {
        HttpInformation::State state = info->getState();
        if(state == HttpInformation::kExpectRequest) { 
            size_t pos = msg.find("\r\n"); 
            if(pos == std::string::npos) { //没收完，继续收
                return;
            }
            std::string request = msg.substr(0, pos);
            //LOG << "Connection fd = " << conn->getFd() << " Request: " << request;
            if(msg.size() >= pos + 2)  //删除request行
                msg = msg.substr(pos + 2); 
            
            //根据request设置method，uri，以及http版本
            //method
            size_t pos_get = request.find("GET");
            size_t pos_post = request.find("POST");
            size_t pos_head = request.find("HEAD");
            if(pos_get != std::string::npos) {
                info->setMethod("GET");
            } else if(pos_post != std::string::npos) {
                info->setMethod("POST");
            } else if(pos_head != std::string::npos) {
                info->setMethod("HEAD");
            } else {
                info->setError(true);
                handleError(conn, 400, "Bad Request");
                break;
            }
            
            //uri
            pos = request.find("/");
            if(pos == std::string::npos) {
                info->setUri("index.html");
                info->setVerison("HTTP/1.1");
            } else {
                size_t uri_end = request.find(' ', pos); 
                if(uri_end < 0) {
                    info->setError(true);
                    handleError(conn, 400, "Bad Request");
                    break;
                } else {
                    info->setUri(request.substr(pos + 1, uri_end - pos - 1));
                }
                pos = uri_end;
            }
            
            //verison
            pos = request.find("/", pos);
            if(pos == std::string::npos) {
                info->setError(true);
                handleError(conn, 400, "Bad Request");
                break;
            } else {
                if(request.size() - pos <=3) {
                    info->setError(true);
                    handleError(conn, 400, "Bad Request");
                    break;
                } else {
                    std::string verison = request.substr(pos + 1, 3);
                    info->setVerison(verison);
                }
            }
            info->setState(HttpInformation::kExpectHeader);
        } else if(state == HttpInformation::kExpectHeader) {
            size_t pos = msg.find("\r\n"); 
            if(pos == std::string::npos) {
                return;
            } else {
                size_t colon= msg.find(':');
                if(colon == std::string::npos) { //空行，headers 结束
                    msg = msg.substr(pos + 2);
                    if(info->getMethod() == "POST") {
                        info->setState(HttpInformation::kExpectBody);
                    } else {
                        info->setState(HttpInformation::kFinished);
                    }
                } else {
                    std::string key = msg.substr(0, colon);
                    size_t val_start = msg.find(' ');
                    std::string val  = msg.substr(val_start + 1, pos - val_start - 1);
                    info->setHeaders(key, val);
                    msg = msg.substr(pos + 2);
                }
            }
        } else if(state == HttpInformation::kExpectBody) {
            int content_length = -1;
            std::string content_str = info->getHeaders("Content-Length");
            if(content_str != "") {
                content_length = stoi(content_str);
            } else {
                info->setError(true);
                handleError(conn, 400, "Bad Request: Lack of Content-Length");
                break;
            }

            // printf("content-length: %d", content_length);
            if(msg.size() < content_length) { //没有收完
                return;
            } else { //全部结束
                info->setState(HttpInformation::kFinished);
            }
        } else { //kFinished
            return;
        }
    }
}

//分析request
//TODO(jingyu): 对mmap read sendfile三者进行分析，并进行压测，综合分析、理解
std::string HttpServer::analyzeRequest(const ConnectionPointer& conn, HttpInformation* info, Buffer& msg) {
    std::string method = info->getMethod();
    std::string uri = info->getUri();
    std::string out;
    HttpInformation::WeakTimer timer = info->getTimer();
    if(method == "GET" || method == "HEAD") {
        std::string header;
        header += "HTTP/" + info->getVerison() + " 200 OK\r\n";
        //1.1默认为长连接
        std::string conn_state = info->getHeaders("Connection");
        if(conn_state != "" && (conn_state == "keep-alive" || conn_state == "Keep-Alive")) {
            //长链接
            header += std::string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + std::to_string(kAliveTime) + "\r\n";
            if(timer.expired()) {
                //timer = conn->getLoop()->runAfter(std::bind(&Connection::forceClose, conn), kShortTime); //长时间没收到消息，关闭该链接
                timer = conn->getLoop()->runAfter([conn]() {conn->forceClose(); }, kShortTime); //长时间没收到消息，关闭该链接
                info->setTimer(timer);
            } else {
                auto guard = timer.lock();
                conn->getLoop()->updateTimer(guard, kAliveTime); //收到消息，更新时间
            }  
        } else if(info->getVerison() == "1.1" && conn_state != "close") {
            header += std::string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + std::to_string(kAliveTime) + "\r\n";
            info->setHeaders("Connection", "Keep-Alive");
            if(timer.expired()) {
                //timer = conn->getLoop()->runAfter(std::bind(&Connection::forceClose, conn), kShortTime); //长时间没收到消息，关闭该链接
                timer = conn->getLoop()->runAfter([conn]() {conn->forceClose(); }, kShortTime); //长时间没收到消息，关闭该链接
                info->setTimer(timer);
            } else {
                auto guard = timer.lock();
                conn->getLoop()->updateTimer(guard, kAliveTime); //收到消息，更新时间
            }  
        } else {
            // if(!timer) {
            //     timer = conn->getLoop()->runAfter(std::bind(&Connection::foceClose, conn), kShortTime); //长时间没收到消息，关闭该链接
            //     info->setTimer(timer);
            // } else {
            //     printf("update\n");
            //     conn->getLoop()->updateTimer(timer, kAliveTime); //收到消息，更新时间
            // } 
        }

        if(uri == "hello") {
            char hello[] = {"Hello World\n"};
            header += "Content-Type: text/plain\r\n";
            header += "Content-length: " + std::to_string(sizeof hello) + "\r\n";
            header += "Server: Jingyu's Server\r\n\r\n";

            out += header;
            out += std::string(hello, hello + sizeof hello);
            return out;
        } 
        if(uri == "favicon.ico") {
            header += "Content-Type: image/png\r\n";
            header += "Content-length: " + std::to_string(sizeof favicon) + "\r\n";
            header += "Server: Jingyu's Server\r\n\r\n";

            out += header;
            out += std::string(favicon, favicon + sizeof favicon);
            return out;
        }
        
        struct stat buf;
        if(stat(uri.c_str(), &buf) < 0) {
            info->setError(true);
            handleError(conn, 404, "Not Found!");
            return "";
        }

        //确定类型
        size_t type_pos = uri.find('.');
        std::string type;
        if(type_pos == std::string::npos) {
            type = mime[".html"];
        } else {
            type = mime[uri.substr(type_pos)];
        }

        header += "Content-Type: " + type + "\r\n";
        header += "Content-Length: " + std::to_string(buf.st_size) + "\r\n";
        header += "Server: Jingyu's Server\r\n\r\n";
        out += header;
        
        //头部结束，判断是否为HEAD方法
        if(method == "HEAD") {
            return header;
        }
        
        int file_fd = open(uri.c_str(), O_RDONLY, 0);
        if(file_fd < 0) {
            info->setError(true);
            handleError(conn, 404, "Not Found!");
            return "";
        }

        void* mmap_ret = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
        close(file_fd);
        if(mmap_ret == (void*)-1) {
            munmap(mmap_ret, buf.st_size);
            info->setError(true);
            handleError(conn, 404, "Not Found!");
            return "";
        }
        
        char* file_ptr = static_cast<char*>(mmap_ret);
        out += std::string(file_ptr, file_ptr + buf.st_size);
        munmap(mmap_ret, buf.st_size);
        return out;
    }
}

void HttpServer::handleError(const ConnectionPointer& conn, int err_num, std::string str) {
    str = " " + str;
    std::string body;
    std::string header;

    body += "<html><title>哎~出错了</title>";
    body += "<body bgcolor=\"ffffff\">";
    body += std::to_string(err_num) + str;
    body += "<hr><em> Jingyu's Web Server</em>\n</body></html>";
    
    header += "HTTP/1.1 " + std::to_string(err_num) + str + "\r\n";
    header += "Content-Type: text/html\r\n";
    header += "Connection: Close\r\n";
    header += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    header += "Server: Jingyu's Web Server\r\n";;
    header += "\r\n";

    header += body;
    conn->send(header);
}