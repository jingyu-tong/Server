//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

//禁止拷贝类，不允许拷贝的类可通过继承该类实现
class noncopyable 
{
    protected:
        noncopyable() {};
        ~noncopyable() {};
    public:
        //c++11 可用delete关键字
        noncopyable(const noncopyable&) = delete;
        const noncopyable& operator=(const noncopyable&) = delete;        
    // private:
    //     //boost做法：重载拷贝和=，声明为私有，避免被拷贝
    //     noncopyable(const noncopyable&);
    //     const noncopyable& operator=(const noncopyable&);
};


#endif