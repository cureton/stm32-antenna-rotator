#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

// Define PWM frequency and duty cycle parameters

#define PERIOD_TICKS 512 

// Dead time in timer clock ticks (e.g., approx 1us dead time)
// The actual register value calculation is complex and depends on the specific STM32 family.
// Consult the device RM for the dead-time register (BDTR) calculation. 
// This value effectively delays the assertion of both the main and complimentry output providing deadtime for transisters. 

#define DEAD_TIME_VALUE 10 // Example value, adjust as needed

void timer_setup(void) {
    /* Enable clocks for GPIOA and TIM1 */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_TIM1);

    /* Configure PA8 (TIM1_CH1) and PA7 (TIM1_CH1N) as Alternate Function Push-Pull */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);
    gpio_set_af(GPIOA, GPIO_AF1, GPIO8 | GPIO9); // AF1 is for TIM1 on most STM32F4xx
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13 | GPIO14);
    gpio_set_af(GPIOB, GPIO_AF1, GPIO13 | GPIO14); // AF1 is for TIM1 on most STM32F4xx

    /* Timer base configuration */
    /* Run counter on internal clock, Centre aliged PWM mode, Up cound direction (Unused) */  
    timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,  TIM_CR1_DIR_UP);
    
    /* Set prescaler to div(1), use full APB2 clock  */ 
    timer_set_prescaler(TIM1, 0); 

    /* Set repetition period */ 
    timer_set_period(TIM1, PERIOD_TICKS);
 
    /* Dead time insertion and complementary output enable */
    // Note: The specific value for dead time depends on clock speed and desired microsecond delay.
    // Refer to the STM32 reference manual for precise dead-time register (BDTR) value calculation.
    timer_set_deadtime(TIM1, DEAD_TIME_VALUE); 


    /* Enable output compare preload (ensures smooth updates) */
    timer_enable_oc_preload(TIM1, TIM_OC1);
    timer_enable_oc_preload(TIM1, TIM_OC2);

    /* Output Compare Mode configuration */
    /* Output compare mode - Normal PWM  */
    timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);
    timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM1);

    /* Enable PWM outputs 1 and 2 and their compliments */
    timer_enable_oc_output(TIM1, TIM_OC1);
    timer_enable_oc_output(TIM1, TIM_OC1N);
    timer_enable_oc_output(TIM1, TIM_OC2);
    timer_enable_oc_output(TIM1, TIM_OC2N);


    /* Start timers are 50% duty cycle - neutral drive */
    timer_set_oc_value(TIM1, TIM_OC1, PERIOD_TICKS/2);
    timer_set_oc_value(TIM1, TIM_OC2, PERIOD_TICKS/2);

    // For advanced timers, MOE must be set for outputs to appear.
    timer_enable_break_main_output(TIM1); // This sets the MOE bit in BDTR

    /* Enable the main output channel (CH1) */
    timer_enable_oc_output(TIM1, TIM_OC1); // This sets the CC1E bit
    timer_enable_oc_output(TIM1, TIM_OC2); // This sets the CC1E bit

    /* Finally, enable the timer counter */
    timer_enable_counter(TIM1);
}

int main(void) {

    timer_setup();

    while (1) {
        // Main loop can adjust duty cycle if needed
        // e.g., timer_set_oc_value(TIM1, TIM_OC1, new_duty);
    }

    return 0;
}
