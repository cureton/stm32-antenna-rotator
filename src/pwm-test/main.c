#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

// PA0 TIM5_CH1. AF2


const uint32_t timer_peri = TIM5; // timer peripheral
const enum tim_oc_id oc_id = TIM_OC1; // output compare channel designator

int main(void)
{

    // setup PA0 for PWM
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_TIM5); // enable TIM clock
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO0 | GPIO1 | GPIO2 | GPIO3 ); // pin PA11 Alt Function
    gpio_set_af(GPIOA, GPIO_AF2, GPIO0 | GPIO1 | GPIO2 | GPIO3 );

    int freq = 400;
    timer_set_prescaler(timer_peri, 16-1); // s/b 1MHz
    int period = 1000000/freq; // assumes prescaled set to 1MHz
    timer_enable_preload(timer_peri); // causes counter to be loaded from its ARR only at next update event
    timer_set_period(timer_peri, period); // set the timer period in the (ARR) auto-reload register
    timer_set_oc_value(timer_peri, oc_id, period * 1/4); // set duty cycle to 25%
    timer_set_oc_value(timer_peri, TIM_OC2, period * 1/2); // set duty cycle to 25%
    timer_set_oc_value(timer_peri, TIM_OC3, period * 3/4); // set duty cycle to 25%
    timer_set_oc_value(timer_peri, TIM_OC4, period * 1/2); // set duty cycle to 25%
    timer_set_counter(timer_peri, 0); // TIM_CNT
    
    timer_enable_oc_preload(timer_peri, oc_id);

    timer_set_oc_mode(timer_peri, oc_id, TIM_OCM_PWM1); // output active when counter is lt compare register
    timer_set_oc_mode(timer_peri, TIM_OC2, TIM_OCM_PWM1); // output active when counter is lt compare register
    timer_set_oc_mode(timer_peri, TIM_OC3, TIM_OCM_PWM1); // output active when counter is lt compare register
    timer_set_oc_mode(timer_peri, TIM_OC4, TIM_OCM_PWM1); // output active when counter is lt compare register

    timer_enable_oc_output(timer_peri, oc_id); // enable timer output compare
    timer_enable_oc_output(timer_peri, TIM_OC2); // enable timer output compare
    timer_enable_oc_output(timer_peri, TIM_OC3); // enable timer output compare
    timer_enable_oc_output(timer_peri, TIM_OC4); // enable timer output compare
    timer_continuous_mode(timer_peri); // enable the timer to run continuously
    timer_generate_event(timer_peri, TIM_EGR_UG); // required!
    timer_enable_counter(timer_peri);
    //timer_enable_irq(timer_peri, TIM_DIER_COMIE);  //enable commutation interrupt
    //nvic_enable_irq(NVIC_TIM1_CC_IRQ);






    // For advanced timers, like TIM1, you need to additionally enable the ouput:
    // timer_enable_break_main_output(timer_peri);

    while (1);
}
