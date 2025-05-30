/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "buzzer.h"
#include "gate.h"
#include "dht11.h"
#include "delay.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "i2c_lcd.h"
#include "ultrasonic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifdef __GNUC__
/* With GCC small printf (option LD Linker->Libraries->Small printf
 * set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int  __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int  fputc(int ch, FILE *f)
#endif /* __GNUC__*/

/** @brief Retargets the C library printf function to the USART.
 *  @param None
 *  @retval None
 */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop
     until the end of transmission */
  if(ch == '\n')
     HAL_UART_Transmit(&huart2, (uint8_t*) "\r", 1, 0xFFFF);
     HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 0xFFFF);
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DHT11 dht;
ULTRASONIC ultra1;
uint8_t rx_buffer[20]; // Comport Master에서 수신한 문자열 버퍼
uint8_t bt_buffer[20]; // HC-05 응답 버퍼


volatile uint8_t rx_index = 0; // 수신 인덱스
 uint8_t rx_data=0;
volatile uint8_t rx_complete = 0; // Comport Master 수신 완료 플래그


volatile uint8_t bt_index = 0; // HC-05 응답 인덱스
 uint8_t bt_data=0;
volatile uint8_t bt_complete = 0; // HC-05 응답 완료 플래그


uint8_t current_mode;

uint8_t str_len=0;
typedef enum{
  AT,
  COM
}MODE;


uint8_t park1_light=0;
uint8_t park2_light=0;
uint8_t park3_light=0;
uint8_t park4_light=0;
uint8_t park5_light=0;
uint8_t flame=0;

volatile uint16_t adcValue[6]={0};
volatile uint8_t check_light[5]={0};
uint16_t leds[5]={ PARK1_LED_Pin, PARK2_LED_Pin , PARK3_LED_Pin, PARK4_LED_Pin, PARK5_LED_Pin};

uint8_t vacant_count=0;

uint8_t lcd_buffer[30];

uint32_t blink_count = 0;
//MORO ENTERENCE
typedef enum
{
  IDLE,
  LED_BLINK,
  MOTOR_OPEN,
  MOTOR_CLOSE
}ControlState;

ControlState control_state=IDLE;

uint32_t last_distance = 999;
uint8_t action_triggered=0;
uint8_t motor_active=0;
uint32_t timer_count=0;

uint16_t delay_count=0;
 uint16_t last_step=(uint16_t)((120 * STEPS_PER_REVOLUTION) / 360);
 uint16_t current_step=0;
 uint8_t camera_flag=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void set_AT_mode(void);
void set_DATA_mode(void);

void Send_AT_Command() {
    char response[50] = {0};

    str_len=strlen(rx_buffer);
    printf("Sending to HC-05: %s \r\n", rx_buffer);
    HAL_StatusTypeDef tx_status = HAL_UART_Transmit(&huart6, (uint8_t *)rx_buffer, strlen(rx_buffer), 1000);
    if (tx_status != HAL_OK) {
	printf("Transmit Failed! Status: %d\r\n", tx_status);
    }

    printf("Waiting for Response...\r\n");
    HAL_StatusTypeDef rx_status = HAL_UART_Receive(&huart6, (uint8_t *)response, 10, 2000);
    if (rx_status == HAL_OK) {
	printf("HC-05 Response: '%s'\r\n", response);
    } else {
	printf("Receive Failed! Status: %d\r\n", rx_status);
    }
}

void sensor_process(void);
void led_on_off(void);
uint8_t cal_vacant_place(void);
void lcd_print(void);

void motor_open(void);
void motor_close(void);
void motor_stop(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_USART6_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */
  i2c_lcd_init();
  HAL_ADC_Start_DMA(&hadc1,adcValue,6);//0 : park1_light 1 : park2_light 2 : park3_light 3: park4_light 4 : park5_light 5 : flame
  HAL_TIM_Base_Start(&htim1);              //Timer for DHT11
  HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_1);//Timer for HR-SC04
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);


  dht11Init(&dht, GPIOC, GPIO_PIN_9);
  ultra_Init(&ultra1, GPIOA, GPIO_PIN_8);
  move_cursor(1,1);


  printf("system started. Good Luck. \r\n");
  set_AT_mode();
  //set_COM_mode();
  current_mode=AT;

  HAL_UART_Receive_IT(&huart2, rx_buffer, 1);
  //HAL_UART_Receive_IT(&huart1, bt_buffer, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

     /*dht11 test code*/
        if(flame != 1)
        {
           ultra_Trigger(&ultra1, TIM_IT_CC1);
	   HAL_Delay(60);
	   sensor_process();
	   HAL_Delay(10);
	    //빈 자리 계산
	   vacant_count=cal_vacant_place();

           //점유 여부 표시
           led_on_off();
           if(dht11Read(&dht))
           {
             //라즈베리파이에 블루투스 데이터 송신
		 snprintf(data_to_ras,sizeof(data_to_ras),"Temp:%d/Humi:%d/pl1:%d/pl2:%d/pl3:%d/pl4:%d/pl5:%d/empty:%d/flame:%d/Main:%s/CF:%s\r\n",
			  dht.temperature,dht.humidity,check_light[0],check_light[1],check_light[2],check_light[3],check_light[4],vacant_count,flame,motor_active == 1?"MOVE":"STOP",camera_flag == 1? "CARIN":"NOCAR");
		 HAL_UART_Transmit(&huart6, (uint8_t *)data_to_ras, strlen(data_to_ras), 1000);

		 //안드로이드 어플 블루투스 데이터 통신
		 snprintf(data_to_ras,sizeof(data_to_ras),"Temp:%d/Humi:%d/pl1:%d/pl2:%d/pl3:%d/pl4:%d/pl5:%d/empty:%d/flame:%d/Main:%s\r\n",
				  dht.temperature,dht.humidity,check_light[0],check_light[1],check_light[2],check_light[3],check_light[4],vacant_count,flame,action_triggered == 1?"MOVE":"STOP",camera_flag == 1? "CARIN":"NOCAR");
		 HAL_UART_Transmit(&huart1, (uint8_t *)data_to_ras, strlen(data_to_ras), 1000);

		 //LCD데이터 출력
		 move_cursor(0,0);
		 sprintf(lcd_buffer,"Empty Space:%d",vacant_count);
		 lcd_string(lcd_buffer);
		 move_cursor(1,0);
		 sprintf(lcd_buffer,"Temp:%d,Humid:%d",dht.temperature, dht.humidity);
		 lcd_string(lcd_buffer);

		 printf("tem:%d , hum: %d %% \r\n",dht.temperature,dht.humidity);
		 if(camera_flag==1)
		 {
		     camera_flag=0;
		 }
           }
           else
           {
               printf("DHT11 can't\n\r");
            }
        }
        else
        {
          //BUzze r 울리기

        }
        HAL_Delay(2000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void set_AT_mode()
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  printf("current mode : %s \n", current_mode == 1 ? "COM":"AT");
  current_mode=AT;
  HAL_Delay(500);
}

void set_DATA_mode()
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  printf("current mode : %s", current_mode == 1 ? "COM":"AT");
  current_mode=COM;
  HAL_Delay(500);
}

void sensor_process(void)
{

  for(uint8_t i=0;i<5;i++)
  {
    if(adcValue[i]>800)
    {
       check_light[i]=1;
    }
    else
    {
       check_light[i]=0;
    }
  }

  if(adcValue[5]>950)
  {
     flame=1;
  }
  else
  {
     flame=0;
  }
}
uint8_t cal_vacant_place(void)
{
  uint8_t count=0;
  for(uint8_t i=0; i<5; i++)
  {
    //count++ if no car is in parking lot
    if(check_light[i] == 0)
    {
      count++;
    }
  }
  return count;
}

void led_on_off(void)
{
  for(uint8_t i=0; i<5;i++)
  {
    if(check_light[i] == 1)
    {
      HAL_GPIO_WritePin(GPIOC, leds[i], GPIO_PIN_SET);
    }
    else if(check_light[i] == 0)
    {
      HAL_GPIO_WritePin(GPIOC, leds[i], GPIO_PIN_RESET);
    }
}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_13)
  {
      if(current_mode == AT)
      {
	current_mode = COM;
      }
      else if(current_mode == COM)
      {
	current_mode = AT;
      }
  }
}

// 모터 제어 함수
void motor_open(void) {
  //TIM10시작
}

void motor_close(void) {
  //TIM10 시작
}

void motor_stop(void) {
  //타미어 10 정지
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    //PC <-> STM32
  if (huart->Instance == USART2) {
          if (rx_buffer[rx_index] == '\n' || rx_index >= sizeof(rx_buffer) - 1) {
              rx_complete = 1;
          }
          else
          {
              rx_index++;
              HAL_UART_Receive_IT(&huart2, &rx_buffer[rx_index], 1);
          }
      }
}

//Input Capture Callback
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM3)
  {
    if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      if(ultra1.capture_flag == 0)
      {
	IC_Value_1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
	ultra1.capture_flag=1;
	__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
      }
      else if(ultra1.capture_flag == 1)
      {
	IC_Value_2 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
	if(IC_Value_2 > IC_Value_1)
	{
	  echo_time=IC_Value_2 - IC_Value_1;
	}
	else
	{
	  echo_time=(0xFFFF - IC_Value_1) + IC_Value_2;
	}
	ultra1.distance=echo_time/58;
//        if(echo_time/58 > 400)
//        {
//          ultra1.distance = 400;
//        }
//        else if(echo_time/58 < 2)
//	 {
//	   ultra1.distance = 2;
//	 }
	printf("Object detected....[ %d cm ] \r\n",ultra1.distance);

	if((echo_time/58 <20) && !action_triggered)
	{
	  timer_count=0;
	   control_state = MOTOR_OPEN;//모터 회전(문 열기)
	  // motor_start_time = HAL_GetTick();
	   motor_open();
	   camera_flag=1;
	   action_triggered=1;
	   HAL_TIM_Base_Start_IT(&htim4);
	   HAL_TIM_Base_Start_IT(&htim10);
	  // BUZZER_Open();
	}
	else if((echo_time/58 >20) && motor_active && control_state != MOTOR_CLOSE)
	{
	  //차량 사라짐 모터 역회전
	  timer_count=0;
	  motor_active=1;
	  control_state=MOTOR_CLOSE;
	  motor_close();
	  HAL_TIM_Base_Start_IT(&htim4);
	  HAL_TIM_Base_Start_IT(&htim10);
	  //BUZZER_Close();

	}

	ultra1.capture_flag = 0;
	__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
	__HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC1);
      }
    }
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

  if(htim->Instance == TIM4)
  {
    switch(control_state)
    {
      case MOTOR_OPEN:
	timer_count++;
	if(timer_count >=1000)
	{
	  motor_stop();
	  motor_active=1;
	  action_triggered=1;
	  control_state =IDLE;
	  timer_count=0;
	  HAL_TIM_Base_Stop_IT(&htim4);
	}
	break;
      case MOTOR_CLOSE:
	timer_count++;
	if(timer_count >=1000)
	{
	  motor_stop();
	  motor_active=0;
	  action_triggered=0;
	  control_state=IDLE;
	  HAL_TIM_Base_Stop_IT(&htim4);
	}
         break;
      default:
	timer_count=0;
	//HAL_TIM_Base_Stop_IT(&htim4);
	break;
    }
  }
  if(htim->Instance == TIM10)
  {
//   static uint16_t delay_count=0;
//    static uint16_t last_step=(uint16_t)((90 * STEPS_PER_REVOLUTION) / 360);
//    static uint16_t current_step=0;
    switch(control_state)
    {
      case MOTOR_OPEN:
	delay_count++;
	if(current_step < last_step)
	{
	  stepMotor(current_step % 8); // 8상 스텝 제어
	  current_step++;
	}
	else if(current_step >=last_step)
	{
	  delay_count=0;
	  current_step=0;
	  HAL_TIM_Base_Stop_IT(&htim10);
	}
	break;
      case MOTOR_CLOSE:

      	delay_count++;
	if(current_step < last_step)
	{
	  stepMotor(7-(current_step % 8)); // 8상 스텝 제어
	  current_step++;
	}
	else if(current_step >=last_step)
	{
	  delay_count=0;
	  current_step=0;
	  HAL_TIM_Base_Stop_IT(&htim10);
	}
	break;

      default:
	delay_count=0;
	current_step=0;
	 //HAL_TIM_Base_Stop_IT(&htim10);
	break;
    }
  }

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
