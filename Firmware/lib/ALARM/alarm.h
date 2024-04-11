#ifndef ALARM_H
#define ALARM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx.h"

    void Alarm_Control(int8_t intensity);
    void Auto_Alarm(void);

#ifdef __cplusplus
}
#endif

#endif
