#ifndef TRU_DELAY_H
#define TRU_DELAY_H

#include <stdint.h>

// Tick frequency in HZ
typedef enum{
	TRU_TICKFREQ_100HZ  = 100U,
	TRU_TICKFREQ_1KHZ   = 1000U,
	TRU_TICKFREQ_1MHZ   = 1000000UL,
	TRU_TICKFREQ_500MHZ = 500000000UL,
	TRU_TICKFREQ_1GHZ   = 1000000000UL,
}tru_tickfreq_t;

#define TRU_TICKFREQ_TO_US_DIVISOR 1000000UL
#define TRU_TICKFREQ_TO_MS_DIVISOR 1000U

void tru_delay_set_corefreq(uintptr_t freq);
uintptr_t tru_delay_get_corefreq(void);
void tru_delay_set_tickfreq(uintptr_t freq);
uintptr_t tru_delay_get_tickfreq(void);
__attribute__((weak)) void tru_delay_us(uintptr_t us);
__attribute__((weak)) void tru_delay_ms(uintptr_t ms);

#endif
