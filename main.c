/**
  ******************************************************************************
  * @file    Timer/Firmware/080615/SRC/main.c 
  * @author  Kotmyshev E.V. Samara RUS
  * @version V0.5.1
  * @date    08-June-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#define RTC_TR_RESERVED_MASK    ((uint32_t)0x007F7F7F)
#define RTC_DR_RESERVED_MASK    ((uint32_t)0x00FFFF3F)

#define COMAND	    		(GPIOA->BSRRH |= GPIO_BSRR_BS_8); 
#define WRDATA	    		(GPIOA->BSRRL |= GPIO_BSRR_BS_8);
#define LEDON	    		  (GPIOB->BSRRL |= GPIO_BSRR_BS_14); 
#define LEDOFF    		  (GPIOB->BSRRH |= GPIO_BSRR_BS_14);
	
	unsigned int i;							// very important value :-)
		

unsigned char SRC = 0; 			// RTC Clock Source:  0 - Error, 1 - LSE, 2 - LSI, 3 - HSE
unsigned char MENU = 0; 		// Current Menu Mode: 0 - Current time, 1 - Time Setup, 2 - Manual Act Setup, 3 - Prog View
unsigned char PGPN = 1;			// Pointer of Array Program 
unsigned char STPG = 0; 		// Programm Setup Menu Mode: 0 - View Only, 1 - Hour, 2 - Min, 3 - ActHour, 4 - ActMin, 5 - ActDays
unsigned char STNU = 0; 		// Current Setup Menu Mode: 1 - Setup Hour, 2 - Setup Min 3 - Setup Day 4 - Act Hour 5 - Act Min
unsigned char MOST = 0;			// Motor State: 0 - OFF, 1 - ON (Manual)
unsigned char MSTP = 0;			// Motor Activision: 0 - by Manual, 1 - by PRG
unsigned char WAFL = 0;			// Wait Flag:  0 - Not Wait, 1 - Wait
unsigned char BLKN = 0;			// Blinking Flag

char HRT = 0; char HRU = 0; char MNT = 0; char MNU = 0; char SCT = 0; char SCU = 0; // Current Time Values
char DAY = 0; char HR = 0; char MN = 0;
char NHRT = 0; char NHRU = 0; char NMNT = 0; char NMNU = 0;													// New Time Values
char NDAY = 0;  char NHR = 0;  char NMN = 0;

char MHR = 0; char MMN = 0; char MSC = 0;	char MMNT = 0;		char MMNU = 0;	// Manual State -> Act Time

//char YET = 0; char YEU = 0; char MOT = 0; char MOU = 0; char DAT = 0; char DAU = 0;  // Current date
	
char PGST [17]; char PGHR [17]; char PGMN [17]; char PAHR [17]; char PAMN [17]; char PGDY [17]; char PADY [17]; char PGKL = 0; 

/******************************************************/
 void simpleDelay(unsigned int delayTime)
{
    for (i = 0; i < delayTime; i++);
}


void SEND (unsigned char SDAT	)								// WH0802A Display Write Function with strobe
{	
GPIOA->ODR |= SDAT;									// WR DATA to ODR Registr Port A
	
simpleDelay(10);										
GPIOA->BSRRL |= GPIO_BSRR_BS_10;		// Generation STROB signal on PA10
simpleDelay(10);
GPIOA->BSRRH |= GPIO_BSRR_BS_10;	
simpleDelay(40);
	
GPIOA->ODR &= ~0xFF;								// Reset Data Bits (PA0...PA7)
}

char ConvertD ( unsigned int Zifra) 					// Convert Decimal 0-9 Number into WH0802A CODE
 {
		char DIG = 0x00;
	 
	  if (Zifra > 9) Zifra = 9;
	  if (Zifra < 1) Zifra = 0;
		if (Zifra == 0) DIG = 0x30;
		if (Zifra == 1) DIG = 0x31;
	 	if (Zifra == 2) DIG = 0x32;
	 	if (Zifra == 3) DIG = 0x33;
	 	if (Zifra == 4) DIG = 0x34;
	 	if (Zifra == 5) DIG = 0x35;
	 	if (Zifra == 6) DIG = 0x36;
	 	if (Zifra == 7) DIG = 0x37;
	 	if (Zifra == 8) DIG = 0x38;
	 	if (Zifra == 9) DIG = 0x39;
		
	 return DIG;
}
 
char ConvDayA ( unsigned int Bukva) 					// Convert Decimal 0-7 Number into First Symbol Day's Name (WH0802A CODE)
{
	
char DAYA = 0x00;
	 
	  if (Bukva > 9) Bukva = 0;
	  if (Bukva < 1) Bukva = 0;
		if (Bukva == 0) DAYA = 0x4B;		// EveryDay
		if (Bukva == 1) DAYA = 0xA8;		// Ponedelnik
	 	if (Bukva == 2) DAYA = 0x42;		// Vtornik
	 	if (Bukva == 3) DAYA = 0x43;		// CPEDA
	 	if (Bukva == 4) DAYA = 0xAB;		// 4etverg
	 	if (Bukva == 5) DAYA = 0xA8;		// Pyatniza
	 	if (Bukva == 6) DAYA = 0x43;		// Subbota
	 	if (Bukva == 7) DAYA = 0x42;		// Voskresenie
		if (Bukva == 8) DAYA = 0x32;		// Every 2-nd	day
		if (Bukva == 9) DAYA = 0x33;		// Every 3-th	day

	 return DAYA;

}


char ConvDayB ( unsigned int Bukva) 					// Convert Decimal 0-7 Number into Second Symbol Day's Name (WH0802A CODE)
{
	
char DAYA = 0x00;
	 
	  if (Bukva > 9) Bukva = 0;
	  if (Bukva < 1) Bukva = 0;
		if (Bukva == 0) DAYA = 0xE0;		// EveryDay
		if (Bukva == 1) DAYA = 0x48;		// Ponedelnik
	 	if (Bukva == 2) DAYA = 0x54;		// Vtornik
	 	if (Bukva == 3) DAYA = 0x50;		// CPEDA
	 	if (Bukva == 4) DAYA = 0x54;		// 4etverg
	 	if (Bukva == 5) DAYA = 0x54;		// Pyatniza
	 	if (Bukva == 6) DAYA = 0xA0;		// Subbota
	 	if (Bukva == 7) DAYA = 0x43;		// Voskresenie
		if (Bukva == 8) DAYA = 0xA6;		// Every 2-nd day
		if (Bukva == 9) DAYA = 0xA6;		// Every 3-th	day
	
	 return DAYA;

}
void DisplayStart()														// WH0802A Display Initialization
{

	GPIOA->BSRRH |= GPIO_BSRR_BS_9; // Pre-Setup Write Only State
	GPIOB->BSRRL |= GPIO_BSRR_BS_1; //  5.0V Enable (NCP1400A Power On)
	simpleDelay(700000); 						// Display Start Time >20 ms
	COMAND; 												
	SEND(0x38) ; 										// Function Set: 8-bit, 2-line
	simpleDelay(1500);							// Wait >37 us
	SEND(0x0C) ; 										// Display Set (No Cursor & No Blinking)
	simpleDelay(1500);							// Wait >37 us
	SEND(0x6) ; 										// Entery Mode Set (Right-Moving Cursor)
	simpleDelay(1500);							// Wait >37 us
	SEND(0x1) ; 										// Display Clear
	simpleDelay(51000);							// Wait >1.53 ms
	
}

void WaitMsg()																// Print Starting Message
{
COMAND; 
SEND(0x80) ; // First Line
WRDATA;			 // Write Text RUS "ZAPUSK"
SEND(0x20); SEND(0xA4); SEND(0x41); SEND(0xA8); SEND(0xA9); SEND(0x43); SEND(0x4B); SEND(0x20); 
COMAND; 
SEND(0xC0) ; // 2-nd line
WRDATA;
}

void ClockEn ()																// Clock Enable, Default Time Setup
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;					// Power Interface Clock Enable
	PWR->CR |= PWR_CR_DBP; 											// Access to RTC, RTC Backup and RCC CSR registers enabled
	RCC->CSR &= ~RCC_CSR_RTCRST; 								// Reset not activated

	RCC->CSR |= RCC_CSR_LSEON; 									// LSE Enabled
	
	for (i = 0; (i < 10000000) & ((RCC->CSR & RCC_CSR_LSERDY) == 0 ); i++);						// Wait > 4 s
	
	if ((RCC->CSR & RCC_CSR_LSERDY) == 0 )
	{
	RCC->CSR &= ~RCC_CSR_LSEON; 								// LSE Disabled
	RCC->CSR |= RCC_CSR_LSION; 									// LSI Enabled
	while( (RCC->CSR & RCC_CSR_LSIRDY) == 0 )		// Wait LSI
	RCC->CSR |= 0x20000; 												// LSI is source clock for RTC
	SRC = 2;
	}
	else																			// LSE is Ready
	{
	RCC->CSR &= ~RCC_CSR_LSION; 							// LSI Disabled
	RCC->CSR |= 0x10000; 											// LSE is source clock for RTC
	SRC = 1;
	}
		

	RCC->CSR |= RCC_CSR_RTCEN;									// RTC Enabled
																							// Disable the RTC registers Write protection
	RTC->WPR = 0xCA;														// Entering security Key 1
	RTC->WPR = 0x53;														// Entering security Key 2
	RTC->ISR |= RTC_ISR_INIT;										// Enter Initialization mode
	while( (RTC->ISR & RTC_ISR_INITF) == 0 ) ; 	// Wait for the confirmation of Initialization mode
	
	if (SRC == 1) RTC->PRER = 0x007F00FF;				// RTC prescaler register for 32768 Hz
	if (SRC == 2) RTC->PRER = 0x007C0295;				// RTC prescaler register for 37000 Hz)
	
	//RTC->TR |= 0x0;															// Default Time 00:00:00
	//RTC->DR |= 0xE101; 													// Default Day Sunday ( 01 Month, 01 Day, 00 Year)
	RTC->CR &= ~RTC_CR_FMT;											// FMT=0: 24 hour/day format
	RTC->ISR &= ~RTC_ISR_INIT;									// Exit Initialization mode
	RTC->WPR = 0xFF;														// Enable the RTC Registers Write Protection
	
	SEND(0x20);	SEND(0x4C); SEND(0x53);					// Print Message about LSE or LSI
	if (SRC == 1) SEND(0x45);
	if (SRC == 2) SEND(0x49);
	SEND(0x20); SEND(0x4F); SEND(0x4B); SEND(0x20);
	
	simpleDelay(3200000);
}

void ClockSetup() 														// Setup New Time in RTC
{
	unsigned int tmpreg = 0x0;  unsigned int tmpred = 0x101;
	
	// Disable the RTC registers Write protection
	RTC->WPR = 0xCA;														// Entering security Key 1
	RTC->WPR = 0x53;														// Entering security Key 2
	RTC->ISR |= RTC_ISR_INIT;										// Enter Initialization mode
	while( (RTC->ISR & RTC_ISR_INITF) == 0 ) ; 	// Wait for the confirmation of Initialization mode

tmpreg = (((NHRT)<<20) | ((NHRU)<<16) | ((NMNT)<<12) | ((NMNU)<<8) | ((SCT)<<4) | (SCU));

 /* Set the RTC_TR register */
    RTC->TR = (uint32_t)(tmpreg & RTC_TR_RESERVED_MASK); 
	
tmpreg = (((NDAY)<<13) | (tmpred));	
	
 /* Set the RTC_DR register */
    RTC->DR = (uint32_t)(tmpreg & RTC_DR_RESERVED_MASK);

	RTC->ISR &= ~RTC_ISR_INIT;									// Exit Initialization mode
	RTC->WPR = 0xFF;														// Enable the RTC Registers Write Protection
}

void InitIRQ()																// Initialization IRQs
{
//  IRQ for buttons
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable clock for IRQ
	EXTI->IMR |= 0xFF;  									// Exti interrupt mask register set for 0...7 lines 
	EXTI->RTSR |= 0xFF;										// Rising Edge trigger enabled for 0...7 lines 
	SYSCFG->EXTICR[0] |= 0x2222;					// Exti lines 0...3 set on PORT C
	SYSCFG->EXTICR[1] |= 0x2222;					// Exti lines 4...7 set on PORT C
	
	NVIC_EnableIRQ(EXTI0_IRQn);						// Enable IRQ for PC0 (Button S1 - "UP" )
	NVIC_EnableIRQ(EXTI1_IRQn);						// Enable IRQ for PC1 (Button S2 - "DOWN" )
	NVIC_EnableIRQ(EXTI2_IRQn);						// Enable IRQ for PC2 (Button S3 - "ENTER" )
	NVIC_EnableIRQ(EXTI3_IRQn);						// Enable IRQ for PC3 (Button S4 - "CURR. TIME" )
	NVIC_EnableIRQ(EXTI4_IRQn);						// Enable IRQ for PC4 (Button S5 - "TIME SETUP" )
	NVIC_EnableIRQ(EXTI9_5_IRQn);					// Enable IRQ for PC5, PC6, PC7 (Buttons S6,S7,S8 : PROG, PW-ON, PW-OFF)
}
void InitPh()																	// GPIO & Timers  Initialization
{
	
	// GPIO SETUP
	RCC->AHBENR |= 0x7; 					// All Gpio (ABC) Clock Enable
	GPIOA->MODER |= 0x155555; 		// PortA Pins 0...10 are GP Out mode
	GPIOA->OSPEEDR |= 0xFFFFFFFF; // PortA Max Speed
	GPIOB->MODER |= 0x15000004; 	// PortB Pins 1,12,13,14 are GP Out mode
	GPIOB->OSPEEDR |= 0xFFFFFFFF; // PortB Max Speed
	
	GPIOC->MODER &= ~0xFF;				// PortC 0...7 bits - Input (Reset State)
	GPIOC->PUPDR |= 0xAAAA;				// PortC 0...7 bits - Pull-down
	
	// TIMERS SETUP 
	RCC->APB1ENR |= 0x03	;				// TIM2, TIM3  Clock Enable
	// TIM2
	TIM2->PSC = 32000-1		; 			// Set TIM2 prescaler 
	TIM2->CNT = 1;								// Initial TIM2 value
	TIM2->ARR = 240;							// Set Auto-reload value
	TIM2->DIER |= TIM_DIER_UIE; 	// TIM2 Update IRQ Enable
	NVIC_EnableIRQ(TIM2_IRQn);		// TIM2 NVIC IRQ Enable
	// TIM3
	TIM3->PSC = 32000-1		; 			// Set TIM3 prescaler 
	TIM3->CNT = 1;								// Initial TIM3 value
	TIM3->ARR = 400;							// Set Auto-reload value
	TIM3->DIER |= TIM_DIER_UIE; 	// TIM3 Update IRQ Enable
	NVIC_EnableIRQ(TIM3_IRQn);		// TIM3 NVIC IRQ Enable
	
	
}


void ViewFullTime ()													// Write Current Time, Day & Programs State
{
COMAND; 
SEND(0x80) ; 			//  Cursor in 1-st line
WRDATA;
		SEND(ConvertD(HRT));	SEND(ConvertD(HRU));  SEND(0x3A);	// Print Hours and Separator (:)
		SEND(ConvertD(MNT));  SEND(ConvertD(MNU));  SEND(0x3A);	// Print Minutes and Separator (:)
		SEND(ConvertD(SCT));  SEND(ConvertD(SCU));							// Print Seconds
COMAND; 
SEND(0xC0) ;		 // Cursor in 2-nd line
WRDATA;

if (MOST == 0) 
{
		if (PGKL == 0) 
		{
		SEND(0x20); SEND(0x20); SEND(0x20); SEND(0x20); SEND(0x20); SEND(0x20);
		}
		else
		{
		char PGKT = 0; char PGKU = 0;	
		PGKT = PGKL/10; PGKU = PGKL - (PGKT*10);	
		SEND(0xA8); SEND(0x70); SEND(0x3A);
		if (PGKT == 0) { SEND(ConvertD(PGKU)); SEND(0x20); } else {SEND(ConvertD(PGKT)); SEND(ConvertD(PGKU)); } SEND(0x20);
		}
}
else
{
		char MMNT = MMN/10; char MMNU = MMN-(MMNT*10) ; 
		if (MSTP > 0) {SEND(0xBE);} else {SEND(0x70); }
		SEND(ConvertD(MHR)); SEND(0x3A); SEND(ConvertD(MMNT)); SEND(ConvertD(MMNU)); SEND(0x20);
}	
		SEND(ConvDayA(DAY));  SEND(ConvDayB(DAY));  
}

void ViewTimeSetup ()													// Write Time Setup Menu
{
				NHRT = NHR/10;
				NHRU = NHR-(NHRT*10);
				NMNT = NMN/10;
				NMNU = NMN-(NMNT*10);
	
				COMAND; 
				SEND(0xC0);
				WRDATA;
				SEND (ConvertD(NHRT)); SEND (ConvertD(NHRU)); SEND (0x3A); 
				SEND (ConvertD(NMNT)); SEND (ConvertD(NMNU)); SEND (0x20);
				SEND (ConvDayA(NDAY)); SEND (ConvDayB(NDAY));
}

void ViewActSetup ()													// Write Manual Act Time	
{
	
COMAND; 
SEND(0xC0);
WRDATA; 			
MMNT = MMN/10; 
MMNU = MMN-(MMNT*10) ; 
	
SEND(0x20); SEND(ConvertD(MHR)); SEND(0x3A); SEND(ConvertD(MMNT)); SEND(ConvertD(MMNU)); SEND(0x20);SEND(0x20);SEND(0x20);

}

void ViewProgMenu ()													// Write Prog Menu
{
char PNT = 0; char PNU = 0; char PGHT = 0; char PGHU = 0; char PGMT = 0; char PGMU = 0; char PAMT = 0; char PAMU = 0;
COMAND; 
SEND(0x80);
WRDATA; 
PNT = PGPN/10; PNU = PGPN - (PNT*10); 
SEND(ConvertD(PNT)); SEND(ConvertD(PNU));
if (PGST[PGPN] == 1) SEND(0x2B); if (PGST[PGPN] == 0) SEND(0x2D); SEND(0x20);
PAMT = PAMN[PGPN]/10; PAMU = PAMN[PGPN] - (PAMT*10);	
SEND(ConvertD(PAHR[PGPN])); SEND(0x3A); SEND(ConvertD(PAMT)); SEND(ConvertD(PAMU));
COMAND; 
SEND(0xC0);
WRDATA;
PGHT = PGHR[PGPN]/10; PGHU = PGHR[PGPN] - (PGHT*10); PGMT = PGMN[PGPN]/10; PGMU = PGMN[PGPN] - (PGMT*10);
SEND(ConvertD(PGHT)); SEND(ConvertD(PGHU)); SEND(0x3A); SEND(ConvertD(PGMT)); SEND(ConvertD(PGMU)); SEND(0x20);
SEND (ConvDayA(PGDY[PGPN])); SEND (ConvDayB(PGDY[PGPN]));
	
}


int main(void)																// Main Program Body & Infinite loop
{
  InitPh();																		// GPIO A B C Enabled
	DisplayStart(); 														// Initialization of WH0802A
	WaitMsg();																	// Print Starting Message
	ClockEn ();																	// Clock Enable (LSE or LSI for RTC)
	InitIRQ();
	SRC = 0;
	MENU = 0;

  /* Infinite loop */
  while (1)
  {
	if (MOST == 1) LEDON;
	if (MOST == 0) LEDOFF;	
	if ((MHR == 0) & (MMN == 0)) MOST = 0;	

			if (SCU != ((RTC->TR & RTC_TR_RESERVED_MASK) & RTC_TR_SU)) 	// If Seconds changed
			{
	{																																// Read RTC Time & Data 
	uint32_t tmpreg = 0; uint32_t tmpred = 0;
	tmpreg = (RTC->TR & RTC_TR_RESERVED_MASK);
	tmpred = (RTC->DR & RTC_DR_RESERVED_MASK);
	
	HRT = ((tmpreg & RTC_TR_HT) >>20);
	HRU = ((tmpreg & RTC_TR_HU) >>16);
	MNT = ((tmpreg & RTC_TR_MNT) >>12);
	MNU = ((tmpreg & RTC_TR_MNU) >>8);
	SCT = ((tmpreg & RTC_TR_ST) >>4);
	SCU = (tmpreg & RTC_TR_SU);
	
	//YET = ((tmpred & RTC_DR_YT) >>20);
	//YEU = ((tmpred & RTC_DR_YU) >>16);
	DAY = ((tmpred & RTC_DR_WDU) >>13);	
	//MOT = ((tmpred & RTC_DR_MT) >>12);
	//MOU = ((tmpred & RTC_DR_MU) >>8);
	//DAT = ((tmpred & RTC_DR_DT) >>4);
	//DAU =  (tmpred & RTC_DR_DU);
	
	HR = HRT*10+HRU; MN = MNT*10+MNU; 
	}
				
				if (MENU == 0) ViewFullTime ();
				if (MOST == 1) 																						// Countdown  of actuator work
				{
					if ((MSC == 0) & (MMN == 0) & (MHR > 0)) MHR--;					
					if ((MSC == 0) & (MMN == 0) & (MHR > 0)) MMN = 60;
					if ((MSC == 0) & (MMN > 0)) MMN--;
					if ((MSC == 0) & (MMN > 0)) MSC = 60;	
					if (MSC > 0) MSC--;
				}
				if ((PGKL > 0) & (MENU != 3))
						{
							char n = 1;
							while ( n < 17 )
							{
							if ((PGST[n] == 1) & (PGHR[n] == HR) & (PGMN[n] == MN))
								{
										if ((PGDY[n] == 0) | (PGDY[n] == DAY) | (PADY[n] == DAY))
											{
												if (PGDY[n] > 7) 
												{
												PADY[n] = DAY + (PGDY[n]-6);
												if (PADY[n] > 7) PADY[n] = PADY[n] - 7;
												}
												if (MOST == 0) 
												{
													MHR = PAHR[n]; MMN = PAMN[n]; MSC = 60;
													MSTP = n;
													MOST = 1;
													
												}
											}	
											
								}
								
							n++;
							}
							
							
						}
			}
  }
}





void EXTI0_IRQHandler(void) 									// Button "UP" IRQ
{
if (WAFL == 0)
{
	TIM2->CNT = 1;
	TIM2->CR1 |= TIM_CR1_CEN; // TIM2 COUNT ON
	WAFL = 1;
	
	
	if (MENU == 1) 						// Time Setup Mode
	{
		
		switch (STNU)
		{
			case 1:
			{
				
				if (NHR <= 23) NHR++;
				if (NHR >= 24) NHR = 0;
				break;
				
			}
			case 2:
			{
				if (NMN <= 59) NMN++;
				if (NMN >= 60) NMN = 0;			
				break;
	
			}
			case 3:
			{
				if (NDAY <= 7) NDAY++;
				if (NDAY >= 8) NDAY = 1;
				break;
			
			}
		}
		
	ViewTimeSetup ();
	
	}
	if (MENU == 2) 						// Manual Act Mode
		{
			switch (STNU)
				{
					case 4 :
					{
						if (MHR <= 9) 	MHR++;
						if (MHR >= 10) 	MHR = 0;
						break;
					}
					case 5 :
					{
						if (MMN <= 59) MMN++;
						if (MMN >= 60) MMN = 0;	
						break;
					}
				}
		ViewActSetup ();
		}
	


	if (MENU == 3) 						// Programm Mode
	{
		
		switch (STPG)
		{
			case 0:
			{
				
				if (PGPN <= 16) PGPN++;
				if (PGPN >= 17) PGPN = 1;
				break;
				
			}
			
			case 1:
			{
				
				if (PGHR[PGPN] <= 23) PGHR[PGPN]++;
				if (PGHR[PGPN] >= 24) PGHR[PGPN] = 0;
				break;
				
			}
			case 2:
			{
				if (PGMN[PGPN] <= 59) PGMN[PGPN]++;
				if (PGMN[PGPN] >= 60) PGMN[PGPN] = 0;			
				break;
	
			}
			case 3:
			{
				if (PAHR[PGPN] <= 9) 	PAHR[PGPN]++;
				if (PAHR[PGPN] >= 10) PAHR[PGPN] = 0;
				break;
			
			}
			case 4:
			{
				if (PAMN[PGPN] <= 59) PAMN[PGPN]++;
				if (PAMN[PGPN] >= 60) PAMN[PGPN] = 0;			
				break;
	
			}
			case 5:
			{
				if (PGDY[PGPN] <= 9) PGDY[PGPN]++;
				if (PGDY[PGPN] >= 10) PGDY[PGPN] = 0;			
				break;
	
			}
			
		}
		
	ViewProgMenu ();
	
	}
}	
	EXTI->PR |= 0x01;  // Reset Pending Bit	
}

void EXTI1_IRQHandler(void) 									// Button "DOWN" IRQ
{
	
if (WAFL == 0)
{
	TIM2->CNT = 1;
	TIM2->CR1 |= TIM_CR1_CEN; // TIM2 COUNT ON
	WAFL = 1;
	
		
	if (MENU == 1)  						// Time Setup Mode
	{
		
		switch (STNU)
		{
			case 1:
			{
				
				if (NHR == 0) NHR = 24;
				if (NHR > 0) NHR--;
				break;
				
			}
			case 2:
			{
				if (NMN == 0) NMN = 60;
				if (NMN > 0) NMN--;			
				break;
	
			}
			case 3:
			{
				if (NDAY <= 1) NDAY = 8;
				if (NDAY > 1) NDAY--;
				break;
			
			}
		}
		
	ViewTimeSetup ();
	
	}
	
	if (MENU == 2) 							// Manual Act Mode
		{
			switch (STNU)
				{
					case 4 :
					{
						if (MHR == 0) MHR = 10;
						if (MHR > 0) 	MHR--;
						break;
					}
					case 5 :
					{
						if (MMN == 0) MMN = 60;
						if (MMN > 0) 	MMN--;
						break;
					}
				}
		ViewActSetup ();
		}
	
	if (MENU == 3) 						// Programm Mode
	{
		
		switch (STPG)
		{
			case 0:
			{
				
				if (PGPN == 1) PGPN = 17;
				if (PGPN > 1) PGPN--;
				break;
				
			}
			
			case 1:
			{
				
				if (PGHR[PGPN] == 0) PGHR[PGPN] = 24;
				if (PGHR[PGPN] > 0) PGHR[PGPN]--;
				break;
				
			}
			case 2:
			{
				if (PGMN[PGPN] == 0) PGMN[PGPN] = 60;
				if (PGMN[PGPN] > 0) PGMN[PGPN]--;			
				break;
	
			}
			case 3:
			{
				if (PAHR[PGPN] == 0) 	PAHR[PGPN] = 10;
				if (PAHR[PGPN] > 0) PAHR[PGPN]-- ;
				break;
			
			}
			case 4:
			{
				if (PAMN[PGPN] == 0) PAMN[PGPN] = 60;
				if (PAMN[PGPN] > 0) PAMN[PGPN]--;			
				break;
	
			}
			case 5:
			{
				if (PGDY[PGPN] == 0) PGDY[PGPN] = 10;
				if (PGDY[PGPN] > 0) PGDY[PGPN]--;			
				break;
	
			}
			
		}
		
	ViewProgMenu ();
	
	}		
			
}	
	EXTI->PR |= 0x02; // Reset Pending Bit
}

void EXTI2_IRQHandler(void)										// Button "ENTER" IRQ
{
if (WAFL == 0)
{
	TIM2->CNT = 1;
	TIM2->CR1 |= TIM_CR1_CEN; // TIM2 COUNT ON
	WAFL = 1;
	
		
if (MENU == 1)  						// Time Setup Mode
	{
		
		switch (STNU)
		{
			case 3:													// Finish Time Setup
			{
				ViewTimeSetup ();
				ClockSetup();									// Send New Time Into RTC
				MENU = 0;											// Back to Current Time Menu Mode
				TIM3->CR1 &= ~TIM_CR1_CEN;		// STOP TIM3 - Stop Blinking
				break;
				
			}
			case 2:													// Next -> Day Setup
			{
				STNU = 3;	
				ViewTimeSetup ();
				break;
	
			}
			case 1:													// Next -> Min Setup
			{
				STNU = 2;
				ViewTimeSetup ();
				break;
			
			}
		}
		
	
	}
if (MENU == 2) 							// Manual Act Mode
		{
			switch (STNU)
				{
					case 4 :
					{
						STNU = 5;
						ViewActSetup ();
						break;
					}
					case 5 :
					{
						TIM3->CR1 &= ~TIM_CR1_CEN;			// STOP TIM3 - Stop Blinking
						MENU = 0;			STNU = 0;					// Back to Current Time Menu Mode
						MSC = 59; 											// 59 Seconds countdown
						MOST = 1;												// Actuator ON
						MSTP = 0;
						ViewActSetup ();
						break;
					}
				}
		}
		
	if (MENU == 3) 						// Programm Mode
	{
	if (STPG == 5)
	{
	PGST[PGPN] = 1;	
	if ((PAHR[PGPN] == 0) & (PAMN[PGPN] == 0)) PGST[PGPN] = 0;
	if (PGDY[PGPN] > 7)
		{
		PADY[PGPN] = DAY + (PGDY[PGPN]-6);
		if (PADY[PGPN] > 7) PADY[PGPN] = PADY[PGPN] - 7;
		} else{ PADY [PGPN] = 0; }
		
	TIM3->CR1 &= ~TIM_CR1_CEN;			// STOP TIM3 - Stop Blinking
	STPG = 0;	
	}	
	else
		{
		TIM3->CR1 |= TIM_CR1_CEN;			// TIM3 - Count On - Blinking	Enabled
		STPG++; 
		}
	ViewProgMenu ();
	}			
		
}

EXTI->PR |= 0x04; // Reset Pending Bit
}

void EXTI3_IRQHandler(void) 									// Button "CURR. TIME" IRQ
{
		char n = 1;
		PGKL = 0;
		while ( n < 17 )
		{
		PGKL = PGKL + PGST[n];
		n++;
		}
	
	EXTI->PR |= 0x08;
	MENU = 0;
}

void EXTI4_IRQHandler(void) 									// Button "TIME SETUP" IRQ
{	
EXTI->PR |= 0x10;
	
if (MENU == 0)
{	
MENU = 1;	// Setup Time
STNU = 1;	// Setup Hour

COMAND; 
SEND(0x80);
WRDATA; 			// Write Text RUS "UST.VREM"
SEND(0xA9); SEND(0x43); SEND(0x54); SEND(0x2E); SEND(0x42); SEND(0x50); SEND(0x45); SEND(0x4D);
COMAND; 
SEND(0xC0);
WRDATA; 			// Write Current Time & Day 
SEND (ConvertD(HRT)); SEND (ConvertD(HRU)); SEND (0x3A); SEND (ConvertD(MNT)); SEND (ConvertD(MNU)); SEND(0x20); 
SEND(ConvDayA(DAY));  SEND(ConvDayB(DAY));	

NHRT = HRT; NHRU = HRU; NMNT = MNT; NMNU = MNU; NDAY = DAY; 	
NHR = (HRT*10) + HRU;
NMN = (MNT*10) + MNU;

TIM3->CR1 |= TIM_CR1_CEN;																			// Start Blinking
BLKN=1;	
}
	
}
	

void EXTI9_5_IRQHandler(void) 								// Buttons S6,S7,S8 IRQ
{
if (WAFL == 0)
{
	TIM2->CNT = 1;
	TIM2->CR1 |= TIM_CR1_CEN; // TIM2 COUNT ON
	WAFL = 1;
	
	if (EXTI->PR & (1<<6)) // PWR-ON BUTTON PRESSED
	{
		switch (MENU)
		{
			case 0:
		{
		
		MENU = 2;								 //  Set Menu Mode -> Manual Actuator Setup	
		STNU = 4;								 //  Set Menu Mode -> Manual Actuator Setup	-> Hour
		
COMAND; 
SEND(0x80);
WRDATA; 							// Write Text RUS "RUCHNOY"
SEND(0x50); SEND(0xA9); SEND(0xAB); SEND(0x48); SEND(0x4F); SEND(0xA6); SEND(0x20); SEND(0x70);
ViewActSetup ();		
TIM3->CR1 |= TIM_CR1_CEN;																			// Start Blinking
BLKN=1;	

			break;
		}
			case 2: 
		{
			MENU = 0;
			STNU = 0;
			break;
		}
	}
	}	
if (EXTI->PR & (1<<7)) // PWR-OFF BUTTON PRESSED
	{
		if (MENU != 3)
		{
		MOST = 0;					//	 Actuator OFF
		MENU = 0;
		STNU=0;
		ViewFullTime ();			 
		}
		else
		{
		char n = 1;
		while ( n < 17 )
		{
		PGST[n] = 0;
		n++;
		}
		ViewProgMenu ();
		}
		
	}
	
	if (EXTI->PR & (1<<5)) // PROG BUTTON PRESSED
	{
	if (MENU == 0)
			{
				MENU = 3;
				PGPN = 1;
				STPG = 0;
				ViewProgMenu ();
			}
		
	}
}	
EXTI->PR |= 0x3E0;				// Clear Pending bits
}

void TIM2_IRQHandler(void) 										// BUTTONS WAIT TIMER
  {
   
		TIM2->SR &= ~TIM_SR_UIF; 			//	Clear IRQbit
		TIM2->CR1 &= ~TIM_CR1_CEN;		// STOP TIM2
		WAFL = 0;
		
  }

void TIM3_IRQHandler(void) 										// BLINKING TIMER
  {
		TIM3->SR &= ~TIM_SR_UIF; 			//	Clear IRQbit
		
		if (MENU == 1)								// Setup Time
		{
			switch (STNU)
		{
			case 1:			// Blinking Cursor to Hour
			{
				COMAND; 									
				SEND(0xC0);
				WRDATA;
				break;	
			}
			case 2:			// Blinking Cursor to Minuts
			{
				COMAND; 									
				SEND(0xC3);
				WRDATA;	
				break;
			}
			case 3:			// Blinking Cursor to Day
			{
				COMAND; 									
				SEND(0xC6);
				WRDATA;
				break;
			}
			
			
		}
			if (BLKN == 0) // Delete Current Symbols in Cursor
			{
				BLKN = 1;
			SEND(0x20);
			SEND(0x20);
			}
			else					 // Print Current Info in Cursor
			{
				BLKN = 0;
			switch (STNU)
		{
			case 1:			// Print New Hour
			{
				SEND (ConvertD(NHRT)); SEND (ConvertD(NHRU));  
				break;	
			}
			case 2:			// Print New Minuts
			{
				SEND (ConvertD(NMNT)); SEND (ConvertD(NMNU)); 
				break;
			}
			case 3:			// Print New Day
			{
				SEND (ConvDayA(NDAY)); SEND (ConvDayB(NDAY));
				break;
			}
		}
			
			
			}
			
			
		}
		
		if (MENU == 2)								// Setup Manual Act
		{
			if (BLKN == 1)
			{
			ViewActSetup ();
			BLKN = 0;
			}
			else
			{
			switch (STNU)
				{
				case 4:
				{
				COMAND; 									
				SEND(0xC1);
				WRDATA;
				SEND(0x20);	
				break;	
				}
				case 5:
				{
				COMAND; 									
				SEND(0xC3);
				WRDATA;
				SEND(0x20);	
				SEND(0x20);	
				break;	
				}
				}	
			BLKN = 1;
			}
		
		}
		
		if (MENU == 3)								// Programm Menu
		{
			if (BLKN == 1)
			{
			ViewProgMenu ();
			BLKN = 0;
			}
			else
			{
			switch (STPG)
				{
				case 1:
				{
				COMAND; 									
				SEND(0xC0);
				WRDATA;
				SEND(0x20);
				SEND(0x20);					
				break;	
				}
				case 2:
				{
				COMAND; 									
				SEND(0xC3);
				WRDATA;
				SEND(0x20);	
				SEND(0x20);	
				break;	
				}
				case 3:
				{
				COMAND; 									
				SEND(0x84);
				WRDATA;
				SEND(0x20);		
				break;	
				}
				case 4:
				{
				COMAND; 									
				SEND(0x86);
				WRDATA;	
				SEND(0x20);	
				SEND(0x20);	
				break;	
				}
				case 5:
				{
				COMAND; 									
				SEND(0xC6);
				WRDATA;
				SEND(0x20);	
				SEND(0x20);	
				break;	
				}
				
				}	
			BLKN = 1;
			}
		
		}
		
  }




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
