
#include "app.h"
#include "cmsis_os.h"
/*start adv*/

#if 0 //NO_PRINTF
#define printf(...)
#endif

#ifdef CANNON_V2
char name[] = "BalanceCar";
#endif
extern float speed_A;                           //�ٶȺ�
extern int32_t speed_L;                         //��ߵ���ٶ�
extern int32_t speed_R;                         //�ұߵ���ٶ�
extern imu_sensor_raw_data_t sensor_saw_data;   //IMU�ʹ�����ԭʼֵ
extern imu_sensor_data_t sensor_data;           //У׼ת�����ֵ��Offset��MyOffset����
extern imu_euler_data_t sensor_euler_angle;     //ŷ����
extern float r_pitch;                           //pitch�ķ�תֵ
extern float Speed_Kp,Speed_Ki;
extern float Angle_Kp,Angle_Kd;
extern float Turn_Kp;                           //ת�����P
extern float Car_Angle_Center;                  //ƽ���Ƕ�
extern int8_t remote_control_X,remote_control_Y;
extern int16_t motor_output_Speed;
extern int16_t motor_output_temp;
extern int16_t motor_output_Angle;

extern float  speed_target;
extern float  turn_target_speed;
extern float  turn_target_orientaion;
extern int8_t trun_mode;

static xQueueHandle  TXQueue;                   //�����������͵��ж�
static osMutexId osMutexSPI;                    //����SPI�������뺯���Ļ�����
static const float adc2vol_coeff = 3.998f;      //adcֵת��Ϊ��ص�ѹmV��ϵ��
static volatile int running = 0;

typedef struct 
{
    uint8_t type;
    uint8_t length;
    uint8_t value[18];
}BLEMessage;

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    (void)(xTask);
    (void)(pcTaskName);
    for(;;) {
        BSP_LED_Toggle(LED0);
    }
}

/* 
 * ������ʪ�ȴ�����
 * ������imu�жϵ�ͬʱ���� ���������л�����i2c����
 */
volatile int16_t temperature, humidity;
void UpdateMySensor(void)
{
    JSensor_HUM_TEMP_Typedef tdef;
    tdef.humidity    = (int16_t *)&humidity;
    tdef.temperature = (int16_t *)&temperature;
    jsensor_app_read_sensor(JSENSOR_TYPE_HUMITY_TEMP, (void *)&tdef);
}

//���������LEDָʾ��
void My_LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable the GPIO_LED Clock */
    __GPIOB_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

extern int CarRunning;
static void ToggleLEDsThread(void const *argument)
{
    uint32_t adc_value;
    uint8_t  quantity;  //�����ٷ���
    static int count;
    static int stopcount;
    
    for(;;)
    {
        //�ȴ��������ȶ�
        if(!stopcount){
            if(count == 15){
                CarRunning = 1;
                stopcount = 1;
            }else {
                count++;
            }
        }
        
        BSP_LED_Toggle(LED0);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
        
        if(running) {
            BLEMessage RXMessage;
            //��ȡ��ʪ��
            RXMessage.type   = 0x00;
            RXMessage.length = 0x02;
            memcpy(RXMessage.value, (uint8_t*)&temperature, RXMessage.length);
            xQueueSend( TXQueue, ( void* )&RXMessage, 0 );
            
            RXMessage.type   = 0x01;
            RXMessage.length = 0x02;
            memcpy(RXMessage.value, (uint8_t*)&humidity, RXMessage.length);
            xQueueSend( TXQueue, ( void* )&RXMessage, 0 );
            
            //��ȡADC
            Get_Adc(&adc_value);
            float vol = adc_value * adc2vol_coeff / 1000;  //����ADCֵ�����ص�ѹ
            //3.7~4.2
            //�������4.2V �޸���3.7V��û����
            //OCV��ѹ��������ϵ 550mA�����ŵ��ѹ��������ϵ
            //100%----4.20V 100%----4.20V 
            //90%-----4.06V 90%-----3.97V 
            //80%-----3.98V 80%-----3.87V 
            //70%-----3.92V 70%-----3.79V 
            //60%-----3.87V 60%-----3.73V 
            //50%-----3.82V 50%-----3.68V 
            //40%-----3.79V 40%-----3.65V 
            //30%-----3.77V 30%-----3.62V 
            //20%-----3.74V 20%-----3.58V 
            //10%-----3.68V 10%-----3.51V 
            //5%------3.45V 5%------3.42V 
            //0%------3.00V 0%------3.00V 
            const float max_vol = 4.20 * 3;
            const float min_vol = 3.60 * 3;
            float q = (vol - min_vol)/(max_vol - min_vol) * 100;
            if(q>100) {
                quantity = 100;
            }
            else if(q<0) {
                quantity = 0;
            }
            else {
                quantity = (uint8_t)q;
            }
            
            RXMessage.type   = 0x02;
            RXMessage.length = 0x01;
            memcpy(RXMessage.value, (uint8_t*)&quantity, RXMessage.length);
            xQueueSend( TXQueue, ( void* )&RXMessage, 0 );
            osDelay(300);
        }
        else {
            osDelay(1000);
        }
    }
}

static void BLEMessageQueueConsumer (const void *argument)
{
    BLEMessage pxMessage;
    for(;;)
    {
        if( xQueueReceive( TXQueue,  ( void* )&pxMessage, osWaitForever ) )
        {
            if(osMutexWait(osMutexSPI, osWaitForever) == osOK && running) {
                ble_device_send(pxMessage.type,pxMessage.length,pxMessage.value);
            }
            osMutexRelease(osMutexSPI);
        }
        osDelay(50);    //��ֹ��������̫�������
    }
}

static void BLEThread(void const *argument)
{
    for(;;) {
        if(osMutexWait(osMutexSPI, osWaitForever) == osOK) {
            HCI_Process();
        }
        osMutexRelease(osMutexSPI);
    }
}

static void mainThread(const void *argument){
    const TickType_t xFrequency = 5;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        OutData[0] = sensor_euler_angle.pitch*100;
        OutData[1] = sensor_saw_data.gyro[0];
        OutData[2] = sensor_saw_data.gyro[1];
        OutData[3] = sensor_saw_data.gyro[2];
        
        //OutData[0] = sensor_saw_data.acc[0];
        //OutData[1] = sensor_saw_data.acc[1];
        //OutData[2] = sensor_saw_data.gyro[0];
        //OutData[3] = sensor_saw_data.gyro[0];

        OutPut_Data();
    }
}

//��ʼ��������
void jsensor_app_set_sensors(void)
{
    jsensor_app_set_sensor(JSENSOR_TYPE_HUMITY_TEMP);
    //jsensor_app_set_sensor(JSENSOR_TYPE_PRESSURE);
    //jsensor_app_set_sensor(JSENSOR_TYPE_MOTION_6AXIS);
    //jsensor_app_set_sensor(JSENSOR_TYPE_MAGNET);
}

void on_ready(void)
{
    My_LED_Init();
    jsensor_app_set_sensors();
    
    uint8_t tx_power_level = 7;
    uint16_t adv_interval = 100;
    uint8_t bdAddr[6];
    uint32_t data_rate = 400;

    HCI_get_bdAddr(bdAddr);
    //adv_name_generate(bdAddr+4);

    ble_set_adv_param(name, bdAddr, tx_power_level, adv_interval);
    ble_device_start_advertising();

    imu_sensor_select_features(ALL_ENABLE);

    imu_sensor_reset();

    imu_sensor_set_data_rate(&data_rate, LSM6DS3_XG_FIFO_MODE_CONTINUOUS_OVERWRITE);

    //imu_sensor_filter();

    imu_sensor_start();

    //HAL_Delay(100);
    Motor_Pwm_Init();
    Encoder_Init();
    //Steer_Pwm_Init();
    Adc_Init();
    //SD_Init();

    TXQueue=xQueueCreate( 5 , 20 );
    
    osMutexDef(osMutexSPI);        //����SPI�Ƿǿ����뺯����������Ҫʹ�û����ź�
    osMutexSPI = osMutexCreate(osMutex(osMutexSPI));
    
    osThreadDef(BLE, BLEThread, osPriorityIdle, 0, 3*configMINIMAL_STACK_SIZE);                         //�������գ���͵����ȼ�
    osThreadCreate(osThread(BLE), NULL);

    osThreadDef(QCons, BLEMessageQueueConsumer, osPriorityAboveNormal, 0, 3*configMINIMAL_STACK_SIZE);  //�������ͣ�����ж�
    osThreadCreate(osThread(QCons), NULL);

    osThreadDef(uLEDThread, ToggleLEDsThread, osPriorityNormal, 0, 3*configMINIMAL_STACK_SIZE);         //ָʾ�ƺ�������Ϣ����
    osThreadCreate(osThread(uLEDThread), NULL);

    osThreadDef(mainThread, mainThread, osPriorityRealtime, 0, 3*configMINIMAL_STACK_SIZE);             //����ʱ��������ȼ�
    osThreadCreate(osThread(mainThread), NULL);

    osKernelStart();
}

extern  float invSqrt(float x) ;
/* Device On Message */

/*
У��sum
length:��ҪУ���ֽڵĳ���
* value����ҪУ��������ָ��
return:0�ɹ���1����
*/
uint8_t Checksum(uint8_t length, uint8_t* value){
    uint8_t check=0;
    for(int i=0;i<length;i++) check+=*(value+1+i);
    if(check!=*(value+1+length))return 0;
    else return 1;
}

//mytools
#include <math.h>
#define M_PI 3.14159265358979323846f
typedef struct {
    float x;
    float y;
} Direction;

Direction Angle_to_Direction(float angle)
{
    Direction dir;
    dir.x = (float)cos(angle/180*M_PI);
    dir.y = (float)sin(angle/180*M_PI);
    return dir;
}

extern float Car_Angle_Center;  //ƽ���Ƕ�
void ble_device_on_message(uint8_t type, uint16_t length, uint8_t* value)
{
    if(type == 0) {
        //ҡ��
        value[length] = '\0';
        Direction dir = Angle_to_Direction(atoi((const char *)value));
        
        speed_target      = -dir.y*50;      //����ǰ���ٶ�
        turn_target_speed = -dir.x*20;      //����ת���ٶ�
        if(dir.y < 0 ) {
            turn_target_speed = -turn_target_speed;
        }
        turn_target_orientaion = 0;         //���÷���
    }
    else if(type == 1) {
        //΢��ƽ��Ƕ�
        
    }
    else if(type == 2) {
        //�ָ�ƽ��״̬
        speed_target = 0;
        turn_target_speed = 0;
    }
    else if(type == 3) {
        //ֹͣ
        CarRunning = 0;
    }
}

/* Device on connect */
void ble_device_on_connect(void)
{
    running = 1;
    Ble_conn_state = BLE_NOCONNECTABLE;
}

/* Device on disconnect */
void ble_device_on_disconnect(uint8_t reason)
{
    running = 0;
    /* Make the device connectable again. */
    Ble_conn_state = BLE_CONNECTABLE;
    ble_device_start_advertising();
}

