#include "tru_delay.h"

uintptr_t tru_corefreq = TRU_TICKFREQ_500MHZ;
uintptr_t tru_tickfreq = TRU_TICKFREQ_500MHZ;

void tru_delay_set_corefreq(uintptr_t freq){
	tru_corefreq = freq;
}

uintptr_t tru_delay_get_corefreq(void){
	return tru_corefreq;
}

void tru_delay_set_tickfreq(uintptr_t freq){
	tru_tickfreq = freq;
}

uintptr_t tru_delay_get_tickfreq(void){
	return tru_tickfreq;
}

__attribute__((weak)) uintptr_t tru_delay_get_tick(void){
	return 0;
}

__attribute__((weak)) void tru_delay_us(uintptr_t us){
#if defined(DEBUG)
	uintptr_t delay = tru_tickfreq / TRU_TICKFREQ_TO_US_DIVISOR * us / 1000;
#else
	uintptr_t delay = tru_tickfreq / TRU_TICKFREQ_TO_US_DIVISOR / TRU_TICKFREQ_TO_US_DIVISOR * us;
#endif

	while(delay--){
		 __asm__ __volatile__("");
	}
}

__attribute__((weak)) void tru_delay_ms(uintptr_t ms){
#if defined(DEBUG)
	uintptr_t delay = tru_tickfreq / TRU_TICKFREQ_TO_MS_DIVISOR * ms / 1000;
#else
	uintptr_t delay = tru_tickfreq / TRU_TICKFREQ_TO_MS_DIVISOR * ms;
#endif

	while(delay--){
		 __asm__ __volatile__("");
	}
}
