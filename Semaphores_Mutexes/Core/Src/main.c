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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BUFFER_SIZE 5
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osThreadId producer1Handle;
osThreadId producer2Handle;
osThreadId consumer1Handle;
osThreadId consumer2Handle;
osMutexId myMutex01Handle;
osMutexId bufferMutexHandle;
osSemaphoreId semFullHandle;	// Tracks full spots
osSemaphoreId semEmptyHandle;	// Tracks empty spots
/* USER CODE BEGIN PV */
SemaphoreHandle_t xSemaphore;

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void ProducerTask(void const * argument);
void ConsumerTask(void const * argument);

/* USER CODE BEGIN PFP */

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* definition and creation of bufferMutex */
  osMutexDef(bufferMutex);
  bufferMutexHandle = osMutexCreate(osMutex(bufferMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of semFull */
  osSemaphoreDef(semFull);
  semFullHandle = osSemaphoreCreate(osSemaphore(semFull), BUFFER_SIZE);
  for (int i = 0; i < BUFFER_SIZE; i++) {
        osSemaphoreWait(semFullHandle, 0); // Consume all full places, to init count at 0
  }
  /* definition and creation of semEmpty */
  osSemaphoreDef(semEmpty);
  semEmptyHandle = osSemaphoreCreate(osSemaphore(semEmpty), BUFFER_SIZE);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  xSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(xSemaphore);

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityIdle, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityIdle, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, StartTask04, osPriorityIdle, 0, 128);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* definition and creation of producer1 */
  osThreadDef(producer1, ProducerTask, osPriorityNormal, 0, 128);
  producer1Handle = osThreadCreate(osThread(producer1), (void*)1);

  /* definition and creation of producer2 */
  osThreadDef(producer2, ProducerTask, osPriorityNormal, 0, 128);
  producer2Handle = osThreadCreate(osThread(producer2), (void*)2);

  /* definition and creation of consumer1 */
  osThreadDef(consumer1, ConsumerTask, osPriorityNormal, 0, 128);
  consumer1Handle = osThreadCreate(osThread(consumer1), (void*)1);

  /* definition and creation of consumer2 */
  osThreadDef(consumer2, ConsumerTask, osPriorityNormal, 0, 128);
  consumer2Handle = osThreadCreate(osThread(consumer2), (void*)2);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_4|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
	  {
		  // Critical Section Start
	  	  for (uint8_t i = 0; i < 5; i++) {
	  		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
	  		  vTaskDelay(pdMS_TO_TICKS(1000));
	  	  }
	  	  // Critical Section End
	  	  xSemaphoreGive(xSemaphore);
	  }
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	  vTaskDelay(pdMS_TO_TICKS(100)); // Non-critical work
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
	  if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
	  {
		  // Critical Section Start
		  for (uint8_t i = 0; i < 5; i++) {
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
			  vTaskDelay(pdMS_TO_TICKS(1000));
		  }
	  	  // Critical Section End
	  	  xSemaphoreGive(xSemaphore);
	  }
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	  vTaskDelay(pdMS_TO_TICKS(100)); // Non-critical work
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
	  if (osMutexWait(myMutex01Handle, osWaitForever) == osOK) // Task is blocked until lock acquired
	  {
		  for (uint8_t i = 0; i < 5; i++)
		  {
			  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
			  vTaskDelay(pdMS_TO_TICKS(1000));
		  }
		  osMutexRelease(myMutex01Handle);
		  vTaskDelay(pdMS_TO_TICKS(1));  // Small delay to improve fairness (1 task will win every time when they have same priority)
	  } // No else, task is blocked

  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
	  if (osMutexWait(myMutex01Handle, osWaitForever) == osOK) // Task is blocked until lock acquired
	  {
		  for (uint8_t i = 0; i < 5; i++)
		  {
			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
			  vTaskDelay(pdMS_TO_TICKS(1000));
		  }
		  osMutexRelease(myMutex01Handle);
		  vTaskDelay(pdMS_TO_TICKS(1));  // Small delay to improve fairness (1 task will win every time when they have same priority)
	  } // No else, task is blocked
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_ProducerTask */
/**
* @brief Function implementing the producer1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ProducerTask */
void ProducerTask(void const * argument)
{
  /* USER CODE BEGIN ProducerTask */
  int id = (int)argument;
  char stringBuffer[25];
  sprintf(stringBuffer, " + Produced by %d \r\n", id);
  uint32_t value = 0;
  /* Infinite loop */
  for(;;)
  {
	value++;  										// Simulate data generation
	osSemaphoreWait(semEmptyHandle, osWaitForever); // Wait, if buffer full
	osMutexWait(bufferMutexHandle, osWaitForever);  // Lock buffer
	buffer[in] = value;
	HAL_UART_Transmit(&huart2, (uint8_t*)stringBuffer, strlen(stringBuffer), HAL_MAX_DELAY);
	in = (in + 1) % BUFFER_SIZE;
	osMutexRelease(bufferMutexHandle);              // Unlock buffer
	osSemaphoreRelease(semFullHandle);              // Signal new item
    osDelay(5000);
  }
  /* USER CODE END ProducerTask */
}

/* USER CODE BEGIN Header_ConsumerTask */
/**
* @brief Function implementing the consumer1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ConsumerTask */
void ConsumerTask(void const * argument)
{
  /* USER CODE BEGIN ConsumerTask */
  int id = (int)argument;
  char stringBuffer[25];
  sprintf(stringBuffer, " --- Consumed by %d \r\n", id);
  uint32_t value;
  /* Infinite loop */
  for(;;)
  {
	osSemaphoreWait(semFullHandle, osWaitForever); // Wait for data
	osMutexWait(bufferMutexHandle, osWaitForever); // Lock buffer
	value = buffer[out];
	HAL_UART_Transmit(&huart2, (uint8_t*)stringBuffer, strlen(stringBuffer), HAL_MAX_DELAY);
	out = (out + 1) % BUFFER_SIZE;
	osMutexRelease(bufferMutexHandle); 			   // Unlock buffer
	osSemaphoreRelease(semEmptyHandle); 		   // Signal space available
    osDelay(5000);
  }
  /* USER CODE END ConsumerTask */
}

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
