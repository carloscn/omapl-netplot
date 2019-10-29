// Project.
/****************************************************************************/
/*                                                                          */
/*  @file       : ads8568.c                                                 */
/*  @Copyright  : Wei Haochen (c) 2018 MULTIBEANS_ORG All rights reserved.  */
/*  @Revision   : Ver 1.0.                                                  */
/*  @Data       : 2018.06.11 Realse.                                        */
/*  @Belong     : PROJECT.                                                  */
/*                                                                          */
/*  **code : (UTF-8) in Ubuntu 12.04_x86. vim platform.	                    */
/****************************************************************************/
/*  @Attention:                                                             */
/*  ---------------------------------------------------------------------   */
/*  |    Data    |  Behavior |     Offer     |          Content         |   */
/*  | 2018.06.11 |   create  |DelvisBeans(M) | ceate the document.      |   */
/*  ---------------------------------------------------------------------   */
/*                                                            MULTIBEANS.   */
/****************************************************************************/

/**
 * \brief   This project ads8568 adc device driver code for multi-platform.
 *
 * \License  THIS FILE IS PART OF MULTIBEANS PROJECT ;
 *           all of the files  - The core part of the project;
 *           THIS PROGRAM IS FREE SOFTWARE, JUST NEED GPL v3.0 LICENSE;
 *           YOU SHOULD HAVE RECEIVED A COPY OF WTFPL LICENSE, IF NOT,
 *           MULTIBEANS WILL TAKE APPROPRIATE MEASURES.
 *
 *           * You can download the license on our Github. ->
 *           * -> https://github.com/lifimlt  <-
 *           * Copyright (c) 2013-2018 MULTIBEANS ORG. http://www.mltbns.com/
 *           * Copyright (c) 2018 Wei Haochen.  Email: weihaochen@mltbns.com
 *
 *  \note    void.
 */

#include <stdlib.h>
#include <string.h>

#include "ads8568.h"
#include "log.h"

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Clock.h>
#define TICK_PERIOD		    	5	// 5us

#define ADS8568_IRQ_EVENT		8

/* Convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio)	(16 * (bank) + (gpio) + 1)

#define ADS8568_PAR_BUSY_GPIO_BANK	5
#define ADS8568_PAR_BUSY_GPIO_BANK_INT	SYS_INT_GPIO_B5INT


#define ADS8568_PAR_BUSY			GPIO_TO_PIN(5, 11)
#define ADS8568_PAR_CONVST			GPIO_TO_PIN(5, 13)

struct _ads8568 {

    	float *data;
    	RingBuffer *buffer;

    	unsigned int trigger;
    	TRIGGER_CALLBACK callback;
    	void *user_data;

    	unsigned int rate;
    	unsigned int range;
    	unsigned int channel;

	Hwi_Handle hwi;
    	unsigned int irq_event;
    	unsigned int event_id;

    	Clock_Handle clock;
    	unsigned int tick_period;

     	int is_running;
	int counter;


	void	(* write_reg )( ads8568 *ad, unsigned int value );
	void	(* set_channel ) ( ads8568 *ad, unsigned int ch );
	void	(* trigger_fun ) ( ads8568 *ad, unsigned int count, TRIGGER_CALLBACK cb, void *user_data );
	void	(* start )( ads8568 *ad, unsigned int rate );
	void	(* destroy ) ( ads8568 *ad );
	void	(* reset ) ( ads8568 *ad );
	void	(* stop) ( ads8568 *ad );

	void	(* irq_set )( ads8568 *ad );
	void	(* gpio_setup )( ads8568 *ad );
	void	(* emifa_init )( ads8568 *ad );

};

/**
 * \brief    This function is ADS8568 start convst
 *
 * \offer    Wei Haochen
 *
 * \param    struct ads8568_t (in ads8568.h document)
 *
 * \return   None.
 *
 *               ____       __________________
 * \note  covst_a    \_____/
 *               ____       __________________
 *        covst_b    \_____/
 *                   |<--->|
 *                     5us
 */
void ads8568_emifa_init(ads8568 *ad)  
{
	EMIFAPinMuxSetup();
	EMIFAAsyncDevDataBusWidthSelect(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_DATA_BUSWITTH_16BIT);
	EMIFAAsyncDevOpModeSelect(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_ASYNC_INTERFACE_STROBE_MODE );
	//EMIFA_ASYNC_INTERFACE_NORMAL_MODE
	EMIFAExtendedWaitConfig(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_EXTENDED_WAIT_DISABLE);
	EMIFAWaitTimingConfig(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2, EMIFA_ASYNC_WAITTIME_CONFIG(1, 5, 1, 1, 5, 1, 0 ));
}

void ads8568_gpio_setup(ads8568 *ad) 
{

	volatile unsigned int savePinMux = 0;

	savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) & \
						 ~(SYSCFG_PINMUX11_PINMUX11_19_16 | SYSCFG_PINMUX11_PINMUX11_15_12 | SYSCFG_PINMUX11_PINMUX11_11_8);

	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) = \
    		 (PINMUX11_BUSY_ENABLE | PINMUX11_CONVST_ENABLE | PINMUX11_RESRT_ENABLE | savePinMux);

	GPIODirModeSet(SOC_GPIO_0_REGS, 92, GPIO_DIR_INPUT);
	GPIODirModeSet(SOC_GPIO_0_REGS, 93, GPIO_DIR_OUTPUT);
	GPIOPinWrite(SOC_GPIO_0_REGS, 93, GPIO_PIN_HIGH);
	GPIODirModeSet(SOC_GPIO_0_REGS, 94, GPIO_DIR_OUTPUT);
	GPIOPinWrite(SOC_GPIO_0_REGS, 94, GPIO_PIN_LOW);
}

/*GPIO interrupt service function*/

void ads8568_busy_hwi(UArg ad_addr) 
{
    	ads8568 *ad = (ads8568 *)ad_addr;

    	if (ad->counter == 0) {
		ad->data = (float *)ring_buffer_acquired(ad->buffer, ad->trigger * sizeof(float));
		if (ad->data == NULL) {
			LOG_WARN("failed to acquired buffer");
			Task_sleep(1000 * 1000);
		}
	}

	/* Disable the interrupts for pins of bank AD7606_PAR_BUSY in GPIO.*/
	GPIOBankIntDisable(SOC_GPIO_0_REGS, ADS8568_PAR_BUSY_GPIO_BANK);

	/* Clears the Interrupt Status of AD7606_PAR_BUSY in GPIO.*/
	GPIOPinIntClear(SOC_GPIO_0_REGS, ADS8568_PAR_BUSY);

	/* Set low convst gpio to prepare the next conversion  */
	GPIOPinWrite(SOC_GPIO_0_REGS, ADS8568_PAR_CONVST, GPIO_PIN_LOW);

	if (ad->data != NULL) {
		int values[8];
		int i;
		for (i = 0; i < 8; i++)
		    	values[i] = ((Int16 *)SOC_EMIFA_CS2_ADDR)[0x00]; 
		
		int16_t raw = values[ad->channel];
		ad->data[ad->counter++] = (float)raw * ad->range / 32768;
	}

	if (ad->counter == ad->trigger) { /* trigger event set*/

		ad->counter = 0;
		ring_buffer_release(ad->buffer, ad->trigger * sizeof(float));

		if (ad->callback)  /* callback */
            	ad->callback(ad->data, ad->trigger, ad->user_data);
    	}

	/* Enable interrupts for pins of bank AD7606_PAR_BUSY.*/
	GPIOBankIntEnable(SOC_GPIO_0_REGS, ADS8568_PAR_BUSY_GPIO_BANK);
}

void ads8568_irq_setup(ads8568 *ad) 
{

	Hwi_Params hwiParams;
	Hwi_Handle myHwi;
	Error_Block eb;

	Error_init(&eb);
	Hwi_Params_init(&hwiParams);
	hwiParams.arg = (unsigned int)ad;
	hwiParams.enableInt = FALSE;
	hwiParams.eventId = ad->event_id;
	hwiParams.priority = 4;
	myHwi = Hwi_create(ad->irq_event, ads8568_busy_hwi, &hwiParams, &eb);
	if (myHwi == NULL) 
        LOG_FATAL("failed to create hwi");

	/* enable both interrupts */
	Hwi_enableInterrupt(ad->irq_event);
}

ads8568 * ads8568_new(unsigned int range, RingBuffer *buffer) 
{
    	ads8568 *ad = (ads8568 *)malloc(sizeof(ads8568));
    	memset(ad, 0, sizeof(ads8568));

    	ad->irq_event = ADS8568_IRQ_EVENT;
    	ad->event_id = ADS8568_PAR_BUSY_GPIO_BANK_INT;

    	ad->range = range;
    	ad->buffer = buffer;
    	ad->counter = 0;
    	ad->channel = 0;

	/* The Local PSC number for GPIO is 3. GPIO belongs to PSC1 module.*/
	PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_EMIFA, PSC_POWERDOMAIN_ALWAYS_ON,
			PSC_MDCTL_NEXT_ENABLE);

	ad->write_reg		=	&ads8568_write_reg;
	ad->set_channel		=	&ads8568_set_channel;
	ad->trigger_fun		=	&ads8568_trigger;
	ad->start		=	&ads8568_start;
	ad->destroy		=	&ads8568_destroy;
	ad->reset		=	&ads8568_reset;
	ad->irq_set		=	&ads8568_irq_setup;
	ad->gpio_setup		=	&ads8568_gpio_setup;
	ad->emifa_init		=	&ads8568_emifa_init;
	ad->stop		=	&ads8568_stop;
	ad->emifa_init(ad);
	ad->gpio_setup(ad);
	ad->irq_set(ad);
	ad->reset(ad);

	ad->write_reg(ad, \
		      AD8568_WRITE_ENABLE   |	\
                      AD8568_REF_ENABLE     |	\
		      AD8568_REF_SET3V      | 	\
	              AD8568_REFDAC_FULL);
    	return ad;
}

void 	ads8568_reset( ads8568 *ad )
{
	uint16_t count = 0x1FFF;
	GPIOPinWrite(SOC_GPIO_0_REGS, 93, GPIO_PIN_HIGH);
	while(count--);
	GPIOPinWrite(SOC_GPIO_0_REGS, 93, GPIO_PIN_LOW);
}

void ads8568_destroy(ads8568 *ad) 
{
    	if (ad)	free(ad);
}

void ads8568_on_ticked(UArg ad_ptr) 
{
	/*ads8568_start*/
	GPIOPinWrite(SOC_GPIO_0_REGS, ADS8568_PAR_CONVST, GPIO_PIN_HIGH);
}

void ads8568_start(ads8568 *ad, unsigned int rate) 
{
    	//ASSERT(ad); 
    	if (rate > 2000000 / TICK_PERIOD) {
        	LOG_WARN("unstatisfied sample rate, set as max %d", 2000000 / TICK_PERIOD);
        	rate = 1000000 / TICK_PERIOD;
    	}

   	 /* calculate time period. */
    	int period = 2000000 / TICK_PERIOD / rate;

    	if (! ad->is_running) {   /* first start */
        	Error_Block eb;

        	Clock_Params params;
        	Clock_Params_init(&params);
        	params.period = period;
        	params.arg = (unsigned int)ad;
        	params.startFlag = TRUE;
        	ad->clock = Clock_create(ads8568_on_ticked, period, &params, &eb);
        	if (ad->clock == NULL)
        	    LOG_FATAL("failed to create clock");

        	Clock_start(ad->clock);
    	}	
    	else { 
        	Clock_stop(ad->clock); 
        	Clock_setPeriod(ad->clock, period);
        	Clock_start(ad->clock);
    	}
    	ad->is_running = true;
}

void ads8568_stop( ads8568 *ad ) 
{
    	ASSERT(ad); 
    	ASSERT(ad->is_running);
    	Clock_stop(ad->clock);
    	ad->is_running = false;
}

void ads8568_trigger(ads8568 *ad, unsigned int count, TRIGGER_CALLBACK cb, void *user_data) 
{
    	ASSERT(ad); 
    	ASSERT(cb);
    	ad->trigger = count;
    	ad->callback = cb;
    	ad->user_data = user_data;

    	LOG_DEBUG("set trigger as %d", ad->trigger);
}

void ads8568_set_channel(ads8568 *ad, unsigned int ch) {
    	ASSERT(ad);
    	ASSERT((int)ch > -1 && (int)ch < 8);
    	ad->channel = ch;
}

void ads8568_write_reg( ads8568 *ad, unsigned int value ) 
{
	((short *)SOC_EMIFA_CS2_ADDR)[0] = (value >> 16) & 0x0000FFFF;
	((short *)SOC_EMIFA_CS2_ADDR)[0] = value & 0x0000FFFF;
}



