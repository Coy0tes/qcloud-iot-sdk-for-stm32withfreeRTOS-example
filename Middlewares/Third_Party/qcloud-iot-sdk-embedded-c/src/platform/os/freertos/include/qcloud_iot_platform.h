#ifndef TC_IOT_PLATFORM_H
#define TC_IOT_PLATFORM_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define tc_iot_hal_malloc malloc
//#define tc_iot_hal_free free
//#define tc_iot_hal_printf printf
#define HAL_Snprintf snprintf


/**
 * �����ض�ƽ̨�µ�һ����ʱ���ṹ��,
 */
typedef int 			intptr_t;
typedef unsigned int	uintptr_t;


struct qTimer {
    uintptr_t end_time;
};

typedef struct qTimer	Timer;








#endif /* end of include guard */
