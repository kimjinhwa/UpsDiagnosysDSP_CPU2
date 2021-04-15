/*
 * CpuTImer1_2.c
 *
 *  Created on: Apr 8, 2021
 *      Author: STELLA
 */
#include <CpuTimer1_2.h>
#include "driverlib.h"
#include "device.h"
#include "F2837xD_device.h"

void configCPUTimer(uint32_t cpuTimer, float freq, float period);
__interrupt void cpuTimer1ISR(void);
__interrupt void cpuTimer2ISR(void);

uint16_t cpuTimer1IntCount;
uint16_t cpuTimer2IntCount;

void initCPUTimers(void)
{
    //
    // Initialize timer period to maximum
    //

    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //

    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    //
    // Make sure timer is stopped
    //

    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    //
    // Reload all counter register with period value
    //

    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    //
    // Reset interrupt counter
    //

    cpuTimer1IntCount = 0;
    cpuTimer2IntCount = 0;

    Interrupt_register(INT_TIMER1, &cpuTimer1ISR);
    Interrupt_register(INT_TIMER2, &cpuTimer2ISR);

    configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 10000);// 100hz
    configCPUTimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, 10000);// 100hz

    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    CPUTimer_enableInterrupt(CPUTIMER2_BASE);

    Interrupt_enable(INT_TIMER1);
    Interrupt_enable(INT_TIMER2);

    CPUTimer_startTimer(CPUTIMER1_BASE);
    CPUTimer_startTimer(CPUTIMER2_BASE);
}

__interrupt void
cpuTimer1ISR(void)
{
    if(cpuTimer1IntCount)
    cpuTimer1IntCount--;
}

__interrupt void
cpuTimer2ISR(void)
{
    if(cpuTimer2IntCount)
    cpuTimer2IntCount--;
}

void configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    uint32_t temp;

    //
    // Initialize timer period:
    //
    temp = (uint32_t)(freq / 1000000 * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
                              CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);

    //
    // Resets interrupt counters for the three cpuTimers
    //
    if(cpuTimer == CPUTIMER1_BASE)
    {
        cpuTimer1IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER2_BASE)
    {
        cpuTimer2IntCount = 0;
    }
}
