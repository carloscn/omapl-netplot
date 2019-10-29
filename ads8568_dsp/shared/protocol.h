#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#if defined (__cplusplus)
extern "C" {
#endif

/* package header files */
#include <ti/ipc/MessageQ.h>

#include <stdint.h>

#define MSG_SIZE (sizeof(Message))

#define SHARED_REGION_ID_MSG_QUEUE   0x0
#define SHARED_REGION_ID_RING_BUFFER 0x1

#define RING_BUFFER_NAME      "RING_BUFFER"

#define MSG_QUEUE_NAME_HOST   "MSG_QUEUE_NAME_HOST"
#define MSG_QUEUE_NAME_SLAVE  "MSG_QUEUE_NAME_SLAVE"

typedef enum {
    MSG_CMD_START,
    MSG_CMD_STOP,

    MSG_CMD_SAMPLE_RATE,

    MSG_CMD_ENABLE_FFT,
    MSG_CMD_DISABLE_FFT,

    MSG_CMD_FFT_DATA,
    MSG_CMD_RAW_DATA
        
} MsgCmd;

typedef struct _Message {
    MessageQ_MsgHeader parent;
    uint32_t cmd; 
    uint32_t rate;
    uint32_t trigger;
    uint32_t channel;
} Message;

#if defined (__cplusplus)
}
#endif

#endif
