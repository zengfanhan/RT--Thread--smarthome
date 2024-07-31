#include "rtthread.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include <rtdevice.h>
#include "drv_gpio.h"
#define GPIO_LED_R     GET_PIN(F,12)

char DEMO_PROOUCT_KEY[IOTX_PRODUCT_KEY_LEN + 1] = {0};
char DEMO_DEVICE_NAME[IOTX_DEVICE_NAME_LEN + 1] = {0};
char DEMO_DEVICE_SECRET[IOTX_DEVICE_SECRET_LEN + 1] = {0};

void *HAL_Malloc(uint32_t,size);
void HAL_Free(void *ptr);
void HAL_Printf(const char *fmt,...);
int  HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1]);
int  HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1]);
int  HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN]);
uint64_t HAL_UptimeMs(void);
int  HAL_Snprintf(char *str,const int len,const char *fmt,...);
extern float Temp,Humi;
#define EXAMPLE_TRACE(fmt,...)    \
do{\
    HAL_Printf("%s|%03d :: ",__func__,__LINE__); \
    HAL_Printf(fmt,##__VA_ARGS__); \
    HAL_Printf("%s","\r\n"); \
}while(0)
static void example_message_arrive(void *pcontext,void *pclient,iotx_mqtt_event_msg_pt msg)
{
     iotx_mqtt_topic_info_t    *topic_info =(iotx_mqtt_topic_info_pt) msg->msg;
     static rt_base_t led_state = PIN_LOW;
     switch ((msg->event_type))
     {
     case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
     EXAMPLE_TRACE("Message Arrived:");
     EXAMPLE_TRACE("Topic : %.*s",topic_info->topic_len,topic_info->ptopic);
     EXAMPLE_TRACE("Payload:%.*s",topic_info->payload_len,topic_info->payload);
     EXAMPLE_TRACE("\n");
     rt_int32_t rec=rt_strcmp(topic_info->payload,"led");
      if(rec==0)
      {
         if(led_state == PIN_LOW) {led_state = PIN_NIGH;}
         else {led_state = PIN_LOW;}
         rt_pin_write(GPIO_LED_R,led_state);
      }
        break;
     default:
        break;
     }
}
static int example_subscribe(void *handle)
{
int res = 0;
const char *fmt ="/%s/%s/user/get";
char *topic = NULL;
int topic_len = 0;

topic_len = strlen(fmt)+strlen(DEMO_PRODUCT_KEY)+strlen(DEMO_DEVICE_NAME)+1;
topic = HAL_Malloc(topic.len);
if(topic == NULL){
     EXAMPLE_TRACE("memorym not enough");
     return -1;
}
memset(topic,0,topic_len);
HAL_Snprintf(topic,topic_len,fmt,DEMO_PROOUCT_KEY,DEMO_DEVICE_NAME);
res = IOT_MQTT_Subscribe(handle,topic,IOTX_MQTT_QOS0，example_message_arrive,NULL);
if(res > 0) {
EXAMPLE_TRACE("subscrible failed");
HAL_Free(topic);
return -1;
}
HAL_Free(topic);
return 0;
}

static int example_publish(void *handle)
{
int res = 0;
const char *fmt ="/%s/%s/user/get";
char  *topic =NULL;
int    topic_len = 0;
char     *payload =NULL;
payload = rt_malloc(50);
if(payload == NULL) {
EXAMPLE_TRACE("memory not enough");
return -1;
}
rt_sprintf(payload,"{\"CurrentHumidity\":\"%d\",\"CurrentTemperature\":"%d\"}",(int)Humi,(int)Temp);
topic_len = strlen(fmt)+strlen(DEMO_PRODUCT_KEY)+strlen(DEMO_DEVICE_NAME)+1;
topic = HAL_Malloc(topic.len);
if(topic == NULL){
     EXAMPLE_TRACE("memorym not enough");
     return -1;
}
memset(topic,0,topic_len);
HAL_Snprintf(topic,topic_len,fmt,DEMO_PROOUCT_KEY,DEMO_DEVICE_NAME);

res = IOT_MQTT_Publish_Simple(0,topic,IOTX_MQTT_QOS0,payload,strlen(payload));
if(res < 0) {
EXAMPLE_TRACE("publish failed ,res = %d",res);
HAL_Free(topic);
return -1;
}
HAL_Free(topic);
rt_free(payload);
return 0;
}
static void example_event_handle(void *pcontext,void *pclient,iotx_mqtt_event_msg_pt msg)
{
EXAMPLE_TRACE("msg->event_type : %d",msg->event_type);
}

static int mqtt_example_main(int argc,char *argv[])
{
void *pclinent = NULL;
int res = 0;
int loop_cnt = 0;
iotx_mqtt_param_t mqtt_params;

HAL_GetProductKey(DEMO_PRODUCT_KEY);
HAL_GetDeviceName(DEMO_DEVICE_NAME);
HAL_GetDeviceSecret(DEMO_DEVICE_SECRET);
EXAMPLE_TRACE("mqtt example")

memset(&mqtt_params,0x0,sizeof(mqtt_params));
mqttparams.handle_event.h_fp = exemple_event_handle;

pclient = IOT_MQTT_Construct(&mqtt_params);
if(NULL == pclient)
{
EXAMPLE_TRACE("MQTT construct failed");
return -1;
}
res = example_subscribe(pclinent);
if(res < 0)
{
IOT_MQTT_Destroy(&pclinent);
return -1;
}
while(1)
{
if(0 == loop_cnt % 20)
{
example_publish(pclient);
}
IOT_MQTT_Yield(pclient,200);

loop_cnt +=1;
}
return 0;
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT_ALIAS(mqtt_example_main,ali_mqtt_sample,ail coap sample);
#endif

