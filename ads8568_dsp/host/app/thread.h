#ifndef _BASE_THREAD_H_
#define _BASE_THREAD_H_

#include <string>
#include <pthread.h>

namespace base {

class ThreadWorker {
public:
    virtual ~ThreadWorker() {};
    virtual void *run() = 0;
};

class Thread {
public:
    Thread(ThreadWorker *worker, std::string name = "");
    ~Thread();

    void start();
    void join();

    void set_priority(int priority); 

    static pthread_t id();
    static std::string id_str();
    static const char *name();
    static bool is_runing_in(pthread_t tid);

private:
    static void *thread_main_func(void *data);

private:
    static int count;

private:
    pthread_t m_tid;
    ThreadWorker *m_worker;
    std::string m_name;
    bool m_is_started;
    bool m_is_joined;
    int m_priority;
};

}

#endif  
