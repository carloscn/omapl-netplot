#include <sstream>
#include <iomanip>
#include <iostream>
#include "thread.h"
#include <assert.h>

using namespace std;
using namespace base;

namespace base {

__thread pthread_t thread_id;
__thread char thread_id_str[64];
__thread char thread_name[64];

}

int Thread::count = 1;

static void set_priority(pthread_attr_t *tattr, int priority) {
    pthread_attr_init(tattr);

    (void)pthread_attr_setschedpolicy(tattr, SCHED_FIFO);

    struct sched_param param;
    pthread_attr_getschedparam(tattr, &param);
    param.sched_priority = priority;

    (void)pthread_attr_setschedparam(tattr, &param);
}   

Thread::Thread(ThreadWorker *worker, string name) 
    : m_worker(worker), 
      m_name(name), 
      m_is_started(false), 
      m_is_joined(false),
      m_priority(50) { 
    Thread::count += 1;
    // TODO: cache thread_id and thread_name
}

Thread::~Thread() { 
    if (m_is_started && ! m_is_joined)
        pthread_detach(m_tid);
}

pthread_t Thread::id() {
    return pthread_self(); // TODO: cache id over global namespace
}

void Thread::set_priority(int priority) {
    m_priority = priority;
}

string Thread::id_str() {
    stringstream ss;
    ss << std::setw(14) << std::setfill('.') << Thread::id();
    return ss.str();
}

const char *Thread::name() {
    return "thread_name"; // TODO: cache name over global namespace
}

bool Thread::is_runing_in(pthread_t tid) {
    return pthread_equal(tid, Thread::id()) != 0;
}

void Thread::start() {
    pthread_attr_t tattr;
    ::set_priority(&tattr, m_priority);
    assert(pthread_create(&m_tid, &tattr, thread_main_func, this) == 0);
    m_is_started = true;
}

void Thread::join() {
    pthread_join(m_tid, NULL);
    m_is_joined = true;
}

void *Thread::thread_main_func(void *data) {
    Thread *thread = static_cast<Thread*>(data);
    return thread->m_worker->run();
}
