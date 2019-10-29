#include <stdlib.h>
#include <string.h>

#include "ad7606.h"
#include "log.h"

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Clock.h>

#include "hw/hw_types.h"
#include "hw/hw_syscfg0_OMAPL138.h"
#include "hw/hw_psc_OMAPL138.h"
#include "hw/soc_OMAPL138.h"
#include "c674x/omapl138/interrupt.h"
#include "psc.h"
#include "gpio.h"
#include "emifa.h"

#define TICK_PERIOD		    5	// 5us

#define AD7606_IRQ_EVENT	8

/* Convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio)	(16 * (bank) + (gpio) + 1)

#define AD7606_PAR_BUSY_GPIO_BANK	5
#define AD7606_PAR_BUSY_GPIO_BANK_INT	SYS_INT_GPIO_B5INT

#define AD7606_PAR_BUSY			GPIO_TO_PIN(5, 11)
#define AD7606_PAR_CONVST		GPIO_TO_PIN(5, 13)

#define PINMUX11_BUSY_ENABLE    (SYSCFG_PINMUX11_PINMUX11_19_16_GPIO5_11 << \
					SYSCFG_PINMUX11_PINMUX11_19_16_SHIFT)

#define PINMUX11_CONVST_ENABLE    (SYSCFG_PINMUX11_PINMUX11_11_8_GPIO5_13 << \
					SYSCFG_PINMUX11_PINMUX11_11_8_SHIFT)

#define PINMUX12_EMIF_A_ENABLE     (SYSCFG_PINMUX12_PINMUX12_31_28_EMA_A0 << \
					SYSCFG_PINMUX12_PINMUX12_31_28_SHIFT) | \
					(SYSCFG_PINMUX12_PINMUX12_27_24_EMA_A1 << \
					SYSCFG_PINMUX12_PINMUX12_27_24_SHIFT) | \
					(SYSCFG_PINMUX12_PINMUX12_23_20_EMA_A2 << \
					SYSCFG_PINMUX12_PINMUX12_23_20_SHIFT) | \
					(SYSCFG_PINMUX12_PINMUX12_19_16_EMA_A3 << \
					SYSCFG_PINMUX12_PINMUX12_19_16_SHIFT) | \
					(SYSCFG_PINMUX12_PINMUX12_15_12_EMA_A4 << \
					SYSCFG_PINMUX12_PINMUX12_15_12_SHIFT) | \
					(SYSCFG_PINMUX12_PINMUX12_11_8_EMA_A5 << \
					SYSCFG_PINMUX12_PINMUX12_11_8_SHIFT) | \
					(SYSCFG_PINMUX12_PINMUX12_7_4_EMA_A6 << \
					SYSCFG_PINMUX12_PINMUX12_7_4_SHIFT) | \
					(SYSCFG_PINMUX12_PINMUX12_3_0_EMA_A7 << \
					SYSCFG_PINMUX12_PINMUX12_3_0_SHIFT)
	
#define PINMUX11_EMIF_A_ENABLE     (SYSCFG_PINMUX11_PINMUX11_31_28_EMA_A8 << \
					SYSCFG_PINMUX11_PINMUX11_31_28_SHIFT) | \
					(SYSCFG_PINMUX11_PINMUX11_27_24_EMA_A9 << \
					SYSCFG_PINMUX11_PINMUX11_27_24_SHIFT) | \
					(SYSCFG_PINMUX11_PINMUX11_23_20_EMA_A10 << \
					SYSCFG_PINMUX11_PINMUX11_23_20_SHIFT) | \
					(SYSCFG_PINMUX11_PINMUX11_19_16_EMA_A11 << \
					SYSCFG_PINMUX11_PINMUX11_19_16_SHIFT) | \
					(SYSCFG_PINMUX11_PINMUX11_15_12_EMA_A12 << \
					SYSCFG_PINMUX11_PINMUX11_15_12_SHIFT) | \
					(SYSCFG_PINMUX11_PINMUX11_11_8_EMA_A13 << \
					SYSCFG_PINMUX11_PINMUX11_11_8_SHIFT) | \
					(SYSCFG_PINMUX11_PINMUX11_7_4_EMA_A14 << \
					SYSCFG_PINMUX11_PINMUX11_7_4_SHIFT) | \
					(SYSCFG_PINMUX11_PINMUX11_3_0_EMA_A15 << \
					SYSCFG_PINMUX11_PINMUX11_3_0_SHIFT)
	
#define PINMUX9_EMIF_D_ENABLE      (SYSCFG_PINMUX9_PINMUX9_3_0_EMA_D7 << \
					SYSCFG_PINMUX9_PINMUX9_3_0_SHIFT) | \
					(SYSCFG_PINMUX9_PINMUX9_7_4_EMA_D6 << \
					SYSCFG_PINMUX9_PINMUX9_7_4_SHIFT) | \
					(SYSCFG_PINMUX9_PINMUX9_11_8_EMA_D5 << \
					SYSCFG_PINMUX9_PINMUX9_11_8_SHIFT) | \
					(SYSCFG_PINMUX9_PINMUX9_15_12_EMA_D4 << \
					SYSCFG_PINMUX9_PINMUX9_15_12_SHIFT) | \
					(SYSCFG_PINMUX9_PINMUX9_19_16_EMA_D3 << \
					SYSCFG_PINMUX9_PINMUX9_19_16_SHIFT) | \
					(SYSCFG_PINMUX9_PINMUX9_23_20_EMA_D2 << \
					SYSCFG_PINMUX9_PINMUX9_23_20_SHIFT) | \
					(SYSCFG_PINMUX9_PINMUX9_27_24_EMA_D1 << \
					SYSCFG_PINMUX9_PINMUX9_27_24_SHIFT) | \
					(SYSCFG_PINMUX9_PINMUX9_31_28_EMA_D0 << \
					SYSCFG_PINMUX9_PINMUX9_31_28_SHIFT)
	 
#define PINMUX8_EMIF_D_ENABLE      (SYSCFG_PINMUX8_PINMUX8_3_0_EMA_D15 << \
					SYSCFG_PINMUX8_PINMUX8_3_0_SHIFT) | \
					(SYSCFG_PINMUX8_PINMUX8_7_4_EMA_D14 << \
					SYSCFG_PINMUX8_PINMUX8_7_4_SHIFT) | \
					(SYSCFG_PINMUX8_PINMUX8_11_8_EMA_D13 << \
					SYSCFG_PINMUX8_PINMUX8_11_8_SHIFT) | \
					(SYSCFG_PINMUX8_PINMUX8_15_12_EMA_D12 << \
					SYSCFG_PINMUX8_PINMUX8_15_12_SHIFT) | \
					(SYSCFG_PINMUX8_PINMUX8_19_16_EMA_D11 << \
					SYSCFG_PINMUX8_PINMUX8_19_16_SHIFT) | \
					(SYSCFG_PINMUX8_PINMUX8_23_20_EMA_D10 << \
					SYSCFG_PINMUX8_PINMUX8_23_20_SHIFT) | \
					(SYSCFG_PINMUX8_PINMUX8_27_24_EMA_D9 << \
					SYSCFG_PINMUX8_PINMUX8_27_24_SHIFT) | \
					(SYSCFG_PINMUX8_PINMUX8_31_28_EMA_D8 << \
					SYSCFG_PINMUX8_PINMUX8_31_28_SHIFT)
	/*EMIFA_BA0 EMIFA_BA1*/ 
#define PINMUX5_EMIF_BA_ENABLE     (SYSCFG_PINMUX5_PINMUX5_31_28_EMA_BA0 << \
					SYSCFG_PINMUX5_PINMUX5_31_28_SHIFT) | \
					(SYSCFG_PINMUX5_PINMUX5_27_24_EMA_BA1 << \
					SYSCFG_PINMUX5_PINMUX5_27_24_SHIFT)
	
	
	/*EMIFA_CSn2  EMIFA_WEn  EMIFA_OEn*/
#define PINMUX7_EMIF_CTL_ENABLE    (SYSCFG_PINMUX7_PINMUX7_3_0_NEMA_CS2 << \
					SYSCFG_PINMUX7_PINMUX7_3_0_SHIFT) | \
					(SYSCFG_PINMUX7_PINMUX7_23_20_NEMA_OE << \
					SYSCFG_PINMUX7_PINMUX7_23_20_SHIFT) | \
					(SYSCFG_PINMUX7_PINMUX7_19_16_NEMA_WE << \
					SYSCFG_PINMUX7_PINMUX7_19_16_SHIFT)

struct _ad7606 {
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
};

/*****************************************************************************
**                        FUNCTION DEFINITIONS
*****************************************************************************/
/**
  * \brief	 This function selects the EMIFA pins for use. The EMIFA pins
  *		 are multiplexed with pins of other peripherals in the
  *		 System on Chip(SoC).
  *
  * \param	 None.
  *
  * \return  None.
  */
static void EMIFAPinMuxSetup() {
	 volatile unsigned int savePinMux = 0;

	 /* Programming the PINMUX12 register. */
	 /*
	 ** Clearing the pins in context and retaining the other pin values
	 ** of PINMUX12 register.
	 */
	 savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(12)) & \
				 ~(SYSCFG_PINMUX12_PINMUX12_31_28 | \
				   SYSCFG_PINMUX12_PINMUX12_27_24 | \
				   SYSCFG_PINMUX12_PINMUX12_23_20 | \
				   SYSCFG_PINMUX12_PINMUX12_19_16 | \
				   SYSCFG_PINMUX12_PINMUX12_15_12 | \
				   SYSCFG_PINMUX12_PINMUX12_11_8  | \
				   SYSCFG_PINMUX12_PINMUX12_7_4   | \
				   SYSCFG_PINMUX12_PINMUX12_3_0);
	/* 
	** Performing the actual Pin Multiplexing to select relevant pins in
	** PINMUX12  register.
	*/
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(12)) = \
		 (PINMUX12_EMIF_A_ENABLE | savePinMux);

	  

	/* Programming the PINMUX11 register. */
	/*
	** Clearing the pins in context and retaining the other pin values
	** of PINMUX11 register.
	*/
	savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) & \
		 ~(SYSCFG_PINMUX11_PINMUX11_31_28 | \
		   SYSCFG_PINMUX11_PINMUX11_27_24 | \
		   SYSCFG_PINMUX11_PINMUX11_23_20 | \
		   SYSCFG_PINMUX11_PINMUX11_19_16 | \
		   SYSCFG_PINMUX11_PINMUX11_15_12 | \
		   SYSCFG_PINMUX11_PINMUX11_11_8  | \
		   SYSCFG_PINMUX11_PINMUX11_7_4   | \
		   SYSCFG_PINMUX11_PINMUX11_3_0);
	/* 
	** Performing the actual Pin Multiplexing to select relevant pins in
	** PINMUX11  register.
	*/
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) = \
		 (PINMUX11_EMIF_A_ENABLE | savePinMux);



	 /* Programming the PINMUX9 register. */
	 /*
	 ** Clearing the pins in context and retaining the other pin values
	 ** of PINMUX9 register.
	 */
	 savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(9)) & \
			 ~(SYSCFG_PINMUX9_PINMUX9_31_28 | \
			   SYSCFG_PINMUX9_PINMUX9_27_24 | \
			   SYSCFG_PINMUX9_PINMUX9_23_20 | \
			   SYSCFG_PINMUX9_PINMUX9_19_16 | \
			   SYSCFG_PINMUX9_PINMUX9_15_12 | \
			   SYSCFG_PINMUX9_PINMUX9_11_8	| \
			   SYSCFG_PINMUX9_PINMUX9_7_4	| \
			   SYSCFG_PINMUX9_PINMUX9_3_0);
	/* 
	** Performing the actual Pin Multiplexing to select relevant pins in
	** PINMUX9  register.
	*/
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(9)) = \
		(PINMUX9_EMIF_D_ENABLE | savePinMux);


	 /* Programming the PINMUX8 register. */
	 /*
	 ** Clearing the pins in context and retaining the other pin values
	 ** of PINMUX8 register.
	 */
	 savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(8)) & \
			 ~(SYSCFG_PINMUX8_PINMUX8_31_28 | \
			   SYSCFG_PINMUX8_PINMUX8_27_24 | \
			   SYSCFG_PINMUX8_PINMUX8_23_20 | \
			   SYSCFG_PINMUX8_PINMUX8_19_16 | \
			   SYSCFG_PINMUX8_PINMUX8_15_12 | \
			   SYSCFG_PINMUX8_PINMUX8_11_8	| \
			   SYSCFG_PINMUX8_PINMUX8_7_4	| \
			   SYSCFG_PINMUX8_PINMUX8_3_0);
	/* 
	** Performing the actual Pin Multiplexing to select relevant pins in
	** PINMUX8  register.
	*/
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(8)) = \
		(PINMUX8_EMIF_D_ENABLE | savePinMux);


	 /* Programming the PINMUX5 register. */
	 /*
	 ** Clearing the pins in context and retaining the other pin values
	 ** of PINMUX5 register.
	 */
	 savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) & \
			 ~(SYSCFG_PINMUX5_PINMUX5_31_28 | \
			   SYSCFG_PINMUX5_PINMUX5_27_24);
	/* 
	** Performing the actual Pin Multiplexing to select relevant pins in
	** PINMUX5  register.
	*/
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(5)) = \
		(PINMUX5_EMIF_BA_ENABLE | savePinMux);


	 /* Programming the PINMUX7 register. */
	 /*
	 ** Clearing the pins in context and retaining the other pin values
	 ** of PINMUX7 register.
	 */
	 savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(7)) & \
			 ~(SYSCFG_PINMUX7_PINMUX7_23_20 | \
			   SYSCFG_PINMUX7_PINMUX7_19_16 | \
			   SYSCFG_PINMUX7_PINMUX7_3_0);
	/* 
	** Performing the actual Pin Multiplexing to select relevant pins in
	** PINMUX7  register.
	*/
	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(7)) = \
		(PINMUX7_EMIF_CTL_ENABLE | savePinMux);
}

static void ad7606_emifa_init(ad7606 *ad) {
	/* Power on the EMIFA */
	PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_EMIFA, PSC_POWERDOMAIN_ALWAYS_ON,
			PSC_MDCTL_NEXT_ENABLE);

	/*selects the EMIFA pins for use*/
	EMIFAPinMuxSetup();

	/*set the buswidth of async device connected.  16bit*/
	EMIFAAsyncDevDataBusWidthSelect(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2,
					EMIFA_DATA_BUSWITTH_16BIT);

	/*selects the aync interface opmode. :Normal Mode*/
	EMIFAAsyncDevOpModeSelect(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2,
	EMIFA_ASYNC_INTERFACE_NORMAL_MODE);

	/*Extended Wait disable.*/
	EMIFAExtendedWaitConfig(SOC_EMIFA_0_REGS,EMIFA_CHIP_SELECT_2,
	EMIFA_EXTENDED_WAIT_DISABLE);

	/*configures the wait timing for the device interfaced on CS2
	* W_SETUP/W_HOLD W_STROBE/R_STROBE W_HOLD/R_HOLD TA*/
	EMIFAWaitTimingConfig(SOC_EMIFA_0_REGS, EMIFA_CHIP_SELECT_2,
                          EMIFA_ASYNC_WAITTIME_CONFIG(1, 2, 1, 1, 3, 1, 1));
}

static void ad7606_gpio_pinmux_setup(ad7606 *ad) {
	volatile unsigned int savePinMux = 0;

	savePinMux = HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) &
		~(SYSCFG_PINMUX11_PINMUX11_19_16 | SYSCFG_PINMUX11_PINMUX11_11_8);

	HWREG(SOC_SYSCFG_0_REGS + SYSCFG0_PINMUX(11)) =
		(PINMUX11_BUSY_ENABLE | PINMUX11_CONVST_ENABLE | savePinMux);
}

static void ad7606_gpio_setup(ad7606 *ad) {
	/*Set the AD7606_PAR_BUSY to the input mode*/
	GPIODirModeSet(SOC_GPIO_0_REGS, AD7606_PAR_BUSY, GPIO_DIR_INPUT);

	/*Set the AD7606_PAR_BUSY to the Falling Edge Interrupt*/
	GPIOIntTypeSet(SOC_GPIO_0_REGS, AD7606_PAR_BUSY, GPIO_INT_TYPE_FALLEDGE);

	/*The interrupt setting allows AD7606_PAR_BUSY_GPIO_BANK*/
	GPIOBankIntEnable(SOC_GPIO_0_REGS, AD7606_PAR_BUSY_GPIO_BANK);

	/*Set the AD7606_PAR_CONVST to output mode*/
	GPIODirModeSet(SOC_GPIO_0_REGS, AD7606_PAR_CONVST, GPIO_DIR_OUTPUT);
	GPIOPinWrite(SOC_GPIO_0_REGS, AD7606_PAR_CONVST, GPIO_PIN_LOW);
}

/*GPIO interrupt service function*/
static void ad7606_busy_hwi(UArg ad_addr) {
    ad7606 *ad = (ad7606 *)ad_addr;

    if (ad->counter == 0) {
        ad->data = (float *)ring_buffer_acquired(ad->buffer, ad->trigger * sizeof(float));
        if (ad->data == NULL) {
            LOG_WARN("failed to acquired buffer");
            Task_sleep(1000 * 1000);
        }
    }

	/* Disable the interrupts for pins of bank AD7606_PAR_BUSY in GPIO.*/
	GPIOBankIntDisable(SOC_GPIO_0_REGS, AD7606_PAR_BUSY_GPIO_BANK);

	/* Clears the Interrupt Status of AD7606_PAR_BUSY in GPIO.*/
	GPIOPinIntClear(SOC_GPIO_0_REGS, AD7606_PAR_BUSY);

	/* Set low convst gpio to prepare the next conversion  */
	GPIOPinWrite(SOC_GPIO_0_REGS, AD7606_PAR_CONVST, GPIO_PIN_LOW);

    if (ad->data != NULL) {
        int values[8];
        int i;
        for (i = 0; i < 8; i++)
            values[i] = ((Int16 *)SOC_EMIFA_CS2_ADDR)[0x00]; 
        
        int16_t raw = values[ad->channel];
        ad->data[ad->counter++] = (float)raw * ad->range / 32768;
    }

    if (ad->counter == ad->trigger) { /* trigger 事件*/
        ad->counter = 0;
        ring_buffer_release(ad->buffer, ad->trigger * sizeof(float));
        if (ad->callback)  /* 回调 */
            ad->callback(ad->data, ad->trigger, ad->user_data);
    }

	/* Enable interrupts for pins of bank AD7606_PAR_BUSY.*/
	GPIOBankIntEnable(SOC_GPIO_0_REGS, AD7606_PAR_BUSY_GPIO_BANK);
}

static void ad7606_irq_setup(ad7606 *ad) {
	Hwi_Params hwiParams;
	Hwi_Handle myHwi;
	Error_Block eb;

	Error_init(&eb);
	Hwi_Params_init(&hwiParams);
	hwiParams.arg = (unsigned int)ad;
	hwiParams.enableInt = FALSE;
	hwiParams.eventId = ad->event_id;
	hwiParams.priority = 4;
	myHwi = Hwi_create(ad->irq_event, ad7606_busy_hwi, &hwiParams, &eb);
	if (myHwi == NULL) 
        LOG_FATAL("failed to create hwi");

	/* enable both interrupts */
	Hwi_enableInterrupt(ad->irq_event);
}

ad7606 * ad7606_new(unsigned int range, RingBuffer *buffer) {
    ad7606 *ad = (ad7606 *)malloc(sizeof(ad7606));
    memset(ad, 0, sizeof(ad7606));

    ad->irq_event = AD7606_IRQ_EVENT;
    ad->event_id = AD7606_PAR_BUSY_GPIO_BANK_INT;

    ad->range = range;
    ad->buffer = buffer;
    ad->counter = 0;
    ad->channel = 0;

	/* The Local PSC number for GPIO is 3. GPIO belongs to PSC1 module.*/
	PSCModuleControl(SOC_PSC_1_REGS, 
                     HW_PSC_GPIO, 
                     PSC_POWERDOMAIN_ALWAYS_ON, 
                     PSC_MDCTL_NEXT_ENABLE);

	ad7606_emifa_init(ad);

	ad7606_gpio_pinmux_setup(ad);

	ad7606_gpio_setup(ad);

	ad7606_irq_setup(ad);

    return ad;
}

unsigned int ad7606_range(ad7606 *ad) {
    ASSERT(ad);
    return ad->range;
}

void ad7606_destroy(ad7606 *ad) {
    if (ad)
        free(ad);
}

static void ad7606_on_ticked(UArg ad_ptr) {
	/*ad7606_start*/
	GPIOPinWrite(SOC_GPIO_0_REGS, AD7606_PAR_CONVST, GPIO_PIN_HIGH);
}

void ad7606_start(ad7606 *ad, unsigned int rate) {
    ASSERT(ad); 
    if (rate > 1000000 / TICK_PERIOD) {
        LOG_WARN("unstatisfied sample rate, set as max %d", 1000000 / TICK_PERIOD);
        rate = 1000000 / TICK_PERIOD;
    }

    /* 计算时钟周期 */
    int period = 1000000 / TICK_PERIOD / rate;

    if (! ad->is_running) {   /* 第一次启动 */
        Error_Block eb;
        Error_init(&eb);

        Clock_Params params;
        Clock_Params_init(&params);
        params.period = period;
        params.arg = (unsigned int)ad;
        params.startFlag = TRUE;
        ad->clock = Clock_create(ad7606_on_ticked, period, &params, &eb);
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

void ad7606_stop(ad7606 *ad) {
    ASSERT(ad); 
    ASSERT(ad->is_running);
    Clock_stop(ad->clock);
    ad->is_running = false;
}

void ad7606_trigger(ad7606 *ad, unsigned int count, TRIGGER_CALLBACK cb, void *user_data) {
    ASSERT(ad); 
    ASSERT(cb);
    ad->trigger = count;
    ad->callback = cb;
    ad->user_data = user_data;

    LOG_DEBUG("set trigger as %d", ad->trigger);
}

void ad7606_set_channel(ad7606 *ad, unsigned int ch) {
    ASSERT(ad);
    ASSERT((int)ch > -1 && (int)ch < 8);
    ad->channel = ch;
}
