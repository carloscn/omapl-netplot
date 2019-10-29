#ifndef _AD_WORKSHOP_H_
#define _AD_WORKSHOP_H_

#include <ti/ipc/MessageQ.h>
#include "ring_buffer.h"

typedef struct _ADWorkshop ADWorkshop;

ADWorkshop * ad_workshop_new(RingBuffer *buffer, MessageQ_QueueId notify_queue, unsigned int heap_id);
void         ad_workshop_destroy(ADWorkshop *shop);

void         ad_workshop_do_fft(ADWorkshop *shop, int flag);

/*
 * AD 数据的处理。进行 FFT 或直接通知 host 读取新的数据
 */
void         ad_workshop_import(ADWorkshop *shop, float *raw, unsigned int size);

#endif
