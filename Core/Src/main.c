/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DHT11_PIN  GPIO_PIN_8
#define DHT11_PORT GPIOA
#define TIM_US     htim2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define WIFI_UART &huart2
// 封装一个简单的发送函数，比 printf 更稳定
void ESP_SendCmd(char *cmd) {
    HAL_UART_Transmit(WIFI_UART, (uint8_t *)cmd, strlen(cmd), 1000);
}
void ESP8266_Init_Auto(void) {
    // 0. 稍微等待模块启动
    HAL_Delay(1000);
    
    // 1. 测试 AT (清除串口缓存，测试连接)
    ESP_SendCmd("AT\r\n");
    HAL_Delay(1000);
    
    // 2. 设置为 Station 模式
    ESP_SendCmd("AT+CWMODE=1\r\n");
    HAL_Delay(1000);
    
    // 3. 连接 Wi-Fi
    // ⚠️【修改】把 123 换成你的 WiFi 名，12345678 换成密码
    // 注意：保留双引号 \" 不要删
    ESP_SendCmd("AT+CWJAP=\"123\",\"12345678\"\r\n");
    
    // 【关键等待】Wi-Fi 连接通常需要 3-8 秒，这里给足时间
    HAL_Delay(8000);
    
    // 4. 连接电脑服务器 (NetAssist)
    // ⚠️【修改】把 192.168.111.85 换成你刚才 ping 通的电脑 IP
    // ⚠️【修改】8888 换成你的端口号
    ESP_SendCmd("AT+CIPSTART=\"TCP\",\"192.168.111.85\",8888\r\n");
    
    // 连接服务器也需要一点时间
    HAL_Delay(2000);
    
    // 5. 开启透传模式 (Transmission Mode)
    ESP_SendCmd("AT+CIPMODE=1\r\n");
    HAL_Delay(500);
    
    // 6. 开始发送
    ESP_SendCmd("AT+CIPSEND\r\n");
    HAL_Delay(500);
    
    // 此时如果不报错，就已经进入透传模式了，后面发啥电脑收啥
}

/* --- 1. printf 重定向函数 (让 printf 输出到串口) --- */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

/* --- 2. 微秒延时函数 --- */
static void DWT_Delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&TIM_US, 0);
    while (__HAL_TIM_GET_COUNTER(&TIM_US) < us);
}

/* --- 3. DHT11 读一位 --- */
static uint8_t DHT11_ReadBit(void)
{
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET);
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET);
    DWT_Delay_us(40);
    return HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET;
}

/* --- 4. DHT11 读一字节 --- */
static uint8_t DHT11_ReadByte(void)
{
    uint8_t i, byte = 0;
    for (i = 0; i < 8; i++)
        byte = (byte << 1) | DHT11_ReadBit();
    return byte;
}

/* --- 5. DHT11 读取主函数 --- */
uint8_t DHT11_Read(float *temp, float *humi)
{
    uint8_t buf[5] = {0};
    uint8_t i;

    /* 主机拉低 ≥18 ms */
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    DWT_Delay_us(18000);
    /* 释放总线，等待上拉 */
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    DWT_Delay_us(30);

    /* 检测响应 */
    if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) != GPIO_PIN_RESET)
        return 1;  // 无响应
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET);
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET);

    /* 读数据 */
    for (i = 0; i < 5; i++) buf[i] = DHT11_ReadByte();

    /* 校验 */
    if (buf[4] != (buf[0] + buf[1] + buf[2] + buf[3]))
        return 2;  // 校验错

    *humi = buf[0] + buf[1] * 0.1f;
    *temp = buf[2] + buf[3] * 0.1f;
    return 0;      // 成功
}
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&TIM_US); // 开启定时器
  printf("DHT11 Test Start...\r\n"); // 先打印一句，测试串口通不通
  printf("System Start...\r\n"); // 发给电脑看：系统启动了
  ESP8266_Init_Auto();
  
  // --- 新增：测试 ESP8266 ---
  //uint8_t at_cmd[] = "AT\r\n"; // AT指令，后面必须加回车换行
  
  // 通过 USART2 发送给 ESP8266
 // HAL_UART_Transmit(&huart2, at_cmd, sizeof(at_cmd)-1, 1000);
  
  // 同时打印给电脑看，证明程序跑到这一步了
 // printf("Sent AT command to ESP8266 via USART2\r\n");
  /* USER CODE END 2 */
	float temp,humi;
	char dataBuffer[64]; // 定义一个缓存区用来存放要发的数据
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      if (DHT11_Read(&temp, &humi) == 0)
		  sprintf(dataBuffer, "Temp=%.1f, Humi=%.1f\r\n", temp, humi);
	      // 2. 发送给电脑
          // 因为已经透传了，直接用 HAL_UART_Transmit 发送内容即可
	      HAL_UART_Transmit(WIFI_UART, (uint8_t *)dataBuffer, strlen(dataBuffer), 1000);
         // printf("T=%.1f  H=%.1f\r\n", t, h);
      HAL_Delay(2000); // 间隔2秒读一次
      
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : DHT11_Pin */
  GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
