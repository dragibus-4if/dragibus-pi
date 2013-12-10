// Hardware registers and defines for the raspberry pi

#ifndef PI_HARDWARE_H
#define PI_HARDWARE_H

#include <dma.h>

/****** BASES ******/
#define BCM2708_PERI_BASE 0x20000000
#define PWM_BASE          (BCM2708_PERI_BASE + 0x20C000) /* PWM controller */
#define CLOCK_BASE        (BCM2708_PERI_BASE + 0x101000)
#define PM_BASE           (BCM2708_PERI_BASE + 0x100000) /* Power Management, Reset controller and Watchdog registers */


/****** GPIO ******/
#define GPIO_BASE         (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C // Set and clear seem to me like they're the wrong way round, but all the docs have them this way round...
#define GPCLR0  0x20200028
#define GPLEV0 0x20200034
#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098


/******* POWER *******/
#define BCM2835_PWM_CONTROL 0
#define BCM2835_PWM_STATUS  1
#define BCM2835_PWM_DMAC    2
#define BCM2835_PWM0_RANGE  4
#define BCM2835_PWM0_DATA   5
#define BCM2835_PWM_FIFO    6
#define BCM2835_PWM1_RANGE  8
#define BCM2835_PWM1_DATA   9

#define BCM2835_PWMCLK_CNTL   40
#define BCM2835_PWMCLK_DIV    41

#define BCM2835_PWMCLK_CNTL_OSCILLATOR 0x01
#define BCM2835_PWMCLK_CNTL_PLLA 0x04
#define BCM2835_PWMCLK_CNTL_PLLD 0x06
#define BCM2835_PWMCLK_CNTL_KILL 1<<5
#define BCM2835_PWMCLK_CNTL_ENABLE 1<<4

#define PWMCTL_MODE1	(1<<1) // mode (0=pwm, 1=serializer)
#define PWMCTL_PWEN1	(1<<0) // enable ch1
#define PWMCTL_CLRF		(1<<6) // clear fifo
#define PWMCTL_USEF1	(1<<5) // use fifo

#define PWMDMAC_ENAB	(1<<31)
// I think this means it requests as soon as there is one free slot in the FIFO
// which is what we want as burst DMA would mess up our timing..
//#define PWMDMAC_THRSHLD	((15<<8)|(15<<0))
#define PWMDMAC_THRSHLD	((15<<8)|(15<<0))

#define BCM2835_PWM1_MS_MODE    0x8000  /*  Run in MS mode                   */
#define BCM2835_PWM1_USEFIFO    0x2000  /*  Data from FIFO                   */
#define BCM2835_PWM1_REVPOLAR   0x1000  /* Reverse polarity             */
#define BCM2835_PWM1_OFFSTATE   0x0800  /* Ouput Off state             */
#define BCM2835_PWM1_REPEATFF   0x0400  /* Repeat last value if FIFO empty   */
#define BCM2835_PWM1_SERIAL     0x0200  /* Run in serial mode             */
#define BCM2835_PWM1_ENABLE     0x0100  /* Channel Enable             */

#define BCM2835_PWM0_MS_MODE    0x0080  /* Run in MS mode             */
#define BCM2835_PWM0_USEFIFO    0x0020  /* Data from FIFO             */
#define BCM2835_PWM0_REVPOLAR   0x0010  /* Reverse polarity             */
#define BCM2835_PWM0_OFFSTATE   0x0008  /* Ouput Off state             */
#define BCM2835_PWM0_REPEATFF   0x0004  /* Repeat last value if FIFO empty   */
#define BCM2835_PWM0_SERIAL     0x0002  /* Run in serial mode             */
#define BCM2835_PWM0_ENABLE     0x0001  /* Channel Enable             */

#define BCM2835_BERR  0x100
#define BCM2835_GAPO4 0x80
#define BCM2835_GAPO3 0x40
#define BCM2835_GAPO2 0x20
#define BCM2835_GAPO1 0x10
#define BCM2835_RERR1 0x8
#define BCM2835_WERR1 0x4
#define BCM2835_EMPT1 0x2
#define BCM2835_FULL1 0x1

// UARTey stuff
#define AUX_ENABLES     0x20215004
#define AUX_MU_IO_REG   0x20215040
#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_LCR_REG  0x2021504C
#define AUX_MU_MCR_REG  0x20215050
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

#define PM_PASSWORD 0x5A000000 

/****** Watchdog registers *****/
#define PM_RSTC                     (PM_BASE+0x1c)
#define PM_WDOG                     (PM_BASE+0x24)
#define PM_WDOG_TIME_SET            0x000fffff
#define PM_RSTC_WRCFG_CLR           0xffffffcf
#define PM_RSTC_WRCFG_FULL_RESET    0x00000020

/******* TIMER *******/
#define SYSTIMERCLO 0x20003004
#define SYSTIMERCHI 0x20003008

#define CS 0x20003000
#define C0 0x2000300C
#define C1 0x20003010
#define C2 0x20003014
#define C3 0x20003018

/*********** DMA ***********/
#define DMA_CONTROLLER_BASE 0x20007000
#define DMA5_CNTL_BASE DMA_CONTROLLER_BASE + 0x500

#endif
