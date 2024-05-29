/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "crypto.h"
#include "main.h"
#include "string.h"
#include "stdio.h"
#define MAX_OUTPUT_SIZE 256
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

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

CRC_HandleTypeDef hcrc;//Declare handle Used for Error Checking in Data Transmission

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
/* NIST vectors examples for CCM are taken from:
	"Recommendation for Block Cipher Modes of Operation:
	The CCM Mode for Authentication and Confidentiality"
	Available at:
	http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
	base url:
	http://csrc.nist.gov/publications/PubsSPs.html
*/

/* length of NIST tag test vector of example */
#define TAG_LENGTH	4
/* length of NIST plain text test vector of example */
#define PLAINTEXT_LENGTH	4
/* length of NIST cipher text test vector of example */
#define CIPHER_TEXT_LENGTH	PLAINTEXT_LENGTH + TAG_LENGTH
//must be accounted for to prevent buffer overflows or data loss
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Header message, will be authenticated but not encrypted */
const uint8_t HeaderMessage[] =
{
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07
};

uint32_t HeaderLength = sizeof (HeaderMessage) ;

/* string length only, without '\0' end of string marker */


/* Payload message, will be authenticated and encrypted */
/*const uint8_t InputMessage[] =
{
	0x20,0x21,0x22,0x23
};*/
//,

/* string length only, without '\0' end of string marker */
//uint32_t InputLength = sizeof (InputMessage) ;

/* Key to be used for AES encryption/decryption */
uint8_t Key[CRL_AES128_KEY] =
{
  0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
  0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f
};

/*const uint8_t Expected_Ciphertext[CIPHER_TEXT_LENGTH] =
{
  0x71,0x61,0x02,0x5b,0x4d,0xac,0x25,0x5d
};*/
/* Initialization Vector, used only in non-ECB modes like CBC(Cipher Block Chaining) and CCM(Counter CBC-MAC) */
//Enhances Security
uint8_t IV[] =
 {
   0x10,0x11,0x12,0x13,0x14,0x15,0x16
 };

/* NIST example 1 ciphertext vector: in encryption we expect this vector as result */
/* Buffer to store the output data and the authentication TAG */
uint8_t OutputMessage[64];

/* Length of the Authentication TAG */
int32_t AuthenticationTAGLength = 0;

/* Size of the output data */
int32_t OutputMessageLength = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
int32_t AES_CCM_Encrypt(uint8_t*  HeaderMessage,
                        uint32_t  HeaderMessageLength,
                        uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *AES128_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        int32_t *OutputMessageLength,
                        int32_t *AuthenticationTAGLength
                        );

int32_t AES_CCM_Decrypt(uint8_t*  HeaderMessage,
                        uint32_t  HeaderMessageLength,
                        uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *AES128_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        int32_t *OutputMessageLength,
                        int32_t  AuthenticationTAGLength);

TestStatus Buffercmp(const uint8_t* pBuffer,
                     uint8_t* pBuffer1,
                     uint16_t BufferLength);
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
    /* Initialize variables */
	int32_t status = AES_SUCCESS;

    /* Initialize peripherals */
    HAL_Init();
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */

    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ETH_Init();
    MX_USART3_UART_Init();
    MX_USB_OTG_FS_PCD_Init();
    MX_CRC_Init();
    /* USER CODE BEGIN 2 */
    HAL_UART_Transmit(&huart3, (uint8_t *)"Enter data to encrypt: ", strlen("Enter data to encrypt: "), 1000);
	uint8_t InputMessage[5];
	uint8_t InputLength = 0;
	HAL_UART_Receive(&huart3, InputMessage, sizeof(InputMessage), HAL_MAX_DELAY);
	InputLength = sizeof(InputMessage);
	status = AES_CCM_Encrypt(HeaderMessage, HeaderLength, InputMessage, InputLength, Key, IV, sizeof(IV), OutputMessage, &OutputMessageLength, &AuthenticationTAGLength);

     if (status == AES_SUCCESS)
     {
 	   HAL_UART_Transmit(&huart3, (uint8_t *)"\nEncrypted data: ", strlen("\nEncrypted data: "), 1000);

 	   HAL_UART_Transmit(&huart3, OutputMessage, OutputMessageLength, 1000);

        // Decrypt the encrypted data
 		uint8_t DecryptedMessage[MAX_OUTPUT_SIZE];
 		uint32_t DecryptedMessageLength = 0;


 		status = AES_CCM_Decrypt(HeaderMessage, HeaderLength,OutputMessage,
 								  OutputMessageLength, Key, IV, sizeof(IV), DecryptedMessage,
 								  &DecryptedMessageLength, AuthenticationTAGLength);

 		if (status == AUTHENTICATION_SUCCESSFUL)
 		{
 			HAL_UART_Transmit(&huart3, (uint8_t *)"\nDecrypted data: ", strlen("\nDecrypted data: "), 1000);
 			HAL_UART_Transmit(&huart3, DecryptedMessage, DecryptedMessageLength, 1000);

 			// Checking
 			TestStatus testResult = Buffercmp(InputMessage, DecryptedMessage, InputLength);
 			if (testResult == PASSED)
 			{
 				HAL_UART_Transmit(&huart3, (uint8_t *)"\nEncryption and decryption successful. "

 						"Original and decrypted data match.\n",

 						strlen("\nEncryption and decryption successful. Original and decrypted data match.\n"), 1000);
 			}
 			else
 			{
 				HAL_UART_Transmit(&huart3, (uint8_t *)"\nEncryption and decryption failed. "

 				"Original and decrypted data do not match.\n",

 				strlen("\nEncryption and decryption failed. Original and decrypted data do not match.\n"), 1000);
 			}
 			HAL_UART_Transmit(&huart3, DecryptedMessage,DecryptedMessageLength, 1000);

 			// Send the success message
 		}
 		else

 		{
 			HAL_UART_Transmit(&huart3, (uint8_t *)"Decryption failed.\n", strlen("Decryption failed.\n"), 1000);
 		}

 	}

 	else
 	{
 		HAL_UART_Transmit(&huart3, (uint8_t *)"Encryption failed.\n", strlen("Encryption failed.\n"), 1000);
 	}

 	while (1)
 		{

 		}
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
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

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
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

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
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

/* USER CODE BEGIN 4 */
/**
  * @brief  AES CCM Authenticated Encryption example.
  * @param  HeaderMessage: pointer to the header message. It will be authenticated but not encrypted.
  * @param  HeaderMessageLength: header message length in byte.
  * @param  InputMessage: pointer to input message to be encrypted.
  * @param  InputMessageLength: input data message length in byte.
  * @param  AES128_Key: pointer to the AES key to be used in the operation
  * @param  InitializationVector: pointer to the Initialization Vector (IV)
  * @param  IvLength: IV length in bytes.
  * @param  OutputMessage: pointer to output parameter that will handle the encrypted message and TAG
  * @param  OutputMessageLength: pointer to encrypted message length.
  * @param  AuthenticationTAGLength: authentication TAG length.
  * @retval error status: can be AES_SUCCESS if success or one of
  *         AES_ERR_BAD_OPERATION, AES_ERR_BAD_CONTEXT
  *         AES_ERR_BAD_PARAMETER if error occured.
  */
int32_t AES_CCM_Encrypt(uint8_t*  HeaderMessage,
                        uint32_t  HeaderMessageLength,
                        uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *AES128_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        int32_t *OutputMessageLength,
                        int32_t *AuthenticationTAGLength
                        )
{
  AESCCMctx_stt AESctx;

  uint32_t error_status = AES_SUCCESS;

  /* Set flag field to default value */
  AESctx.mFlags = E_SK_DEFAULT;

  /* Set key size to 16 (corresponding to AES-128) */
  AESctx.mKeySize = 16;

  /* Set nonce size field to IvLength, note that valid values are 7,8,9,10,11,12,13*/
  AESctx.mNonceSize = IvLength;

  /* Size of returned authentication TAG */
  AESctx.mTagSize = 4;

  /* Set the size of the header */
  AESctx.mAssDataSize = HeaderMessageLength;

  /* Set the size of thepayload */
  AESctx.mPayloadSize = InputMessageLength;

  /* Initialize the operation, by passing the key and IV */
  error_status = AES_CCM_Encrypt_Init(&AESctx, AES128_Key, InitializationVector );

  /* check for initialization errors */
  if(error_status == AES_SUCCESS)
  {
    /* Process Header */
    error_status = AES_CCM_Header_Append(&AESctx,
                                         HeaderMessage,
                                         HeaderMessageLength);
    if(error_status == AES_SUCCESS)
    {
      /* Encrypt Data */
      error_status = AES_CCM_Encrypt_Append(&AESctx,
                                            InputMessage,
                                            InputMessageLength,
                                            OutputMessage,
                                            OutputMessageLength);

      if(error_status == AES_SUCCESS)
      {
        /* Do the Finalization, write the TAG at the end of the encrypted message */
        error_status = AES_CCM_Encrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, AuthenticationTAGLength);
      }
    }
  }

  return error_status;
}


/**
  * @brief  AES CCM Authenticated Decryption example.
  * @param  HeaderMessage: pointer to the header message. It will be authenticated but not Decrypted.
  * @param  HeaderMessageLength: header message length in byte.
  * @param  InputMessage: pointer to input message to be Decrypted.
  * @param  InputMessageLength: input data message length in byte.
  * @param  AES128_Key: pointer to the AES key to be used in the operation
  * @param  InitializationVector: pointer to the Initialization Vector (IV)
  * @param  IvLength: IV length in bytes.
  * @param  OutputMessage: pointer to output parameter that will handle the Decrypted message and TAG
  * @param  OutputMessageLength: pointer to Decrypted message length.
  * @param  AuthenticationTAGLength: authentication TAG length.
  * @retval error status: can be AUTHENTICATION_SUCCESSFUL if success or one of
  *         AES_ERR_BAD_OPERATION, AES_ERR_BAD_CONTEXT
  *         AES_ERR_BAD_PARAMETER, AUTHENTICATION_FAILED if error occured.
  */
int32_t AES_CCM_Decrypt(uint8_t*  HeaderMessage,
                        uint32_t  HeaderMessageLength,
                        uint8_t*  InputMessage,
                        uint32_t  InputMessageLength,
                        uint8_t  *AES128_Key,
                        uint8_t  *InitializationVector,
                        uint32_t  IvLength,
                        uint8_t  *OutputMessage,
                        int32_t *OutputMessageLength,
                        int32_t  AuthenticationTAGLength
                        )
{
  AESCCMctx_stt AESctx;

  uint32_t error_status = AES_SUCCESS;

  /* Set flag field to default value */
  AESctx.mFlags = E_SK_DEFAULT;

  /* Set key size to 16 (corresponding to AES-128) */
  AESctx.mKeySize = 16;

  /* Set nonce size field to IvLength, note that valid values are 7,8,9,10,11,12,13*/
  AESctx.mNonceSize = IvLength;

  /* Size of returned authentication TAG */
  AESctx.mTagSize = 4;

  /* Set the size of the header */
  AESctx.mAssDataSize = HeaderMessageLength;

  /* Set the size of thepayload */
  AESctx.mPayloadSize = InputMessageLength;

  /* Set the pointer to the TAG to be checked */
  AESctx.pmTag = InputMessage + InputMessageLength;

  /* Size of returned authentication TAG */
  AESctx.mTagSize = AuthenticationTAGLength;


  /* Initialize the operation, by passing the key and IV */
  error_status = AES_CCM_Decrypt_Init(&AESctx, AES128_Key, InitializationVector );

  /* check for initialization errors */
  if(error_status == AES_SUCCESS)
  {
    /* Process Header */
    error_status = AES_CCM_Header_Append(&AESctx,
                                         HeaderMessage,
                                         HeaderMessageLength);
    if(error_status == AES_SUCCESS)
    {
      /* Decrypt Data */
      error_status = AES_CCM_Decrypt_Append(&AESctx,
                                            InputMessage,
                                            InputMessageLength,
                                            OutputMessage,
                                            OutputMessageLength);

      if(error_status == AES_SUCCESS)
      {
        /* Do the Finalization, check the authentication TAG*/
        error_status = AES_CCM_Decrypt_Finish(&AESctx, NULL, &AuthenticationTAGLength);
      }
    }
  }

  return error_status;
}
//Compares expected cipher text with actual cipher text for verifying correctness
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
