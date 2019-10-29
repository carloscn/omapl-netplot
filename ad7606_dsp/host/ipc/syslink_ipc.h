#ifndef _SYSLINK_IPC_H_
#define _SYSLINK_IPC_H_

#include <string>

class SysLinkIPC {
public:
    SysLinkIPC(const std::string &proc_name);
    ~SysLinkIPC();

    void start();
    void stop();

    int get_proc_id() { return m_proc_id; }

private:
    std::string m_proc_name;
    int m_proc_id; 
};

#endif
