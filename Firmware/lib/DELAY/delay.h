#ifndef DELAY_H
#define DELAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "misc.h"

    void Delay_Init(void);
    void Delayus(uint32_t us);
    void Delayms(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif
