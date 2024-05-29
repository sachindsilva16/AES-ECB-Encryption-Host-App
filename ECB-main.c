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
/* Private define ------------------------------------------------------------*/
/* NIST vectors examples for CCM are taken from:
	"Recommendation for Block Cipher Modes of Operation:
	The CCM Mode for Authentication and Confidentiality"
	Available at:
	http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
	base url:
	http://csrc.nist.gov/publications/PubsSPs.html
*/


#define PLAINTEXT_LENGTH 16

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Header message, will be authenticated but not encrypted */
//const uint8_t HeaderMessage[] =
//{
//  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07
//};
//
//uint32_t HeaderLength = sizeof (HeaderMessage) ;
//
///* string length only, without '\0' end of string marker */


/* Payload message, will be authenticated and encrypted */
//const uint8_t Plaintext[PLAINTEXT_LENGTH] =
//  {
//    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
//    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
//    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
//    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
//    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
//    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
//    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
//    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
//  };

//const uint8_t Plaintext[PLAINTEXT_LENGTH] =
//  {
//    0x6b, 0x24, 0xbe, 0x21, 0x2e, 0x40, 0x9f, 0x96,
//    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x91, 0x17, 0x2a,
//    0xae, 0x2d, 0x8a, 0x57, 0x14, 0x02, 0xac, 0x9c,
//    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
//    0x30, 0x21, 0x24, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
//    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0x21,
//    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
//    0x10, 0x2b, 0x41, 0x21, 0xe6, 0x6c, 0x37, 0x10
//  };


//const uint8_t Plaintext[PLAINTEXT_LENGTH] = {0x68,0x65,0x6C,0x6C,0x6F,0x77,0x6F,0x72,0x6C,0x64,0x00,0x00,0x00,0x00,0x00};

/* Key to be used for AES encryption/decryption */


//uint8_t Key[CRL_AES128_KEY] = {0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74};


//uint8_t Key[CRL_AES256_KEY] =
//{
//		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
//		0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
//		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
//		0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
//};


//const uint8_t Expected_Ciphertext[PLAINTEXT_LENGTH] =
//  {
//    0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c,
//    0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8,
//    0x59, 0x1c, 0xcb, 0x10, 0xd4, 0x10, 0xed, 0x26,
//    0xdc, 0x5b, 0xa7, 0x4a, 0x31, 0x36, 0x28, 0x70,
//    0xb6, 0xed, 0x21, 0xb9, 0x9c, 0xa6, 0xf4, 0xf9,
//    0xf1, 0x53, 0xe7, 0xb1, 0xbe, 0xaf, 0xed, 0x1d,
//    0x23, 0x30, 0x4b, 0x7a, 0x39, 0xf9, 0xf3, 0xff,
//    0x06, 0x7d, 0x8d, 0x8f, 0x9e, 0x24, 0xec, 0xc7
//  };


/* Buffer to store the output data */
//uint8_t OutputMessage[PLAINTEXT_LENGTH];
//
//
///* Size of the output data */
//int32_t OutputMessageLength = 0;




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


void BinaryToAscii(uint8_t* binaryData, uint32_t dataSize, uint8_t* asciiData) {
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_CRC_Init();


//  const uint8_t Plaintext[PLAINTEXT_LENGTH] = {0x68,0x65,0x6C,0x6C,0x6F,0x77,0x6F,0x72,0x6C,0x64,0x05,0x05,0x05,0x05,0x05};

//  const uint8_t Plaintext[PLAINTEXT_LENGTH] = {0x68,0x65,0x6C,0x6C,0x6F,0x77,0x6F,0x72,0x6C,0x64,0x05,0x05,0x05,0x05,0x05};
  uint8_t Plaintext[PLAINTEXT_LENGTH];
  HAL_UART_Receive(&huart3,Plaintext,PLAINTEXT_LENGTH,30000);

//  const uint8_t Plaintext[PLAINTEXT_LENGTH] = {0x68,0x65,0x6C,0x6C,0x6F,0x77,0x6F,0x72,0x6C,0x64};

  uint8_t Key[CRL_AES128_KEY] = {0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74,0x74,0x65,0x73,0x74};

  uint8_t PCToBoardKey[CRL_AES128_KEY] = {0x74,0X65,0X73,0X74,0X67,0X6F,0X6F,0X64,0X74,0X65,0X73,0X74,0X67,0X6F,0X6F,0X64};
//  PCToBoardKey = testgoodtestgood

uint8_t EncryptedMessage[PLAINTEXT_LENGTH * 2];
uint32_t EncryptedMessageLength = 0;



//============ FOR PC TO BOARD ====================
status = STM32_AES_ECB_Encrypt((uint8_t*)Plaintext, sizeof(Plaintext),PCToBoardKey, EncryptedMessage, &EncryptedMessageLength);


if(status == AES_SUCCESS){
	uint8_t EncryptedAscii[EncryptedMessageLength * 2];

	BinaryToAscii(EncryptedMessage, EncryptedMessageLength, EncryptedAscii);

//	Transmit the encrypted message over UART

//	UART_Transmit_ASCII(EncryptedAscii, EncryptedMessageLength*2, &huart3);
//	HAL_UART_Transmit(&huart3, EncryptedMessage, EncryptedMessageLength*2, 30000);


} else {
	Error_Handler();
}
//====================================================================================================


//============ FOR BOARD TO PC ====================
//status = STM32_AES_ECB_Encrypt((uint8_t*)Plaintext, sizeof(Plaintext), Key, EncryptedMessage, &EncryptedMessageLength);
//
//
//if(status == AES_SUCCESS){
//	uint8_t EncryptedAscii[EncryptedMessageLength * 2];
//
//	BinaryToAscii(EncryptedMessage, EncryptedMessageLength, EncryptedAscii);
//
////	Transmit the encrypted message over UART
//
////	UART_Transmit_ASCII(EncryptedAscii, EncryptedMessageLength*2, &huart3);
////	HAL_UART_Transmit(&huart3, EncryptedMessage, EncryptedMessageLength*2, 30000);
//
//
//} else {
//	Error_Handler();
//}

//======================= BOARD TO PC ===================================================
uint8_t DecryptedMessage[EncryptedMessageLength];
uint32_t DecryptedMessageLength = 0;

status = STM32_AES_ECB_Decrypt(EncryptedMessage, EncryptedMessageLength, PCToBoardKey, DecryptedMessage, &DecryptedMessageLength);


if(status == AES_SUCCESS){
	uint8_t DecryptedAscii[DecryptedMessageLength * 2];

	BinaryToAscii(DecryptedMessage, DecryptedMessageLength, DecryptedAscii);

UART_Transmit_ASCII(DecryptedAscii, DecryptedMessageLength*2, &huart3);

//HAL_UART_Transmit(&huart3, DecryptedMessage, DecryptedMessageLength*2, 30000);
} else {
	Error_Handler();
}

//===================================================================================


//============================ BOARD TO PC ============================================
//uint8_t DecryptedMessage[EncryptedMessageLength];
//uint32_t DecryptedMessageLength = 0;
//
//status = STM32_AES_ECB_Decrypt(EncryptedMessage, EncryptedMessageLength, Key, DecryptedMessage, &DecryptedMessageLength);
//
//
//if(status == AES_SUCCESS){
//	uint8_t DecryptedAscii[DecryptedMessageLength * 2];
//
//	BinaryToAscii(DecryptedMessage, DecryptedMessageLength, DecryptedAscii);
//
//UART_Transmit_ASCII(DecryptedAscii, DecryptedMessageLength*2, &huart3);
//
////HAL_UART_Transmit(&huart3, DecryptedMessage, DecryptedMessageLength*2, 30000);
//} else {
//	Error_Handler();
//}

//===================================================================================


//  void HexToAscii(uint8_t* hexData, uint32_t dataSize, uint8_t* asciiData) {
//        for (int i = 0; i < dataSize; i++) {
//            sprintf((char*)(asciiData + (i * 2)), "%02X", hexData[i]);
//        }
//    }
//
//    void UART_Transmit_Hex(uint8_t* hexData, uint32_t dataSize, UART_HandleTypeDef* huart) {
//        uint8_t asciiData[dataSize * 2];
//        HexToAscii(hexData, dataSize, asciiData);
//        HAL_UART_Transmit(huart, asciiData, dataSize * 2, 1000); // Transmit twice the size for ASCII representation
//
//    }

//    status = STM32_AES_ECB_Encrypt((uint8_t*)Plaintext, sizeof(Plaintext), Key, OutputMessage, &OutputMessageLength);
//
//       if (status == AES_SUCCESS)
//       {
//      	 UART_Transmit_Hex(OutputMessage, OutputMessageLength, &huart3);
//       }
//       else
//       {
//           Error_Handler();
//       }


//  status = STM32_AES_ECB_Encrypt((uint8_t*)Plaintext, sizeof(Plaintext), Key, OutputMessage, &OutputMessageLength);
//
//     if (status == AES_SUCCESS)
//     {
////         HAL_UART_Transmit(&huart3, OutputMessage, OutputMessageLength, 30000);
//
//    	 for(int i=0;i<OutputMessageLength;i++){
//    		 uint8_t hexValue = OutputMessage[i];
//    		 uint8_t upperAscii = (hexValue>>4) +(hexValue >> 4 < 10? '0':'A' - 10);
//
//    		 uint8_t lowerAscii = (hexValue & 0x0F)+((hexValue & 0x0F)<10? '0':'A'-10);
//
//    		 HAL_UART_Transmit(&huart3,&upperAscii,1,30000);
//    		 HAL_UART_Transmit(&huart3,&lowerAscii,1,30000);
//    	 }
//     }
//     else
//     {
//         Error_Handler();
//     }

//     status = STM32_AES_ECB_Decrypt( (uint8_t *) Expected_Ciphertext, PLAINTEXT_LENGTH, Key, &OutputMessage,&OutputMessageLength);
//     if(status == AES_SUCCESS){
//    	 HAL_UART_Transmit(&huart3,OutputMessage,OutputMessageLength,30000);
//     } else {
//    	 Error_Handler();
//     }






//   /* Decrypt DATA with AES in ECB mode */
//     status = STM32_AES_ECB_Decrypt( (uint8_t *) Expected_Ciphertext, PLAINTEXT_LENGTH, Key, OutputMessage,
//                                     &OutputMessageLength);
//     if (status == AES_SUCCESS)
//     {
//       if (Buffercmp(Plaintext, OutputMessage, PLAINTEXT_LENGTH) == PASSED)
//       {
//         /* add application traitment in case of AES ECB encryption is passed */
//
//       }
//       else
//       {
//         Error_Handler();
//
//       }
//     }
//     else
//     {
//       /* In case of encryption/decryption not success the possible values of status:
//        * AES_ERR_BAD_CONTEXT, AES_ERR_BAD_PARAMETER, AES_ERR_BAD_INPUT_SIZE, AES_ERR_BAD_OPERATION
//        */
//
//       Error_Handler();
//
//     }

     /* Turn on the green led in case of AES ECB operations are succssfuls*/
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

//
//void UART_Transmit_ASCII(uint8_t *asciiData, uint32_t dataSize, UART_HandleTypeDef* huart) {
//    HAL_UART_Transmit(huart, asciiData, dataSize, 1000);
//}

/* Function to convert binary data to ASCII representation */
//void BinaryToAscii(uint8_t* binaryData, uint32_t dataSize, uint8_t* asciiData) {
//    for (int i = 0; i < dataSize; i++) {
//        sprintf((char*)(asciiData + (i * 2)), "%02X", binaryData[i]);
//    }
//}


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
//int32_t STM32_AES_ECB_Encrypt(uint8_t* InputMessage,
//                              uint32_t InputMessageLength,
//                              uint8_t  *AES128_Key,
//                              uint8_t  *OutputMessage,
//                              uint32_t *OutputMessageLength)
//{
//  AESECBctx_stt AESctx;
//
//  uint32_t error_status = AES_SUCCESS;
//
//  int32_t outputLength = 0;
//
//  /* Set flag field to default value */
//  AESctx.mFlags = E_SK_DEFAULT;
//
//  /* Set key size to 32 (corresponding to AES-256) */
//  AESctx.mKeySize = 32;
//
//  /* Initialize the operation, by passing the key.
//   * Third parameter is NULL because ECB doesn't use any IV */
//  error_status = AES_ECB_Encrypt_Init(&AESctx, AES128_Key, NULL );
//
//  /* check for initialization errors */
//  if (error_status == AES_SUCCESS)
//  {
//    /* Encrypt Data */
//    error_status = AES_ECB_Encrypt_Append(&AESctx,
//                                          InputMessage,
//                                          InputMessageLength,
//                                          OutputMessage,
//                                          &outputLength);
//
//    if (error_status == AES_SUCCESS)
//    {
//      /* Write the number of data written*/
//      *OutputMessageLength = outputLength;
//      /* Do the Finalization */
//      error_status = AES_ECB_Encrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, &outputLength);
//      /* Add data written to the information to be returned */
//      *OutputMessageLength += outputLength;
//    }
//  }
//
//  return error_status;
//}


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
//int32_t STM32_AES_ECB_Decrypt(uint8_t* InputMessage,
//                              uint32_t InputMessageLength,
//                              uint8_t  *AES128_Key,
//                              uint8_t  *OutputMessage,
//                              uint32_t *OutputMessageLength)
//{
//  AESECBctx_stt AESctx;
//
//  uint32_t error_status = AES_SUCCESS;
//
//  int32_t outputLength = 0;
//
//  /* Set flag field to default value */
//  AESctx.mFlags = E_SK_DEFAULT;
//
//  /* Set key size to 32 (corresponding to AES-256) */
//  AESctx.mKeySize = 32;
//
//  /* Initialize the operation, by passing the key.
//   * Third parameter is NULL because ECB doesn't use any IV */
//  error_status = AES_ECB_Decrypt_Init(&AESctx, AES128_Key, NULL );
//
//  /* check for initialization errors */
//  if (error_status == AES_SUCCESS)
//  {
//    /* Decrypt Data */
//    error_status = AES_ECB_Decrypt_Append(&AESctx,
//                                          InputMessage,
//                                          InputMessageLength,
//                                          OutputMessage,
//                                          &outputLength);
//
//    if (error_status == AES_SUCCESS)
//    {
//      /* Write the number of data written*/
//      *OutputMessageLength = outputLength;
//      /* Do the Finalization */
//      error_status = AES_ECB_Decrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, &outputLength);
//      /* Add data written to the information to be returned */
//      *OutputMessageLength += outputLength;
//    }
//  }
//
//  return error_status;
//}


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
  *         where the assert_par	m error has occurred.
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
