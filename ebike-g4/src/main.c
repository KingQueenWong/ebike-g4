/******************************************************************************
 * Filename: main.c
 * Description: Main program code starts here. Hardware is initialized, and
 *              program flow goes into an infinite loop.
 ******************************************************************************

 Copyright (c) 2020 David Miller

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "main.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define DBG_USB_BUF_LEN 128

uint16_t VirtAddVarTab[TOTAL_EE_VARS];
#define DBG_FLAG_PWM_ENABLE 0x00000001
uint32_t DBG_Flags=0;
uint8_t DBG_Usb_Buffer[DBG_USB_BUF_LEN];

float DBG_RampAngle;
float DBG_RampIncrement;

Main_Variables Mvar;
Motor_Controls Mctrl;
Motor_Observations Mobv;
Motor_PWMDuties Mpwm;
FOC_StateVariables Mfoc;
PID_Type Mpid_Id;
PID_Type Mpid_Iq;

static void MAIN_InitializeClocks(void);
static void MAIN_CheckBootloader(void);
static void MAIN_StartAppTimer(void);

int main (
        __attribute__((unused)) int argc,
        __attribute__((unused)) char* argv[])
{

    // First check if we need to change to the bootloader.
    //
    // The bootloader can be selected to startup instead of normal code
    // by a variety of options (BOOT0 pin, options bits, etc), but here
    // those pins are used for other functions. The bootloader can be
    // entered via an interface command, like over USB or UART. The
    // MCU saves a key into standby sram and reboots. After reboot we
    // check if that key was saved and if so enter the bootloader.
    // This is done first so no peripheral settings will interfere with
    // bootloader operation
    MAIN_CheckBootloader();

    // Initialize clock resources, including power regulators and Flash latency.
    MAIN_InitializeClocks();

    // Set the NVIC priority grouping to the maximum number of preemption levels
    // Setting of PRIGROUP = b011 (0x03) means that the upper 4 bits are group
    // priority, lower 4 bits are sub-priority. But the ST implementation of the
    // Cortex M4 only has the upper 4 bits. So this sets 4 bits (16 levels) of
    // preemption, and no sub-priority levels.
    NVIC_SetPriorityGrouping(0x03u);

    // Start the systick timer for a simple delay timer
    DelayInit();

    // Start up the EEPROM emulation
    EE_Config_Addr_Table(VirtAddVarTab);
    EE_Init(VirtAddVarTab);

    // Initialize peripherals
    ADC_Init();
    CORDIC_Init();
    CRC_Init();
    DRV8353_Init();
    PWM_Init(DFLT_FOC_PWM_FREQ);
    UART_Init();
    USB_Init();
    THROTTLE_Init();
    HALL_Init(DFLT_FOC_PWM_FREQ);

    // Enable the USB CRC class
    USB_SetClass(&USB_CDC_ClassDesc, &USB_CDC_ClassCallbacks);
    USB_Start();

    USB_Data_Comm_Init();

    // LED init
    GPIO_Clk(LED_PORT);
    GPIO_Output(LED_PORT, GLED_PIN);
    GPIO_Output(LED_PORT, RLED_PIN);
    GPIO_Output(DRV_EN_PORT, DRV_EN_PIN);
    GPIO_High(LED_PORT, GLED_PIN);
    GPIO_Low(LED_PORT, RLED_PIN);
//    GPIO_Low(DRV_EN_PORT, DRV_EN_PIN);

    // Use the DAC for some SVM prettiness
    RCC->AHB2ENR |= RCC_AHB2ENR_DAC1EN;
    DAC1->CR |= DAC_CR_EN1 | DAC_CR_EN2;

    // Initialize a ramp
    DBG_RampIncrement = FOC_RampCtrl(20000.0f, 25.0f); // Called at 20kHz, 25Hz wave.

    // Start the app timer
    MAIN_StartAppTimer();

    LIVE_Init(20000); // Live data streaming will be called at 20kHz

    // Set up internal variables
    Mvar.Timestamp = 0u;
    Mvar.Ctrl = &Mctrl;
    Mvar.Foc = &Mfoc;
    Mvar.Obv = &Mobv;
    Mvar.Pwm = &Mpwm;
    Mfoc.Id_PID = &Mpid_Id;
    Mfoc.Iq_PID = &Mpid_Iq;

    // Start the watchdog
    WDT_Init();
    // Infinite loop, never return.
    while (1)
    {
        WDT_Feed();

        USB_Data_Comm_OneByte_Check();
        LIVE_SendPacket(); // Will only send when ready to do so
    }
}

// Called at 1kHz
void MAIN_AppTimerISR(void) {
    static uint16_t led_timer = 0;
    led_timer++;
    if(led_timer == 500) {
        GPIO_Low(LED_PORT, GLED_PIN);
    }
    if(led_timer >= 1000) {
        GPIO_High(LED_PORT, GLED_PIN);
        led_timer = 0;
    }

    // Slow ADC conversions
    ADC_RegSeqComplete();

    // PWM output
    if((DBG_Flags & DBG_FLAG_PWM_ENABLE) != 0 ) {
        PWM_TIM->BDTR |= TIM_BDTR_MOE;
    } else {
        PWM_TIM->BDTR &= ~(TIM_BDTR_MOE);
    }

    // Throttle processing
    THROTTLE_Process();
    Mctrl.ThrottleCommand = THROTTLE_GetCommand();
}

// Called at 20kHz
void MAIN_MotorISR(void) {
    float sin, cos;
    uint16_t dac1, dac2;

    // Increment timestamp
    Mvar.Timestamp++;

    // Increment the ramp angle
    FOC_RampGen(&DBG_RampAngle, DBG_RampIncrement);
    // And the real motor angle
    HALL_IncAngle();
    Mobv.RotorAngle = HALL_GetAngleF();
    Mobv.RotorSpeed_eHz = HALL_GetSpeedF();
    Mobv.HallState = HALL_GetState();
    // Calculate the Sin/Cos using CORDIC
    CORDIC_CalcSinCos(DBG_RampAngle*2.0f-1.0f, &sin, &cos);

    // All injected ADC should be done by now. Read them in.
    ADC_InjSeqComplete();
    Mobv.iA = ADC_GetCurrent(ADC_IA);
    Mobv.iB = ADC_GetCurrent(ADC_IB);
    Mobv.iC = ADC_GetCurrent(ADC_IC);

    // Make some waves
    FOC_Ipark(0.75f, 0.0f, sin, cos, &(Mfoc.Clarke_Alpha), &(Mfoc.Clarke_Beta));
    FOC_SVM((Mfoc.Clarke_Alpha), (Mfoc.Clarke_Beta), &(Mpwm.tA), &(Mpwm.tB), &(Mpwm.tC));
    // Show Ta and Tb on the DAC outputs
    dac1 = (uint16_t)(65535.0f*Mpwm.tA);
    dac2 = (uint16_t)(65535.0f*Mpwm.tB);
    DAC1->DHR12LD = (uint32_t)(dac1) + ((uint32_t)(dac2) << 16);

    // Also apply Ta, Tb, and Tc to the PWM outputs
    PWM_SetDutyF(Mpwm.tA, Mpwm.tB, Mpwm.tC);

    // Output live data if it's enabled
    LIVE_AssemblePacket(&Mvar);
}


/**
 * @brief  Applies clock settings, voltage scaling, Flash latency, etc.
 *
 *         Clock settings are:
 *          - Voltage range 1 with booster (enables 170MHz, without
 *                  booster is 150MHz max, and range 2 is 26MHz max)
 *          - Main clocks (Sysclock, Hclk, Pclk1, Pclk2) at 170MHz
 *          - PLLP clock at 42.5MHz. Used for ADC.
 *          - USB clock sourced from internal 48MHz oscillator, USB SOF
 *                  used to calibrate this clock.
 *          -
 *
 * @retval None
 */
static void MAIN_InitializeClocks(void) {
    uint32_t tempreg;

    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; // Enable access to power control
    PWR->CR3 |= PWR_CR3_UCPD_DBDIS; // Disable USB-PD dead battery pull-downs
    // If already in range 1, don't change it
    if((PWR->CR1 & PWR_CR1_VOS) != PWR_CR1_VOS_0) { // if VOS != b01, not in Range 1
        tempreg = PWR->CR1;
        tempreg &= ~(PWR_CR1_VOS);
        tempreg |= PWR_CR1_VOS_0;
        PWR->CR1 = tempreg;
        // Make sure it takes effect by waiting for checking VOSF
        while((PWR->SR2 & PWR_SR2_VOSF) != 0) { } // wait for the bit
    }
    // Change Flash latency to the required amount
    RCC->AHB1ENR |= RCC_AHB1ENR_FLASHEN;
    // Instruction and data caches enabled, prefetch enabled, 8 wait states
    FLASH->ACR &= ~(FLASH_ACR_LATENCY);
    FLASH->ACR |= FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_8WS;
    // Make sure it has taken effect by re-reading the register
    while((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_8WS) { } // wait for latency to take effect

    // Check if we need to change the boost mode.
    // When switching to boost mode, the AHB prescaler must be divide by 2.
    // After 1us, it can be reconfigured to the needed prescaler.
    if((PWR->CR5 & PWR_CR5_R1MODE) != 0) { // zero is enabled, one is disabled. go figure.
        // Set the AHB prescaler to divide by 2
        tempreg = RCC->CFGR;
        tempreg &= ~(RCC_CFGR_HPRE);
        tempreg |= RCC_CFGR_HPRE_3; // HPRE = b1000, sysclock divided by 2
        RCC->CFGR = tempreg;

        // Clear R1MODE to enable the Range 1 boost voltage
        PWR->CR5 &= ~(PWR_CR5_R1MODE);
    }

    // Configure the PLL to 170MHz
    RCC->CR &= ~(RCC_CR_PLLON); // turn off PLL first
    while((RCC->CR & RCC_CR_PLLRDY) != 0) { }// wait for PLLRDY to clear
    RCC->PLLCFGR =  RCC_PLLCFGR_PLLSRC_1 | // HSI16 is the PLL source
                    (3u << RCC_PLLCFGR_PLLM_Pos) | // M divider is /4
                    (85u << RCC_PLLCFGR_PLLN_Pos) | // N multiplier is x85
                    RCC_PLLCFGR_PLLREN | // PLLR output enabled
                    RCC_PLLCFGR_PLLPEN | // PLLP output enabled
                    (8u << RCC_PLLCFGR_PLLPDIV_Pos); // PLLP divider is /8
    // Not shown: PLLR divider is /2, PLLQ divider is /2 (but PLLQ output is off)
    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while((RCC->CR & RCC_CR_PLLRDY) == 0) { }// wait for PLLRDY to set
    // Switch clock sources
    RCC->CFGR |= (RCC_CFGR_SW); // Set SW[1:0] to b11 to use PLL as system clock
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS) { } // wait for the switch to take effect

    // Set prescalers. HCLK, APB1, and APB2 are all divide by 1 (170MHz in, 170MHz out)
    RCC->CFGR &= ~(RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1 | RCC_CFGR_HPRE);

    // Enable HSI48 for the USB
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;

    // Set output clocks to the peripherals that have a selection available
    RCC->CCIPR = RCC_CCIPR_ADC12SEL_0 | RCC_CCIPR_ADC345SEL_0; // ADC12 and ADC345 sourced from PLL"P" clock

    SystemCoreClockUpdate(); // Sets some internal variables to the actual clock settings.
}

/**
 * @brief  Determines whether or not to enter bootloader at startup.
 *
 *         The bootloader can be selected to startup instead of normal
 *         code by a variety of options (BOOT0 pin, options bits, etc),
 *         but here those pins are used for other functions. The
 *         bootloader can be entered via an interface command, like over
 *         USB or UART. The MCU saves a key into standby sram and reboots.
 *         After reboot we check if that key was saved and if so enter
 *         the bootloader. This is done first so no peripheral settings
 *         will interfere with bootloader operation.
 *
 * @retval None
 */
static void MAIN_CheckBootloader(void) {
    // Set the bootloader entry point as a function pointer. The value is stored at
    // the reset vector location in system memory.
    void (*bootloader)(void);

    // The backup registers are not modified by a software reset, so if application code
    // changes this register and resets the processor, we will know that a bootloader
    // reset is required.
    if(TAMP->BKP0R == BOOTLOADER_RESET_FLAG)
    {
        // Need to reset this register first. Enable access to backup domain...
        RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; // Turn on clocking to the power control
        PWR->CR1 |= PWR_CR1_DBP; // Enable access to backup domain
        // Reset the backup register so we don't get stuck forever restarting in bootloader mode
        TAMP->BKP0R = 0;
        // Remap the system memory to address 0x00000000
        SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE);
        SYSCFG->MEMRMP |= SYSCFG_MEMRMP_MEM_MODE_0; // MEM_MODE = 001b, System Flash memory mapped to 0x00000000
        // And now, load the bootloader
        __set_MSP(*((uint32_t *)(BOOTLOADER_REMAPPED_TOP_OF_STACK))); // Change stack pointer to the end of ram.
        bootloader = (void(*)(void)) *((uint32_t *)(BOOTLOADER_REMAPPED_RESET_VECTOR)); // Set bootloader entry point
        bootloader();// Call the bootloader.
    }
}

/**
 * @brief  Initializes a 1kHz application timer and enables its interrupt.
 * @retval None
 */
static void MAIN_StartAppTimer(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    APP_TIM->PSC = (APP_CLK / 1000000u) - 1; // Clocked at 1MHz, 170MHz is too fast for a 1kHz update with a 16-bit register
    APP_TIM->ARR = (1000000u / APP_TIM_RATE) - 1;
    APP_TIM->DIER = TIM_DIER_UIE; // Enable update interrupt

    NVIC_SetPriority(APP_IRQn, PRIO_APPTIMER);
    NVIC_EnableIRQ(APP_IRQn);

    APP_TIM->CR1 = TIM_CR1_CEN; // Enable counting
}


void MAIN_GoToBootloader(void) {
    // Enable access to backup registers
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; // Enable power control
    PWR->CR1 |= PWR_CR1_DBP; // Enable access to backup domain

    TAMP->BKP0R = BOOTLOADER_RESET_FLAG;

    // Reboot
    NVIC_SystemReset();
}

void MAIN_Reboot(void) {
    // Reboot
    NVIC_SystemReset();
}

uint8_t MAIN_EnableDebugPWM(void) {
    DBG_Flags |= DBG_FLAG_PWM_ENABLE;
    return RETVAL_OK;
}

uint8_t MAIN_DisableDebugPWM(void) {
    DBG_Flags &= ~(DBG_FLAG_PWM_ENABLE);
    return RETVAL_OK;
}

uint8_t MAIN_GetDashboardData(uint8_t* data) {
    // Param1: F32: Throttle position (%)
    // Param2: F32: Speed (rpm)
    // Param3: F32: Phase Amps
    // Param4: F32: Battery Amps
    // Param5: F32: Battery Volts
    // Param6: F32: Controller FET Temperature (degC)
    // Param7: F32: Motor Temperature (degC)
    // Param8: I32: Fault Code

    // MPH = eHz * wheel circ *3600 / polepairs
    // RPM = eHz * 60 / polepairs
//    float rpm_conversion = 60.0f * config_main.inv_pole_pairs;

//    data_packet_pack_float(data, Mctrl.ThrottleCommand);
    data_packet_pack_float(data, 0.0f);
    data+=4;
//    data_packet_pack_float(data, rpm_conversion*HallSensor_Get_Speedf());
    data_packet_pack_float(data, 0.0f);
    data+=4;
//    data_packet_pack_float(data, Mpc.PhaseCurrent);
    data_packet_pack_float(data, 0.0f);
    data+=4;
//    data_packet_pack_float(data, Mpc.BatteryCurrent);
    data_packet_pack_float(data, 0.0f);
    data+=4;
    data_packet_pack_float(data, ADC_GetVbus());
    data+=4;
    data_packet_pack_float(data, ADC_GetFetTempDegC());
    data+=4;
//    data_packet_pack_float(data, g_MotorTemp);
    data_packet_pack_float(data, 0.0f);
    data+=4;
//    data_packet_pack_32b(data, g_errorCode);
    data_packet_pack_32b(data, 0);

    return RETVAL_OK;
}
