//******************************************************************************
// Section include: connect modules header file
//******************************************************************************
#include "resolver.h" // @@@@@@@@ @@@@ @@@@@@@@@ @@@ @@@@@@ @@@@@@
//******************************************************************************
// Local variables definitions for this module
//******************************************************************************
//------------------------------------------------------------------------------
// Global
//------------------------------------------------------------------------------
bool ADC_buf_full = false;                                                      // Флаг - буфер заполнен и готов к обработке
uint16_t ADC_buf[BUF_SIZE];
uint8_t  TMP_Str[10];
uint8_t  MEAS_Enable = 1;
//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
static BPF2_t BPF10k = {                                                        // Filter 10 kHz context
	.S1 = 0.01241106167,      
	.A2 = -1.959757686,     
	.A3 =  0.9751778841     
};

static LPF2_t LPF_Out = {
        .S1 = 0.0002413590555,
        .A2 = -1.955578208,
        .A3 = 0.956543684,
        .B2 = 2
};

const int32_t SIN_core[50] = {0, 34, 267, 882, 2031, 3825, 6318, 9506, 13324, 17648, 
22297, 27050, 31655, 35843, 39349, 41927, 43363, 43495, 42222, 39511, 35402, 30008, 
23511, 16148, 8205, 0, -8138, -15883, -22930, -29016, -33926, -37511, -39692, -40463, 
-39889, -38098, -35278, -31655, -27483, -23029, -18549, -14279, -10417, -7111, -4454,
-2478, -1154, -395, -67, 0};

const int32_t COS_core[50] = {0, 267, 1038, 2227, 3695, 5264, 6728, 7864, 8456, 8304, 
7245, 5160, 1992, -2255, -7506, -13623, -20405, -27603, -34929, -42075, -48727, -54585,
-59381, -62891, -64951, -65468, -64419, -61859, -57916, -52780, -46695, -39945, -32836,
-25679, -18770, -12379, -6730, -1992, 1729, 4393, 6027, 6719, 6611, 5883, 4744, 3411, 
2099, 996, 261, 0};

//******************************************************************************
// Local functions prototypes
//******************************************************************************
static void create_resolver_exitation (uint8_t Step);                           // Формирование сигнала возбуждения резольвера
static void start_adc_sampling (void);                                          // Запуск АЦП
static float bpf_2 (BPF2_t *C, float In);                                       // Полосовой БИХ фильтр вторго порядка
static int32_t HP_Filter (uint32_t input);                                      // Целочисленный ФВЧ (для удаления смещения)
static float lpf_2 (LPF2_t *C, float In);





//******************************************************************************
// Functions (Global first, local after)
//******************************************************************************

//******************************************************************************
//    Обработчик прерывания 8 кГц
//******************************************************************************
void routine_8kHz_INT (void){
static uint8_t Step;

    create_resolver_exitation(Step);                                            // Формирование сигнала возбуждения
    
    if (++Step == 4) {                                                          // Четыре шага формированя квадратур возбуждения
      Step = 0;
      if (MEAS_Enable) start_adc_sampling();                                    // Start ADC cycle
    }
                                                             
}
//******************************************************************************





//******************************************************************************
//   Обработка буфера - вычисление угла
//******************************************************************************
/*
На входе - указатель на буфер с сырыми данными АЦП
На выходе - угол в сотых долях градуса от -180 до 180
*/
void angle_calculate  (void){
int16_t Tmp_buf[BUF_SIZE];
uint16_t i;
int32_t SIN=0;
int32_t COS=0;
int8_t Sign;

float Angle;
    
    //HAL_Delay(200);

    if (ADC_buf_full){
      BPF10k.Prev1 = 0;
      BPF10k.Prev2 = 0;
      
      
        
      for (i=0;i<BUF_SIZE;i++){
        Tmp_buf[i] = (int16_t)bpf_2 (&BPF10k, ADC_buf[i]);                      // Убрали смещение
      }
      
      for (i=0;i<50;i++){                                                       // ДПФ
        SIN += SIN_core[i] * Tmp_buf[i + 300];
        COS += COS_core[i] * Tmp_buf[i + 300];
      }
      
      Angle = atan2f((float)SIN, (float)COS);                                   // Угол из синуса и косинуса
      Angle *= 57.2958;                                                         // Из радианов в градусы
      (Angle>0) ? (Sign=1) : (Sign=-1);                                         // Сохраняем знак
      Angle = lpf_2 (&LPF_Out, fabs(Angle));                                    // Фильтруем модуль, что бы не было резкого скачка фазы от -п в п
      
      Angle *= Sign;                                                            // Возвращаем знак
      
      Angle += 180;                                                             // Выход смещаем от 0 до 360 градусов 

      
      
      sprintf(TMP_Str, "%.2f\r\n", Angle);
      HAL_UART_Transmit(&huart1, TMP_Str, strlen(TMP_Str), 200);
      
      
      //for (i=0;i<BUF_SIZE;i++){
        //sprintf(TMP_Str, "%d\r\n", ADC_buf[i]);                                   // No filter
        //sprintf(TMP_Str, "%d\r\n", (int16_t)HP_Filter( ADC_buf[i]));            // Int HPF 
        //sprintf(TMP_Str, "%d\r\n", (int16_t)bpf_2 (&BPF10k, ADC_buf[i]));         // Float BPF
        //HAL_UART_Transmit(&huart1, TMP_Str, strlen(TMP_Str), 200);
      //}
      //sprintf(TMP_Str, "%d\r\n", 1000);
      //HAL_UART_Transmit(&huart1, TMP_Str, strlen(TMP_Str), 200);
      MEAS_Enable = 1;
      ADC_buf_full  = false;
    }
}
//******************************************************************************












//******************************************************************************
//    Генерация квадратурного сигнала для возбуждения СКВТ
//******************************************************************************
static void create_resolver_exitation (uint8_t Step){

  
  switch (Step){
//   Первая обмотка             |      Вторая обмотка    
    
  case 0:
    __STATOR_5(GPIO_PIN_RESET);
    __STATOR_6(GPIO_PIN_SET);
                                        __STATOR_7(GPIO_PIN_RESET);
                                        __STATOR_8(GPIO_PIN_SET);                                   
    break;
  case 1:
    __STATOR_5(GPIO_PIN_RESET);
    __STATOR_6(GPIO_PIN_SET);    
                                        __STATOR_7(GPIO_PIN_SET);
                                        __STATOR_8(GPIO_PIN_RESET);
    break;
  case 2:
    __STATOR_5(GPIO_PIN_SET);
    __STATOR_6(GPIO_PIN_RESET);  
                                        __STATOR_7(GPIO_PIN_SET);
                                        __STATOR_8(GPIO_PIN_RESET);
    break;
  case 3:
    __STATOR_5(GPIO_PIN_SET);
    __STATOR_6(GPIO_PIN_RESET);    
                                        __STATOR_7(GPIO_PIN_RESET);
                                        __STATOR_8(GPIO_PIN_SET);
    break;

  default:
    break;
  }






//  if (++i > 8) i = 0;
//  
//  switch (i){
//  case 0:
//    __STATOR_5(GPIO_PIN_RESET);
//    __STATOR_6(GPIO_PIN_RESET);
//    
//                                __STATOR_7(GPIO_PIN_SET);
//                                __STATOR_8(GPIO_PIN_RESET);
//    break;
//  case 1:
//    __STATOR_5(GPIO_PIN_RESET);
//    __STATOR_6(GPIO_PIN_SET);
//    
//                                __STATOR_7(GPIO_PIN_RESET);
//                                __STATOR_8(GPIO_PIN_RESET);
//    break;
//  case 2:
//    __STATOR_5(GPIO_PIN_RESET);
//    __STATOR_6(GPIO_PIN_SET);
//    
//                                __STATOR_7(GPIO_PIN_RESET);
//                                __STATOR_8(GPIO_PIN_RESET);
//    break;
//  case 3:
//    __STATOR_5(GPIO_PIN_RESET);
//    __STATOR_6(GPIO_PIN_RESET);
//    
//                                __STATOR_7(GPIO_PIN_RESET);
//                                __STATOR_8(GPIO_PIN_SET);
//    break;
//  case 4:
//    __STATOR_5(GPIO_PIN_RESET);
//    __STATOR_6(GPIO_PIN_RESET);
//    
//                                __STATOR_7(GPIO_PIN_RESET);
//                                __STATOR_8(GPIO_PIN_SET);
//    break;
//  case 5:
//    __STATOR_5(GPIO_PIN_SET);
//    __STATOR_6(GPIO_PIN_RESET);
//    
//                                __STATOR_7(GPIO_PIN_RESET);
//                                __STATOR_8(GPIO_PIN_RESET);
//    break;
//  case 6:
//    __STATOR_5(GPIO_PIN_SET);
//    __STATOR_6(GPIO_PIN_RESET);
//    
//                                __STATOR_7(GPIO_PIN_RESET);
//                                __STATOR_8(GPIO_PIN_RESET);
//    break;
//  case 7:
//    __STATOR_5(GPIO_PIN_RESET);
//    __STATOR_6(GPIO_PIN_RESET);
//    
//                                __STATOR_7(GPIO_PIN_SET);
//                                __STATOR_8(GPIO_PIN_RESET);
//    break;
//  default:
//    break;
//  }



}
//******************************************************************************


//******************************************************************************
//   Запуск нового цикла сбора данных в буфер АЦП
//******************************************************************************
static void start_adc_sampling (void){
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_buf, BUF_SIZE);                    // Запуск АЦП
    __HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT);                                    // Прерывание только по полному заполнению
    MEAS_Enable = 0;
}
//******************************************************************************



//******************************************************************************
//   Полосовой БИХ фильтр вторго порядка
//******************************************************************************
static float bpf_2 (BPF2_t *C, float In){
float Tmp;
  
  Tmp = (In * C->S1) - (C->Prev1 * C->A2) - (C->Prev2 * C->A3);  
  C->Out = Tmp - (C->Prev2);
  C->Prev2 = C->Prev1;
  C->Prev1 = Tmp;
  return C->Out;
}
//******************************************************************************


//******************************************************************************
//   Целочисленный ФВЧ (для удаления смещения)  
//******************************************************************************
static int32_t HP_Filter (uint32_t input) {
static uint32_t prev_in;
static int32_t prev_out;
  prev_out = (input + ((prev_out * 125) >> 7)) - prev_in;
  prev_in =  input;
  
  return prev_out << 0;
}
//******************************************************************************




//******************************************************************************
//  
//******************************************************************************
static float lpf_2 (LPF2_t *C, float In){
float Tmp;
  
  Tmp = (In * C->S1) - (C->Prev1 * C->A2) - (C->Prev2 * C->A3);
  C->Out = Tmp + (C->Prev1 * C->B2) + C->Prev2;
  C->Prev2 = C->Prev1;
  C->Prev1 = Tmp;
  return C->Out;
}
//******************************************************************************



//******************************************************************************
// ENF OF FILE
//******************************************************************************
