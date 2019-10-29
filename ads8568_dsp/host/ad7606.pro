MCSDK                    = /opt/ti
QWT_INSTALL_DIR          = /usr/local/qwt-6.1.0

SYSLINK_INSTALL_DIR      = $$MCSDK/syslink_2_21_01_05
IPC_INSTALL_DIR          = $$MCSDK/ipc_1_25_03_15

QT      +=  network
TARGET       = ad7606

TEMPLATE     = app

VPATH       += app ipc qt

OBJECTS_DIR  = tmp
MOC_DIR      = tmp

CONFIG      += debug

SOURCES     += main.cpp                            \
               mainwindow.cpp                      \
               plot.cpp                            \ 
               thread.cpp                          \
               sample_receiver.cpp                 \
               syslink_ipc.cpp                     \
               message_queue.cpp                   \
               message_heap.c                      \
               ring_buffer.cpp                     \
               controler.cpp \
    app/netclientthread.cpp

HEADERS     += mainwindow.h                        \
               plot.h                              \
               thread.h                            \
               sample_receiver.h                   \
               syslink_ipc.h                       \
               message_queue.h                     \
               message_heap.h                      \
               ring_buffer.h                       \
               controler.h \
    app/netclientthread.h

INCLUDEPATH += app ipc qt                          \
               $$QWT_INSTALL_DIR/include           \
               $$SYSLINK_INSTALL_DIR/packages      \
               $$IPC_INSTALL_DIR/packages      

LIBS        += -L$$QWT_INSTALL_DIR/lib -lqwt       \
               $$SYSLINK_INSTALL_DIR/packages/ti/syslink/lib/syslink.a_release
