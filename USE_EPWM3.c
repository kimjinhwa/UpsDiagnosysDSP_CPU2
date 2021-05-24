#include "driverlib.h"
#include "device.h"
#include "F2837xD_device.h"

__interrupt void pwmE3ISR(void)  // note_2
{
    //DMA_forceTrigger(DMA_CH6_BASE);
    EPWM_clearEventTriggerInterruptFlag(EPWM3_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}
void initEPWM3(void){
    EPWM_SignalParams pwmSignal_3 =
            {4000000, 0.5f, 0.5f, true, DEVICE_SYSCLK_FREQ, SYSCTL_EPWMCLK_DIV_2,
             EPWM_COUNTER_MODE_DOWN, EPWM_CLOCK_DIVIDER_1,
            EPWM_HSCLOCK_DIVIDER_1};

    EPWM_configureSignal(EPWM3_BASE, &pwmSignal_3);
    EPWM_setInterruptSource(EPWM3_BASE , EPWM_INT_TBCTR_ZERO );
    EPWM_setInterruptEventCount(EPWM3_BASE, 1U);
    EPWM_enableInterrupt(EPWM3_BASE);

    Interrupt_register(INT_EPWM3, &pwmE3ISR);
    Interrupt_enable(INT_EPWM3);

}
