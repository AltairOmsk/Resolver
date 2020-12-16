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
bool ADC_buf_full = false;                                                      // ���� - ����� �������� � ����� � ���������
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

//******************************************************************************
// Local functions prototypes
//******************************************************************************
static void create_resolver_exitation (uint8_t Step);                           // ������������ ������� ����������� ����������
static void start_adc_sampling (void);                                          // ������ ���
static float bpf_2 (BPF2_t *C, float In);                                       // ��������� ��� ������ ������ �������
static int32_t HP_Filter (uint32_t input);                                      // ������������� ��� (��� �������� ��������)






//******************************************************************************
// Functions (Global first, local after)
//******************************************************************************

//******************************************************************************
//    ���������� ���������� 8 ���
//******************************************************************************
void routine_8kHz_INT (void){
static uint8_t Step;

    create_resolver_exitation(Step);                                            // ������������ ������� �����������
    
    if (++Step == 4) {                                                          // ������ ���� ����������� ��������� �����������
      Step = 0;
      if (MEAS_Enable) start_adc_sampling();                                    // Start ADC cycle
    }
                                                             
}
//******************************************************************************





//******************************************************************************
//   ��������� ������ - ���������� ����
//******************************************************************************
/*
�� ����� - ��������� �� ����� � ������ ������� ���
�� ������ - ���� � ����� ����� ������� �� -180 �� 180
*/
void angle_calculate  (void){
    //HAL_Delay(200);
    
    if (ADC_buf_full){
      BPF10k.Prev1 = 0;
      BPF10k.Prev2 = 0;
      for (uint16_t i=0;i<BUF_SIZE;i++){
        //sprintf(TMP_Str, "%d\r\n", ADC_buf[i]);                                   // No filter
        //sprintf(TMP_Str, "%d\r\n", (int16_t)HP_Filter( ADC_buf[i]));            // Int HPF 
        sprintf(TMP_Str, "%d\r\n", (int16_t)bpf_2 (&BPF10k, ADC_buf[i]));         // Float BPF
        HAL_UART_Transmit(&huart1, TMP_Str, strlen(TMP_Str), 200);
      }
      sprintf(TMP_Str, "%d\r\n", 1000);
      HAL_UART_Transmit(&huart1, TMP_Str, strlen(TMP_Str), 200);
      MEAS_Enable = 1;
    }
}
//******************************************************************************












//******************************************************************************
//    ��������� ������������� ������� ��� ����������� ����
//******************************************************************************
static void create_resolver_exitation (uint8_t Step){

  
  switch (Step){
//   ������ �������             |      ������ �������    
    
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
//   ������ ������ ����� ����� ������ � ����� ���
//******************************************************************************
static void start_adc_sampling (void){
    ADC_buf_full  = false;
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_buf, BUF_SIZE);                    // ������ ���
    __HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT);                                // ���������� ������ �� ������� ����������
    MEAS_Enable = 0;
}
//******************************************************************************



//******************************************************************************
//   ��������� ��� ������ ������ �������
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
//   ������������� ��� (��� �������� ��������)  
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
// ENF OF FILE
//******************************************************************************
