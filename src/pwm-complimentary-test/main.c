#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

// Define PWM frequency and duty cycle parameters

#define PERIOD_TICKS 512 

// Dead time in timer clock ticks (e.g., approx 1us dead time)
// The actual register value calculation is complex and depends on the specific STM32 family.
// Consult the device RM for the dead-time register (BDTR) calculation. 
// A small value is used here for demonstration.
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
    timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_DOWN | TIM_CR1_DIR_UP);

    timer_set_alignment(TIM1, TIM_CR1_CMS_CENTER_1);
    timer_set_prescaler(TIM1, 0); // No prescaler, use full APB2 clock (84 MHz)
    timer_set_period(TIM1, PERIOD_TICKS-1);
    timer_set_repetition_counter(TIM1, 0); // Not using repetition counter in this simple example
    
    /* Output Compare Mode configuration */
    timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);
    timer_set_oc_value(TIM1, TIM_OC1, 127);
    timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM1);
    timer_set_oc_value(TIM1, TIM_OC2, 50);

    /* Enable output compare preload (ensures smooth updates) */
    timer_enable_oc_preload(TIM1, TIM_OC1);
    timer_enable_oc_preload(TIM1, TIM_OC2);

    /* Dead time insertion and complementary output enable */
    // Note: The specific value for dead time depends on clock speed and desired microsecond delay.
    // Refer to the STM32 reference manual for precise dead-time register (BDTR) value calculation.
    timer_set_deadtime(TIM1, DEAD_TIME_VALUE); 
    
    /* Enable the complementary output channel (CH1N) and main output enable (MOE) */
    // For advanced timers, MOE must be set for outputs to appear.
  /* Enable the complementary output channel (CH1N) */
    // Use this function name instead:
//    timer_enable_oc_output_complementary(TIM1, TIM_OC1); // This function (or similar macro) sets CC1NE
    TIM_CCER(TIM1) |= TIM_CCER_CC1NE |  TIM_CCER_CC2NE ;
    timer_enable_break_main_output(TIM1); // This sets the MOE bit in BDTR

    /* Enable the main output channel (CH1) */
    timer_enable_oc_output(TIM1, TIM_OC1); // This sets the CC1E bit
    timer_enable_oc_output(TIM1, TIM_OC2); // This sets the CC1E bit

    /* Finally, enable the timer counter */
    timer_enable_counter(TIM1);
}

int main(void) {
//    rcc_clock_setup_hse(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_84MHZ]); // Example clock setup

    timer_setup();

    while (1) {
        // Main loop can adjust duty cycle if needed
        // e.g., timer_set_oc_value(TIM1, TIM_OC1, new_duty);
    }

    return 0;
}
