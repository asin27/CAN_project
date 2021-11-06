#include <string.h>
#include <stdint.h>

/* lib_adc.c */
void ADC_init (void);
void ADC_start_conversion (void);
char ADC_generate_random(void);

/* IRQ_adc.c */
void ADC_IRQHandler(void);

//unsigned short AD_current;