#ifndef _AD7606_H_
#define _AD7606_H_

#include "ring_buffer.h"

typedef void (* TRIGGER_CALLBACK)(float *data, unsigned int count, void *user_data);

typedef struct _ad7606 ad7606;

ad7606 * ad7606_new(unsigned int range, RingBuffer *buffer);
void     ad7606_destroy(ad7606 *ad);

/* 
 * 设置采样率并启动 AD7606
 */
void     ad7606_start(ad7606 *ad, unsigned int rate);

void     ad7606_stop(ad7606 *ad);

/* 
 * 注册 trigger 回调事件 
 */
void     ad7606_trigger(ad7606 *ad, unsigned int count, TRIGGER_CALLBACK cb, void *user_data);

unsigned int ad7606_range(ad7606 *ad);

/*
 * 设置采样通道
 */
void     ad7606_set_channel(ad7606 *ad, unsigned int ch);

#endif
