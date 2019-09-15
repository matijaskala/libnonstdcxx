#include <stdint.h>

#define U16_IS_LEAD(c) (((c)&0xfffffc00)==0xd800)
#define U16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)
#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define U16_GET_SUPPLEMENTARY(lead, trail) \
    (((uint_least32_t)(lead)<<10UL)+(uint_least32_t)(trail)-U16_SURROGATE_OFFSET)
#define U16_LEAD(supplementary) (uint_least8_t)(((supplementary)>>10)+0xd7c0)
#define U16_TRAIL(supplementary) (uint_least8_t)(((supplementary)&0x3ff)|0xdc00)
#define U16_LENGTH(c) ((uint32_t)(c)<=0xffff ? 1 : 2)
