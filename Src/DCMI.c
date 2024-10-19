



#include "DCMI.h"

#define BUFFER_SIZE 640 * 480

uint8_t imageDataBuffer[BUFFER_SIZE];

void DCMI_InitGPIO(void);
void DCMI_InitDMA(void);

void DCMI_InitGPIO(void)
{
	// Enable Clock to DCMI
	RCC->AHB2ENR |= RCC_AHB2ENR_DCMIEN;
	while(!(RCC->AHB2ENR & RCC_AHB2ENR_DCMIEN));
	// Enable Clock to GPIO Port H and Port I
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOHEN | RCC_AHB2ENR_GPIOIEN);
	while(!(RCC->AHB2ENR & (RCC_AHB2ENR_GPIOHEN | RCC_AHB2ENR_GPIOIEN)));
	// Set PH5, PH8, PH9, PH10, PH11, PH12, PH14 to Alternate Function Mode
	GPIOH->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
	GPIOH->MODER |= (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1 | GPIO_MODER_MODE14_1);
	// Set PI4, PI5, PI6, PI7 to Alternate Function Mode
	GPIOI->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	// Set PH5 to AF10
	GPIOH->AFR[0] |= (0x0A<<20);
	// Set PH8, PH9, PH10, PH11, PH12, PH14 to AF10
	GPIOH->AFR[1] |= ((0x0A<<0) | (0x0A<<4) | (0x0A<<8) | (0x0A<<12) | (0x0A<<16) | (0x0A<<24));
	// Set PI4, PI5, PI6, PI7 to AF10
	GPIOI->AFR[0] |= ((0x0A<<16) | (0x0A<<20) | (0x0A<<24) | (0x0A<<28));
}

void DCMI_InitDMA(void)
{
	// Enable clock to DMA1 and DMAMUX1
	RCC->AHB1ENR |= (RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN);
	while(!(RCC->AHB1ENR & (RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN)));
	// Set DMAMUX Input to 90
	DMAMUX1_Channel0->CCR |= 90;
	// Set DMA Source Address to DCMI_DR
	DMA1_Channel1->CPAR = (uint32_t)&(DCMI->DR);
	// Set Destination Address to imageDataBuffer
	DMA1_Channel1->CMAR = (uint32_t)imageDataBuffer;
	// Set Number of data items to transfer
	DMA1_Channel1->CNDTR = BUFFER_SIZE;
	// Set DMA to Memory Increment
	DMA1_Channel1->CCR = DMA_CCR_MINC;
	// Enable Transfer Complete Interrupt
	DMA1_Channel1->CCR = DMA_CCR_TCIE;
	// Set Data Transfer Direction to Read from Memory
	DMA1_Channel1->CCR = DMA_CCR_DIR;
	// Set Priority to High
	DMA1_Channel1->CCR = DMA_CCR_PL_1;
}

void DCMI_DeInit(void)
{
	// Disable clock to DCMI
	RCC->AHB2ENR &= ~(RCC_AHB2ENR_DCMIEN);
}

void DCMI_Init(void)
{
	// Initialize GPIO and Clock for DCMI
	DCMI_InitGPIO();
	// Initialize DMA for DCMI
	DCMI_InitDMA();
	// Set DCMI to 8-bit Data mode
	DCMI->CR &= ~(DCMI_CR_EDM);
	// Set Pixel Clock Polarity to Rising Edge
	DCMI->CR |= DCMI_CR_PCKPOL;
	// Select Hardware Synchronization Mode
	DCMI->CR &= ~(DCMI_CR_ESS);
	// Set Capture Mode to Snapshot
	DCMI->CR &= ~(DCMI_CR_CM);
	// Enbale JPEG Compression
	DCMI->CR |= DCMI_CR_JPEG;
	// Enable DCMI Line Interrupt
	DCMI->IER|= DCMI_IER_LINE_IE;
	// Enable DCMI_VSYNC Interrupt
	DCMI->IER|= DCMI_IER_VSYNC_IE;
	// Enable DCMI Synchronization Error Interrupt
	DCMI->IER|= DCMI_IER_ERR_IE;
	// Enable DCMI Overrun Interrupt
	DCMI->IER|= DCMI_IER_OVR_IE;
	// Enable DCMI End of Frame Interrupt
	DCMI->IER|= DCMI_IER_FRAME_IE;
	// Enable DCMI
	DCMI->CR |= DCMI_CR_ENABLE;
}

void DCMI_StartCapture(void)
{
	// Enable Capture
	DCMI->CR |= DCMI_CR_CAPTURE;
	// Start the DMA transfer
	DMA1_Channel1->CCR |= DMA_CCR_EN;
}
void DCMI_StopCapture(void)
{
	// Disable Capture
	DCMI->CR &= ~(DCMI_CR_CAPTURE);
}

void DCMI_ReadData(void)
{

}

void DCMI_GetStatus(void)
{
	//Check if FIFO is empty
	if(DCMI->SR & DCMI_SR_FNE)
	{
		UART_Tx_String("FIFO is Empty\n\r");
	}
	// Check if in Frame or Not
	else if(DCMI->SR & DCMI_SR_HSYNC)
	{
		UART_Tx_String("Synchronization between Frames\n\r");
	}
	// Check if in Line or Not
	else if(DCMI->SR & DCMI_SR_VSYNC)
	{
		UART_Tx_String("Synchronization between Lines\n\r");
	}
}

void DCMI_ErrorHandler(void)
{

}

void DCMI_IRQHandler(void)
{
	if(DCMI->RISR & DCMI_RIS_LINE_RIS)		// Check if End of Line Interrupt Fired
	{
		UART_Tx_String("End of Line Interrupt Fired\n\r");
		DCMI->ICR &= ~(DCMI_ICR_LINE_ISC);// Clear End of Line Interrupt
	}
	else if(DCMI->RISR & DCMI_RIS_VSYNC_RIS)// Check Frame Synchronization Interrupt Fired
	{
		UART_Tx_String("Frame Synchronization Interrupt Fired\n\r");
		DCMI->ICR &= ~(DCMI_ICR_VSYNC_ISC);// Clear Frame Synchronization Interrupt
	}
	else if(DCMI->RISR & DCMI_RIS_ERR_RIS)	// Check Embedded Synchronization Error Interrupt Fired
	{
		UART_Tx_String("Embedded Synchronization Error Interrupt Fired\n\r");
		DCMI->ICR &= ~(DCMI_ICR_ERR_ISC);// Clear Embedded Synchronization Error Interrupt
	}
	else if(DCMI->RISR & DCMI_RIS_OVR_RIS) // Check if Data Overrun Interrupt Fired
	{
		UART_Tx_String("Data Overrun Interrupt Fired\n\r");
		DCMI->ICR &= ~(DCMI_ICR_OVR_ISC);// Clear Data Overrun Interrupt
	}
	else if(DCMI->RISR & DCMI_RIS_FRAME_RIS)// Check if End of Frame Interrupt Fired
	{
		UART_Tx_String("End of Frame Interrupt Fired\n\r");
		DCMI->ICR &= ~(DCMI_ICR_FRAME_ISC);// Clear End of Frame Interrupt
	}
}
