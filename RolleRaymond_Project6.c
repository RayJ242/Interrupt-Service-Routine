// ----------------------------------------------------------------------------------------------------------------------------------------
// Name: Raymond Rolle
// Date: April 19th, 2021
// Course: ELEC3371-01
// Description: In this project an Interrupt Service Routine is created which
// will be called from a timer interrupt event or a push button interrupt event.
//
// ----------------------------------------------------------------------------------------------------------------------------------------

// VARIABLE DECLARATIONS
unsigned int PortE_count = 0;
unsigned int freeze = 0;
unsigned int input;
unsigned int MSD, LSD = 0;

// FUNCTION DECLARATIONS
void InitializeUSART1();
void PinConfiguration();
void Timer2Configuration();
void ExternalIntConfig();
void Newline();

void EXTIPB6() iv IVT_INT_EXTI9_5 ics ICS_AUTO {
    EXTI_PR.B6 = 1;

    if      (PortE_count > 100) {}
    else if (PortE_count < 0 ) {}
    else {

        MSD = PortE_count / 10;
        LSD = PortE_count - (MSD*10);
        while(USART1_SR.TC == 0) {}
        USART1_DR = MSD + 48;
        while(USART1_SR.TC == 0) {}
        USART1_DR = LSD + 47;

        Newline();
    }
}

void EXTIPA0 () iv IVT_INT_EXTI0 {
    EXTI_PR.B0 = 1;

    switch (freeze) {
      case 0  : freeze = 1; break;
      case 1  : freeze = 0; break;
      default : break;
    }
}

// MAIN FUNCTION
void main() {

// INITIALIZATIONS    used functions from old projects to initialize
    InitializeUSART1();
    PinConfiguration();
    Timer2Configuration();
    ExternalIntConfig();

    GPIOD_ODR = 0X0000;
    GPIOE_ODR = 0X0000;

// MAIN LOOP
    for(;;) {

        if (TIM2_SR.UIF == 1) {

            if (freeze == 0) {

                TIM2_SR.UIF = 0;

                if (PortE_count > 100) {
                    PortE_count = 0;
                }

                PortE_count = PortE_count + 1;
                GPIOE_ODR = 0x00FF * PortE_count;
            }

                        if (((USART1_SR & (1<<5))== 0x20)) {
                          input = USART1_DR;
                          while (! (USART1_SR & (1<<7)) == 0x80) {}
                          USART1_DR = input;
                          PortE_count = PortE_count + 1;
                          GPIOE_ODR = 0x00FF * PortE_count;
                        }
                }
    }
}
// ----------------------------------------------------------------------------------------------------------------------------------------

// SUB FUNCTIONS
void InitializeUSART1() {
    RCC_APB2ENR |= 1;
    AFIO_MAPR = 0X0F000000;
    RCC_APB2ENR |= 1<<2;
    GPIOA_CRH &= ~(0xFF << 4);
    GPIOA_CRH |= (0x0B << 4);
    GPIOA_CRH |= (0x04 << 8);
    RCC_APB2ENR |= 1<<14;
    USART1_BRR = 0X00000506;
    USART1_CR1 &= ~(1<<12);
    USART1_CR2 &= ~(3<<12);
    USART1_CR3 &= ~(3<<8);
    USART1_CR1 &= ~(3<<9);
    USART1_CR1 |= 3<<2;
    USART1_CR1 |= 1<<13;
    Delay_ms(100);
}

void Timer2Configuration() {
        RCC_APB1ENR.TIM2EN = 1;
        TIM2_CR1 = 0x0000;
        TIM2_PSC = 7999;
        TIM2_ARR = 9000;
        TIM2_CR1 = 0x0001;
}

void PinConfiguration() {
    GPIO_Digital_Output(&GPIOD_BASE, _GPIO_PINMASK_ALL);
    GPIO_Digital_Output(&GPIOE_BASE, _GPIO_PINMASK_ALL);
}

void ExternalIntConfig(){
        RCC_APB2ENR.AFIOEN = 1;
        AFIO_EXTICR1  = 0x00000000;
        AFIO_EXTICR2 |= 0x00000100;
        EXTI_RTSR |= 0x00000041;
        EXTI_IMR |= 0x00000041;
        NVIC_ISER0 |= 1<<6;
        NVIC_ISER0.B23 = 1;
}

void Newline(){
    while(USART1_SR.TC == 0) {}
    USART1_DR = 0x0D;
    while(USART1_SR.TC == 0) {}
    USART1_DR = 0x0A;
}