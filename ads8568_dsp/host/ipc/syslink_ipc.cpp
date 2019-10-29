#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <ti/syslink/Std.h>     
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/SysLink.h>
#include <ti/ipc/MultiProc.h>

#include "syslink_ipc.h"
#include "log.h"

using namespace std;

SysLinkIPC::SysLinkIPC(const std::string &proc_name)
    : m_proc_name(proc_name), m_proc_id(-1) {
    SysLink_setup();
    char *name = strdup(m_proc_name.c_str());
    m_proc_id = MultiProc_getId(name);
    free(name);
}

SysLinkIPC::~SysLinkIPC() {
    SysLink_destroy();
}

void SysLinkIPC::start() {
    if (Ipc_control(m_proc_id, Ipc_CONTROLCMD_LOADCALLBACK, NULL) < 0)
        LOG_FATAL("load callback failed");

    if (Ipc_control(m_proc_id, Ipc_CONTROLCMD_STARTCALLBACK, NULL) < 0)
        LOG_FATAL("start callback failed");
}

void SysLinkIPC::stop() {
    if (Ipc_control(m_proc_id, Ipc_CONTROLCMD_STOPCALLBACK, NULL) < 0)
        LOG_FATAL("stop callback failed");
}
