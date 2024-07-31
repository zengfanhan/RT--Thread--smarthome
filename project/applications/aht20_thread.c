#include <rtthread.h>
#include <aht10.h>

#define DBG_TAG "main"
#define DBG_LVL        DBG_LOG

#define AHT_I2C_BUS_NAME "i2c3"
#define THREAD_PRIORITY     25
#define THREAD_STACK_SIZE   1024
#define THREAD_TIMESLICE    5

float Temp,Humi;
static rt_thread_t tid1 = RT_NULL;
static aht10_device_t AHT;

static void thread_aht20_entry(void *parameter){

while(1){
     Humi = aht10_read_humidity(AHT);
     Temp = aht10_read_temperature(AHT);
     rt_thread_mdelay(500);
}
}

int thread_aht20_sample(void){ 

    AHT = aht10_init(AHT_I2C_BUS_NAME);
    tid1 = rt_thread_create("aht20",
                            thread_aht20_entry,RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY,THREAD_TIMESLICE);
    if(tid1 != RT_NULL) rt_thread_startup(tid1);
    return 0;
}

INIT_APP_EXPORT(thread_aht20_sample);