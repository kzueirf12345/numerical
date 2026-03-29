#ifndef LOGF_SRC_VOGF_VOGF_H
#define LOGF_SRC_VOGF_VOGF_H

#define FLOAT_POS_ZERO      (*(float*)&(uint32_t){0x00000000u})  // +0
#define FLOAT_NEG_ZERO      (*(float*)&(uint32_t){0x80000000u})  // -0

#define FLOAT_POS_INF       (*(float*)&(uint32_t){0x7F800000u})  // +inf
#define FLOAT_NEG_INF       (*(float*)&(uint32_t){0xFF800000u})  // -inf

#define FLOAT_QNAN          (*(float*)&(uint32_t){0x7FC00000u})  // quite nan, first mantissa bit is 1
#define FLOAT_SNAN          (*(float*)&(uint32_t){0x7F800001u})  // signaling nan, first mantissa bit is 0, but mant != 0

float vogf(float x);

#endif /*LOGF_SRC_VOGF_VOGF_H*/