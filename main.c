
#include "crypto.h"
#include "main.h"
#include "string.h"
#include <stdlib.h>



/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma location=0x2007c000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x2007c0a0
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x2007c000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x2007c0a0))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */
#endif

ETH_TxPacketConfig TxConfig;

CRC_HandleTypeDef hcrc;

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


#define PLAINTEXT_LENGTH 32


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
int32_t STM32_AES_ECB_Encrypt(uint8_t* InputMessage,
                              uint32_t InputMessageLength,
                              uint8_t  *AES128_Key,
                              uint8_t  *OutputMessage,
                              uint32_t *OutputMessageLength);

int32_t STM32_AES_ECB_Decrypt(uint8_t* InputMessage,
                              uint32_t InputMessageLength,
                              uint8_t *AES128_Key,
                              uint8_t  *OutputMessage,
                              uint32_t *OutputMessageLength);

TestStatus Buffercmp(const uint8_t* pBuffer,
                     uint8_t* pBuffer1,
                     uint16_t BufferLength);


void BinaryToHex(uint8_t* binaryData, uint32_t dataSize, uint8_t* asciiData) {
    for (int i = 0; i < dataSize; i++) {
        uint8_t upperNibble = (binaryData[i] >> 4) & 0x0F;
        uint8_t lowerNibble = binaryData[i] & 0x0F;

        asciiData[i * 2] = (upperNibble < 10) ? (upperNibble + '0') : (upperNibble - 10 + 'A');
        asciiData[i * 2 + 1] = (lowerNibble < 10) ? (lowerNibble + '0') : (lowerNibble - 10 + 'A');
    }
}

void UART_Transmit_ASCII(uint8_t *asciiData, uint32_t dataSize, UART_HandleTypeDef* huart) {
    HAL_UART_Transmit(huart, asciiData, dataSize, 30000);
}



void HexToBinary(uint8_t* asciiData, uint32_t dataSize, uint8_t* binaryData) {
    for (int i = 0; i < dataSize; i += 2) {
        uint8_t upperNibble = (asciiData[i] <= '9') ? (asciiData[i] - '0') : (asciiData[i] - 'A' + 10);
        uint8_t lowerNibble = (asciiData[i + 1] <= '9') ? (asciiData[i + 1] - '0') : (asciiData[i + 1] - 'A' + 10);

        binaryData[i / 2] = (upperNibble << 4) | lowerNibble;
    }
}

int main(void)
{

  /* USER CODE BEGIN 1 */
	int32_t status = AES_SUCCESS;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_CRC_Init();

  // HAL_UART_Transmit(&huart3, "HELLO", 4, 1000);
  //  uint8_t buffer[4];
  //  HAL_UART_Receive(&huart3, buffer, 4, 30000);

//const uint8_t Plaintext[PLAINTEXT_LENGTH] = {0x74, 0x68, 0x69, 0x73, 0x69, 0x73, 0x73, 0x61, 0x63, 0x68, 0x69, 0x6E, 0x64, 0x73, 0x69, 0x6C};

//  HAL_UART_Transmit(&huart3,(uint8_t *)"Enter data to encrypt : ",strlen("Enter data to encrypt : "),1000);
  uint8_t Plaintext[PLAINTEXT_LENGTH];
  HAL_UART_Receive(&huart3,Plaintext,PLAINTEXT_LENGTH,30000);

  uint8_t PlaintextBinary[PLAINTEXT_LENGTH/2];



  HexToBinary(Plaintext, PLAINTEXT_LENGTH, PlaintextBinary);



//  const uint8_t Plaintext[PLAINTEXT_LENGTH] = {0x68,0x65,0x6C,0x6C,0x6F,0x77,0x6F,0x72,0x6C,0x64};

//  uint8_t Key[CRL_AES128_KEY] = {0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74};

  uint8_t PCToBoardKey[CRL_AES128_KEY] = {0x74,0X65,0X73,0X74,0X67,0X6F,0X6F,0X64,0X74,0X65,0X73,0X74,0X67,0X6F,0X6F,0X64};
//  PCToBoardKey = testgoodtestgood

  uint8_t BoardToPCKey[CRL_AES128_KEY] = {0x74, 0x65, 0x73, 0x74, 0x74, 0x65, 0x73, 0x74, 0x74, 0x65, 0x73, 0x74, 0x74, 0x65, 0x73, 0x74, 0x65, 0x73, 0x74, 0x65, 0x73};
// BoardToPCKey = testtesttesttest

//uint8_t EncryptedMessage[PLAINTEXT_LENGTH];
//uint32_t EncryptedMessageLength = 0;



//============ FOR PC TO BOARD ====================
//status = STM32_AES_ECB_Encrypt((uint8_t*)Plaintext, sizeof(Plaintext),PCToBoardKey, EncryptedMessage, &EncryptedMessageLength);
//
//
//if(status == AES_SUCCESS){
//	uint8_t EncryptedAscii[EncryptedMessageLength * 2];
//
//
//
//	BinaryToHex(EncryptedMessage, EncryptedMessageLength, EncryptedAscii);
////	Transmit the encrypted message over UART
//
//	UART_Transmit_ASCII(EncryptedAscii, EncryptedMessageLength*2, &huart3);
////	HAL_UART_Transmit(&huart3, EncryptedMessage, EncryptedMessageLength*2, 30000);
//
//
//} else {
//	Error_Handler();
//}
//====================================================================================================


//======================= BOARD TO PC ===================================================
uint8_t DecryptedMessage[16];
uint32_t DecryptedMessageLength = 0;

status = STM32_AES_ECB_Decrypt(PlaintextBinary, PLAINTEXT_LENGTH/2, PCToBoardKey, DecryptedMessage, &DecryptedMessageLength);


if(status == AES_SUCCESS){

//	BinaryToHex(DecryptedMessage, DecryptedMessageLength, DecryptedAscii);

//HAL_UART_Transmit(&huart3,(uint8_t *)"\nDecrypted data :",strlen("\nDecrypted data :"),1000);

//UART_Transmit_ASCII(DecryptedAscii, DecryptedMessageLength, &huart3);

//HAL_UART_Transmit(&huart3, DecryptedMessage, DecryptedMessageLength, 30000);
} else {
	Error_Handler();
}

uint8_t ReEncryptedMessage[16];
uint32_t ReEncryptedMessageLength = 0;



status = STM32_AES_ECB_Encrypt(DecryptedMessage,PLAINTEXT_LENGTH/2,BoardToPCKey, ReEncryptedMessage, &ReEncryptedMessageLength);

if(status == AES_SUCCESS){
	uint8_t ReEncryptedAscii[PLAINTEXT_LENGTH];




	BinaryToHex(ReEncryptedMessage, PLAINTEXT_LENGTH, ReEncryptedAscii);
//	Transmit the encrypted message over UART

//	UART_Transmit_ASCII(EncryptedAscii, EncryptedMessageLength*2, &huart3);
	HAL_UART_Transmit(&huart3, ReEncryptedAscii,PLAINTEXT_LENGTH, 30000);


} else {
	Error_Handler();
}


     /* Turn on the green led in case of AES ECB operations are successfuls*/
//     HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
     while (1)
     {}
  }



int32_t STM32_AES_ECB_Encrypt(uint8_t* InputMessage,
                              uint32_t InputMessageLength,
                              uint8_t  *AES128_Key,
                              uint8_t  *OutputMessage,
                              uint32_t *OutputMessageLength)
{
    AESECBctx_stt AESctx;
    uint32_t error_status = AES_SUCCESS;
    int32_t outputLength = 0;

    // Set flag field to default value
    AESctx.mFlags = E_SK_DEFAULT;

    // Set key size to 16 (corresponding to AES-128)
    AESctx.mKeySize = 16;

    // Initialize the operation, by passing the key.
    // Third parameter is NULL because ECB doesn't use any IV
    error_status = AES_ECB_Encrypt_Init(&AESctx, AES128_Key, NULL );

    if (error_status == AES_SUCCESS) {
        // Encrypt Data
        error_status = AES_ECB_Encrypt_Append(&AESctx,
                                               InputMessage,
                                               InputMessageLength,
                                               OutputMessage,
                                               &outputLength);

        if (error_status == AES_SUCCESS) {
            // Write the number of data written
            *OutputMessageLength = outputLength;

            // Do the Finalization
            error_status = AES_ECB_Encrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, &outputLength);

            // Add data written to the information to be returned
            *OutputMessageLength += outputLength;
        }
    }

    return error_status;
}

int32_t STM32_AES_ECB_Decrypt(uint8_t* InputMessage,
                              uint32_t InputMessageLength,
                              uint8_t *AES128_Key,
                              uint8_t  *OutputMessage,
                              uint32_t *OutputMessageLength)
{
    AESECBctx_stt AESctx;
    uint32_t error_status = AES_SUCCESS;
    int32_t outputLength = 0;

    // Set flag field to default value
    AESctx.mFlags = E_SK_DEFAULT;

    // Set key size to 16 (corresponding to AES-128)
    AESctx.mKeySize = 16;

    // Initialize the operation, by passing the key.
    // Third parameter is NULL because ECB doesn't use any IV
    error_status = AES_ECB_Decrypt_Init(&AESctx, AES128_Key, NULL );

    if (error_status == AES_SUCCESS) {
        // Decrypt Data
        error_status = AES_ECB_Decrypt_Append(&AESctx,
                                               InputMessage,
                                               InputMessageLength,
                                               OutputMessage,
                                               &outputLength);

        if (error_status == AES_SUCCESS) {
            // Write the number of data written
            *OutputMessageLength = outputLength;

            // Do the Finalization
            error_status = AES_ECB_Decrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, &outputLength);

            // Add data written to the information to be returned
            *OutputMessageLength += outputLength;
        }
    }

    return error_status;
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

TestStatus Buffercmp(const uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer != *pBuffer1)
    {
      return FAILED;
    }

    pBuffer++;
    pBuffer1++;
  }

  return PASSED;
}
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

void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
