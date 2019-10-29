#include <stdlib.h>
#include <string.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include "ad_workshop.h"
#include "../shared/protocol.h"
#include "queue.h"
#include "log.h"

static Void fft_task_func(UArg arg0, UArg arg1);

typedef struct _Raw {
    Queue_Elem parent;
    float *data;    /* 将指向 IPC RingIO 内数据 */
    unsigned int size;
} Raw;    
        
struct _ADWorkshop {
    Queue *queue;
    RingBuffer *buffer;
    unsigned int heap_id;
    MessageQ_QueueId notify_queue;
    int do_fft;
    Task_Handle fft_task;
    int fft_task_running;
    int fft_task_quit;
};

ADWorkshop *ad_workshop_new(RingBuffer *buffer, 
                            MessageQ_QueueId notify_queue, 
                            unsigned int heap_id) {
    ADWorkshop *shop = (ADWorkshop *)malloc(sizeof(ADWorkshop));
    memset(shop, 0, sizeof(ADWorkshop));
    shop->do_fft = false;
    shop->buffer = buffer;
    shop->heap_id = heap_id;
    shop->queue = queue_new();
    shop->notify_queue = notify_queue;
    shop->fft_task_running = false;
    shop->fft_task_quit = false;

    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.instance->name = "fft";
    taskParams.arg0 = (UArg)shop;
    taskParams.arg1 = (UArg)shop;
    taskParams.stackSize = 0x1000;

    /* 启动 FFT 线程 */
    shop->fft_task = Task_create(fft_task_func, &taskParams, NULL);
    if (shop->fft_task == NULL)
        LOG_FATAL("failed to creat fft task");

    return shop;
}

/*
 * 不断地从数据队列取出数据进行 FFT 运算
 */
static Void fft_task_func(UArg arg0, UArg arg1) {
    LOG_INFO("fft task is running");

    ADWorkshop *shop = (ADWorkshop *)arg0;

    shop->fft_task_running = true;
    while(! shop->fft_task_quit) {
        Raw *raw = (Raw *)queue_pop(shop->queue);
        ASSERT(raw);

        /* FFT 运算 */
        void FFT(float *Input, float *Cmo, unsigned int Tn);
        FFT(raw->data, raw->data, raw->size);

        free(raw);

        /* 通知 host */
        Message *msg = (Message *)MessageQ_alloc(shop->heap_id, MSG_SIZE);
        if (msg == NULL)
            LOG_FATAL("fail to malloc message from heap");

        msg->cmd = MSG_CMD_FFT_DATA;
        MessageQ_put(shop->notify_queue, (MessageQ_Msg)msg);
    }

    shop->fft_task_running = false;

    LOG_INFO("fft task is going to quit\n");
}


void ad_workshop_destroy(ADWorkshop *shop) {
    if (! shop)
        return;

    /* 等待 FFT 线程退出 */
    if (shop->fft_task_running) {
        shop->fft_task_quit = true;
        while(! shop->fft_task_running) {}
    }

    queue_destroy(shop->queue);
    free(shop);
}

void ad_workshop_do_fft(ADWorkshop *shop, int flag) {
    ASSERT(shop);    
    shop->do_fft = flag;
}

/*
 * 如果数据要进行 FFT 运算，则将数据写入 FFT 运算队列，
 * FFT 线程将会把数据从队列里取出来进行 FFT 运算，并向 host 发出通知
 */
void ad_workshop_import(ADWorkshop *shop, float *data, unsigned int size) {
    ASSERT(shop);

    if (shop->do_fft) {
        Raw *raw = (Raw *)malloc(sizeof(Raw));
        raw->data = data;
        raw->size = size;
        queue_push(shop->queue, (Queue_Elem *)raw);
    } else {
        /* 通知 host */
        Message *msg = (Message *)MessageQ_alloc(shop->heap_id, MSG_SIZE);
        if (msg == NULL)
            LOG_FATAL("fail to malloc message from heap");

        msg->cmd = MSG_CMD_RAW_DATA;
        MessageQ_put(shop->notify_queue, (MessageQ_Msg)msg);
    }
}

