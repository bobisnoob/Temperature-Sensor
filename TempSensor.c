/**
  ******************************************************************************
  * File Name     : TempSensor.c
  * PROJECT       : PROG8125 -Assignment #3
  * PROGRAMMER    : Abdelraheem Alkuor (Abdel)
  * FIRST VERSION : 12/Nov/2016
  * Description   : This program displays temperature value with one decimal place on top right
  * 				corner of LCD (LCD1602A) and it illuminates RGB LED color as follows:
  * 				-15.0 C  to  5.0 C  blue
  *				   	  5.1 C  to 15.0 C   yellow
  *					 15.1 C  to 25.0 C   orange
  *					      Above 25.1 C   red
  *
  *	   This is how the program works:
  *	   			______________________________	        FUNCTIONS TO USE
  *   --------->|Get temperature sensor value| uint32_t getTemperatureDigitalValue(void)
  *	  |	        ------------------------------
  *	  |	        			  ||
  *	  |	        			  \/
  *	  |	        ______________________________
  *   |			|Convert temperature digital |
  *   |			|voltage value to temperature| int32_t convert_DtoT(uint32_t temperaturDigitalValue)
  *   |			|digital value               |
  *	  |	        ------------------------------
  *	  |	        			  ||
  *	  |	        			  \/
  *	  |          _____________________________
  *   |			|Convert temperature digital |
  *   |			|value to string format      | void itoa(int, char *, 10)
  *   |			|without negative sign if	 |
  *   |			|there is one				 |
  *   |		    ------------------------------
  *	  |	        			  ||
  *	  |	        			  \/
  *	  |          _____________________________
  *   |			|Find temperature string size| uint8_t strlen(char *)
  *   |			------------------------------
  *	  |	        			  ||
  *   |	        			  \/
  *   |	       _______________________________
  *	  |	       |Convert temperature string to|
  *	  |	       | readable format with decimal| void getTemperatureAnalogValue(uint8_t *size,char *temperatureString);
  *	  |	       -------------------------------
  *	  |	    				  ||
  *   |	        			  \/
  *   |	       _______________________________
  *	  |	       |Round temperature value with |
  *	  |	       |one decimal place			 | void roundTemperatureValue(uint8_t size,char *temperatureString);
  *	  |	       -------------------------------
  *	  |	   				   	  ||
  *   |	           		      \/
  *   |	       _______________________________
  *	  |	       |add negative sign if there is|
  *	  |	       |one 					     | void addTemperatureSign(uint8_t size, char *temperatureValue);
  *	  |	       -------------------------------
  *	  |					   	  ||
  *   |	           		      \/
  *   |	       _______________________________
  *	  |	       |Change RGB LED color		 |
  *	  |	       |according to temperature     | void RGBColorTemperature(char * currentTemperature);
  *	  |	       |value						 |
  *	  |	       -------------------------------
  *	  |					   	  ||
  *   |	           		      \/
  *   |	       _______________________________
  *	  |	       |Print out the temperature    |
  *	  |	       |value on the top right corner| void writeTemperatureValueLCD(uint8_t ColumnPostionLCD, char *temperatureValue)
  *	  |	       |of LCD						 |
  *	  |	       -------------------------------
  *   |	 	 	 	 	 	  ||
  *   |_______________________\/
  *
  *
  ******************************************************************************
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* USER CODE BEGIN Includes */
#include "LCD1602A.h"
#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim15;
UART_HandleTypeDef huart1;

static uint8_t LCDxPostion=0;//Column position of temperature value on LCD
static uint8_t minusSign=0;// Temperature minus sign
static uint8_t indexSecondDecimalDigit=0;// index of second decimal place of temperature value
static uint8_t indexFirstDecimalDigit=0;// index of first decimal place of temperature value
char temperatureSymbol[2]={0xDF,'\0'};// degree Celsius symbol

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */
char temperatureValue[5]={0};
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM15_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void RGBColorMixer(uint8_t redIntensity, uint8_t greenIntensity, uint8_t blueIntensity);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void RGBColorTemperature(char * currentTemperature);
void getTemperatureAnalogValue(uint8_t *size,char *temperatureString);
void roundTemperatureValue(uint8_t size,char *temperatureString);
void addTemperatureSign(uint8_t size, char *temperatureValue);
void writeTemperatureValueLCD(uint8_t ColumnPostionLCD, char *temperatureValue);
int32_t  convert_DtoT(uint32_t temperaturDigitalValue);
uint32_t getTemperatureDigitalValue(void);
/* USER CODE END PFP */


/* USER CODE END 0 */


int main(void)
{

  /* USER CODE BEGIN 1 */
	uint8_t pacManClose[8] = {
								0b01110,
								0b11011,
								0b11111,
								0b11111,
								0b11111,
								0b11111,
								0b01110,
								0b00000
							 };
	uint8_t pacManOpen[8] = {
								0b11110,
								0b01101,
								0b00111,
								0b00011,
								0b00111,
								0b01111,
								0b11110,
								0b00000
							};
	uint8_t customC[8] = {			 //upper small special 'c'
							0b01100,
							0b10010,
							0b10000,
							0b10000,
							0b10010,
							0b01100,
							0b00000,
							0b00000
							};

	char analogRawTemperatureString[6];
	uint8_t temperatureStringSize;
	int32_t TemperatureDigitalValue;


	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */


	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	HAL_Init();
	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_TIM1_Init();
	MX_TIM2_Init();
	MX_TIM8_Init();
	MX_TIM15_Init();
	MX_USART1_UART_Init();
	LCD1602A_init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_ADC_Start(&hadc1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_2);

	//Create PacMan characters and store them on memory location 0 and 1 on LCD memory
	LCD1602A_CreateChar(1, &pacManOpen[0]);
	LCD1602A_CreateChar(0, &pacManClose[0]);
	//Create special upper small 'c' character and store it on location 2 on LCD memory
	LCD1602A_CreateChar(2, &customC[0]);

	HAL_Delay(2000); // waiting for ADC to sample its first value
	/* USER CODE END 2 */

	/* Infinite loop */

	/* USER CODE BEGIN WHILE */
	while (1)
	{
		TemperatureDigitalValue = getTemperatureDigitalValue();//get temperature sensor voltage digital value every 3 seconds.
		TemperatureDigitalValue = convert_DtoT(TemperatureDigitalValue);//convert temperature digital voltage value to temperature digital value
		itoa(TemperatureDigitalValue, analogRawTemperatureString, 10);
		temperatureStringSize = strlen(analogRawTemperatureString);
		getTemperatureAnalogValue(&temperatureStringSize, &analogRawTemperatureString[0]);//convert temperature string to readable format with decimal
		roundTemperatureValue(temperatureStringSize, &analogRawTemperatureString[0]);
		addTemperatureSign(temperatureStringSize, &analogRawTemperatureString[0]);//add minus sign if it exists
		RGBColorTemperature(&analogRawTemperatureString[0]);
		writeTemperatureValueLCD(LCDxPostion, &analogRawTemperatureString[0]);
	}
}

/* USER CODE BEGIN 4 */
/* FUNCTION      : RGBColorMixer
 * DESCRIPTION   : This function mixes the colors of red, green, and blue using PWM generator as follows:
 * 					Red pin   ---> 150ohms --->PA3
 * 					Ground	  ---------------->GND
 *					Green pin ---> 100ohms --->PC9
 *					Blue pin  ---> 100ohms --->PF10
 * PARAMETERS    : redIntensity   : Unsigned 8bit integer (0-255)
 	 	 	 	   greenIntensity : Unsigned 8bit integer (0-255)
 	 	 	 	   blueIntensity  : Unsigned 8bit integer (0-255)
 * RETURNS       : NULL
*/
void RGBColorMixer(uint8_t redIntensity, uint8_t greenIntensity, uint8_t blueIntensity)
{
	__HAL_TIM_SetCompare(&htim2,  TIM_CHANNEL_4, redIntensity);
	__HAL_TIM_SetCompare(&htim8,  TIM_CHANNEL_4, greenIntensity);
	__HAL_TIM_SetCompare(&htim15, TIM_CHANNEL_2, blueIntensity);
}

/* FUNCTION      : convert_DtoT
 * DESCRIPTION   : Convert ADC value to Temperature value without decimal place multiplied by 1000
 * PARAMETERS    : temperaturDigitalValue : ADC value for temperature sensor
 * RETURNS       : NULL
*/

int32_t convert_DtoT(uint32_t temperaturDigitalValue)
{
	uint32_t analogTemperature=0;
	analogTemperature=77*temperaturDigitalValue-50050;
	return analogTemperature;
}

/* FUNCTION      : getTemperatureDigitalValue
 * DESCRIPTION   : This function reads temperature sensor at pin PA0 which is triggered every 3 seconds by Timer1
 * PARAMETERS    : NULL
 * RETURNS       : Temperature digital value from ADC
*/

uint32_t getTemperatureDigitalValue(void)
{
	uint32_t temperatureDigitalValue;
	temperatureDigitalValue=HAL_ADC_GetValue(&hadc1);
	return temperatureDigitalValue;
}

/* FUNCTION      : getTemperatureAnalogValue
 * DESCRIPTION   : This function formats the Temperature Digital value to have decimal format
 * 					without temperature sign
 * PARAMETERS    :
 * 				   uint8_t *size : Temperature string length
 * 				   char *temperatureString : Temperature value in string format after using "convert_DtoT" function
 * RETURNS       : Null
*/
void getTemperatureAnalogValue(uint8_t *size,char *temperatureString)
{
	char temperatureValue[*size];
	uint8_t i=0;
	uint8_t j=0;

	if(temperatureString[0]=='-')// check for minus sign and if so remove and change temperature string size
	{
		minusSign=1;
		for(int i=1;i<*size;i++)//re-arrange the temperature value without minus sign
		{
			temperatureString[i-1]=temperatureString[i];
		}
	*size=*size-1;//new size of analogRawTemperatureString without minus sign
	}
	if(*size>3)
	{

		for(j=0; j<(*size-3); j++)// store the temperature integer value
		{
			temperatureValue[j]=temperatureString[j];

		}
		temperatureValue[j]='.';// place the decimal mark after the temperature integer value
		for(i=(*size+1)-3; i<*size+1; i++)//store decimal numbers after the decimal mark
		{
			temperatureValue[i]=temperatureString[i-1];
		}
		for (i=0;i<*size;i++)
		{
			temperatureString[i]=0;
		}
		strcpy(temperatureString,temperatureValue);
	}
	else
	{
		temperatureValue[0] = '0';
		temperatureValue[1] = '.'; // place the decimal mark after the temperature integer value
		for (i = 2; i < *size + 1; i++) //store decimal numbers after the decimal mark
		{
			temperatureValue[i] = temperatureString[i - 2];
		}
		for (i = 0; i < *size; i++) {
			temperatureString[i] = 0;
		}
		strcpy(temperatureString, temperatureValue);
	}
}


/* FUNCTION      : roundTemperatureValue
 * DESCRIPTION   : This function rounds the temperature string to have only 1 decimal place
 * 					without temperature sign
 * PARAMETERS    :
 * 				   uint8_t *size : Temperature string length
 * 				   char *temperatureString : Temperature string in decimal format but without sign
 * RETURNS       : Null
*/

void roundTemperatureValue(uint8_t size,char *temperatureString)
{
	//Determine the index of the second decimal place
	if (size == 5)
	{
		indexSecondDecimalDigit = 4;
	}
	else if(size==4 || size==3)
	{
		indexSecondDecimalDigit = 3;
	}

	indexFirstDecimalDigit = indexSecondDecimalDigit - 1;// Assign the first decimal place

	if (temperatureString[indexSecondDecimalDigit] > '5')// Check if the second decimal place is > than 5 i.e 19.96
	{													//														  |__index[4]
		if (temperatureString[indexFirstDecimalDigit] != '9')// if first decimal place is not 9 add 1 to it i.e 15.17-->15.2
		{													//													   ||__index[4]
			temperatureString[indexFirstDecimalDigit] =temperatureString[indexFirstDecimalDigit] + 1;//			   |___index[3]
			temperatureString[indexSecondDecimalDigit]='\0';
			temperatureString[indexSecondDecimalDigit+1]='\0';
		}
		else
		{
			temperatureString[indexFirstDecimalDigit] = '0';// change first decimal to 0 i.e 19.96-->19.0
															//                                  ||__index[4]
			if (temperatureString[indexSecondDecimalDigit - 3] == '9')// 					    |___index[3]
			{
				if (size == 5) // add one to 0 index of temperature and change first index to 0 i.e 19.00-->20.0
				{			  					//															||__index[1]
					temperatureString[1] = '0';//															|___index[0]
					temperatureString[0] = temperatureString[0] + 1;
					temperatureString[3] = '0';
					temperatureString[4] = '\0';
					temperatureString[5] = '\0';
				}//special case when value is 9.9x where x>5
				else if (size==4)//add one to 0 index of temperature and change first index to 0 i.e 9.96-->10.0
				{								//															||__index[1]
					temperatureString[0] = '1';//															|___index[0]
					temperatureString[1] = '0';
					temperatureString[2] = '.';
					temperatureString[3] = '0';
					temperatureString[4] = '\0';
					temperatureString[5] = '\0';
					indexSecondDecimalDigit=4;
				}
			}//special case when value is 0.9x where x>5
			else if ((size == 3) && (temperatureString[indexSecondDecimalDigit - 3] == '0') )//add one to 0 index of temperature and change second index to 0 i.e 0.96-->1.0
			{																				//															          | |__index[2]
				temperatureString[0] = '1';													//																	  |____index[0]
				temperatureString[1] = '.';
				temperatureString[2] = '0';
				temperatureString[3] = '\0';
				temperatureString[4] = '\0';
				temperatureString[5] = '\0';
			}

			else
			{	// if 0 index is less than 9
				temperatureString[indexSecondDecimalDigit - 3] = temperatureString[indexSecondDecimalDigit - 3] + 1;// add one to 0 index  i.e   1.96-->2.0
				temperatureString[indexSecondDecimalDigit] = '\0';											     	//	         				 |__index[0]
				temperatureString[indexSecondDecimalDigit+1] = '\0';
			}
		}
	}
	else // if indexSecondDecimalDigit <= 5 then all decimal places are set to NULL except first decimal place i.e 1.75-->1.7
	{
		temperatureString[indexSecondDecimalDigit] = '\0';
		temperatureString[indexSecondDecimalDigit+1] = '\0';
	}

}

/* FUNCTION      : addTemperatureSign
 * DESCRIPTION   : This function adds '-' sign to temperature if there is one and set the proper column position
 * 				   for LCD
 * PARAMETERS    :
 * 				   uint8_t *size : Temperature string length
 * 				   char *temperatureString : Temperature string decimal format
 * RETURNS       : Null
*/

void addTemperatureSign(uint8_t size, char *temperatureValue)
{

	if (minusSign == 1)
	{
		size = size + 1;
		while (size != 0)
		{
			temperatureValue[size] = temperatureValue[size - 1]; //shift Temperature string array to the right by 1
		size--;
		}
		temperatureValue[0] = '-';
		minusSign = 0;
		LCDxPostion=9;
	}
	else
	{
		LCDxPostion=10;
	}
}

/* FUNCTION      : writeTemperatureValueLCD
 * DESCRIPTION   : This function print out the temperature value in Celsius degree
 * PARAMETERS    :
 * 				   uint8_t ColumnPostionLCD : Temperature value position on LCD
 * 				   char *temperatureValue : processed temperature value in one decimal format
 * RETURNS       : Null
*/
void writeTemperatureValueLCD(uint8_t ColumnPostionLCD, char *temperatureValue)
{

	uint8_t i;
	LCD1602A_Print(0, 3,"Temp.: ");
	if (LCDxPostion == 9)
	{
		LCD1602A_Print(0, ColumnPostionLCD, temperatureValue);
		LCD1602A_Print(0, ColumnPostionLCD + indexSecondDecimalDigit + 1, temperatureSymbol);
		LCD1602A_PrintCustom(0, ColumnPostionLCD + indexSecondDecimalDigit + 2, 2);
		if (ColumnPostionLCD + indexSecondDecimalDigit + 2 == 14)
		{
			LCD1602A_Print(0, 3 + ColumnPostionLCD + indexSecondDecimalDigit, " \0");
		}
		HAL_Delay(1000);

		for (i = ColumnPostionLCD + indexSecondDecimalDigit + 2; i > 9; i--) // pacMan animation
		{
			LCD1602A_PrintCustom(0, i, 0);
			HAL_Delay(300);
			LCD1602A_Print(0, i, " \0");
			LCD1602A_PrintCustom(0, i - 1, 1);
			HAL_Delay(300);
		}
		LCD1602A_Print(0, i, " \0");

	}
	else
	{
		LCD1602A_Print(0, ColumnPostionLCD, temperatureValue);
		LCD1602A_Print(0, ColumnPostionLCD + indexSecondDecimalDigit, temperatureSymbol);
		LCD1602A_PrintCustom(0, ColumnPostionLCD + indexSecondDecimalDigit + 1, 2);
		if (ColumnPostionLCD + indexSecondDecimalDigit + 1 == 14)
		{
			LCD1602A_Print(0, 2 + ColumnPostionLCD + indexSecondDecimalDigit, " \0");
		}
		HAL_Delay(1000);

		for (i = ColumnPostionLCD + indexSecondDecimalDigit + 1; i > 9; i--)// pacMan animation
		{
			LCD1602A_PrintCustom(0, i, 0);
			HAL_Delay(150);
			LCD1602A_Print(0, i, " \0");
			LCD1602A_PrintCustom(0, i - 1, 1);
			HAL_Delay(150);
		}
		LCD1602A_Print(0, i, " \0");
	}


}

/* FUNCTION      :RGBColorTemperature
 * DESCRIPTION   : This function changes RGB LED color according to the temperature value
 * PARAMETERS    :
 * 				  char *currentTemperature : processed temperature value in one decimal format
 * RETURNS       : Null
*/

void RGBColorTemperature(char * currentTemperature)

{
	float temperatureInFloat;
	temperatureInFloat =  atof(currentTemperature);

 	if((temperatureInFloat>=-15.0 ) && (temperatureInFloat<=5.0) )
	{
		RGBColorMixer(0, 0, 255);// blue color
	}

	else if(strcmp(currentTemperature,"5.1")==0)// Used string comparison for equal case due to the inaccuracy of float conversion of 5.1
 	{
		RGBColorMixer(255, 120, 0);// yellow color
 	}
	else if((temperatureInFloat>5.1) && (temperatureInFloat<=15.0))
	{
		RGBColorMixer(255, 120, 0);//yellow color
	}

	else if(strcmp(currentTemperature,"15.1")==0)// Used string comparison for equal case due to the inaccuracy of float conversion of 15.1
	{
		RGBColorMixer(255, 50, 0);// orange color
	}
	else if((temperatureInFloat>15.1) && (temperatureInFloat<=25.0))
	{
		RGBColorMixer(255, 50, 0);//orange color
	}
	else if (strcmp(currentTemperature, "25.1") == 0) // Used string comparison for equal case due to the inaccuracy of float conversion of 25.1
	{
		RGBColorMixer(255, 0, 0);//red color
	}
	else if((temperatureInFloat>25.1))
	{
		RGBColorMixer(255, 0, 0);//red color
	}
	else
	{
		RGBColorMixer(0, 0, 0);
	}
}
/* USER CODE END 4 */
/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_TIM8;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_61CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 36000;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 6000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 254;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim2);

}

/* TIM8 init function */
static void MX_TIM8_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 72;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 254;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim8);

}

/* TIM15 init function */
static void MX_TIM15_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 72;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 254;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim15);

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PF2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PE11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
