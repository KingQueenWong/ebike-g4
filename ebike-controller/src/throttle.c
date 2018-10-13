/*
 * throttle.c
 *
 *  Created on: Sep 17, 2016
 *      Author: David
 */

#include "throttle.h"
#include "gpio.h"

Biquad_Float_Type Throttle_filt=THROTTLE_LPF_DEFAULTS;
volatile Throttle_Analog_Type sThrottle=THROTTLE_ANALOG_DEFAULTS;
volatile Throttle_PAS_Type sPasThrottle=THROTTLE_PAS_DEFAULTS;
Biquad_Float_Type sPasBiqLow=BIQ_LPF_DEFAULTS;
Biquad_Float_Type sPasBiqHigh=BIQ_LPF_DEFAULTS;
// Last output value, used for rate limiting
float prev_output = 0.0f;

void throttle_switch_type(uint8_t thrnum, uint8_t thrtype)
{
  if(thrnum == 1)
  {
    if(thrtype == THROTTLE_TYPE_ANALOG)
    {
      // Set throttle pin to analog input
      GPIO_Analog(ADC_I_VBUS_THR1_PORT, ADC_THR1_PIN);
      // Disable pin change interrupt
      EXTI->IMR &= ~(EXTI_IMR_MR5);
    }
    else if(thrtype == THROTTLE_TYPE_PAS)
    {
      // Set throttle pin to digital input
      GPIO_Input(ADC_I_VBUS_THR1_PORT, ADC_THR1_PIN);
      // Enable pin change interrupt
      EXTI->IMR |= EXTI_IMR_MR5; // Enabled for line 5 (Px5 pins)
//      EXTI->FTSR |= EXTI_FTSR_TR5;
      EXTI->RTSR |= EXTI_RTSR_TR5; // Rising edge enabled
      SYSCFG->EXTICR[1] |= (0x03 << 4); // Set EXTI5 for Port C

      // Enable this interrupt in the NVIC
      NVIC_SetPriority(EXTI9_5_IRQn,PRIO_PAS);
      NVIC_EnableIRQ(EXTI9_5_IRQn);

      // Startup the PAS timer, 0.1ms precision
      PAS1_TIMER_CLK_ENABLE();
      PAS1_TIM->PSC = PAS_TIM_PSC;
      PAS1_TIM->ARR = 0xFFFF;
      PAS1_TIM->CR1 = TIM_CR1_CEN;
    }
  }
  else if(thrnum == 2)
  {
    if(thrtype == THROTTLE_TYPE_ANALOG)
    {
      // Set throttle pin to analog input
      GPIO_Analog(ADC_THR2_AND_TEMP_PORT, ADC_THR2_PIN);
      // Disable pin change interrupt
      EXTI->IMR &= ~(EXTI_IMR_MR0);
    }
    else if(thrtype == THROTTLE_TYPE_PAS)
    {
      // Set throttle pin to digital input
      GPIO_Input(ADC_THR2_AND_TEMP_PORT, ADC_THR2_PIN);
      // Enable pin change interrupt
      EXTI->IMR |= EXTI_IMR_MR0; // Enabled for line 0 (Px0 pins)
//      EXTI->FTSR |= EXTI_FTSR_TR0;
      EXTI->RTSR |= EXTI_RTSR_TR0; // Rising edge enabled
      SYSCFG->EXTICR[0] |= 0x02; // Set EXTI0 for Port B

      // Enable this interrupt in the NVIC
      NVIC_SetPriority(EXTI0_IRQn,PRIO_PAS);
      NVIC_EnableIRQ(EXTI0_IRQn);

      // Startup the PAS timer, 0.1ms precision
      PAS2_TIMER_CLK_ENABLE();
      PAS2_TIM->PSC = PAS_TIM_PSC;
      PAS2_TIM->ARR = 0xFFFF;
      PAS2_TIM->CR1 = TIM_CR1_CEN;
    }
  }
}

float throttle_process(float raw_voltage)
{
	// Filter the raw throttle voltage
	Throttle_filt.X = raw_voltage;
	dfsl_biquadf(&Throttle_filt);

	// *****STARTUP SEQUENCE*****
	// For the first Throttle Startup period, input is ignored
	// This allows the Biquad filter to stabilize
	// After this period, the value of the biquad filter output is
	// selected as the minimum throttle position.

	float temp_cmd;
	if(sThrottle.startup_count < THROTTLE_START_TIME)
	{
		// Throttle startup routine.
		// No effect for about a short duration ("Deadtime")
		// Then, average the throttle position for the remainder of the startup
		// This averaged value becomes the throttle minimum position
		sThrottle.startup_count++;
		if(sThrottle.startup_count >= THROTTLE_START_DEADTIME)
		{
			sThrottle.min = sThrottle.min + Throttle_filt.Y;
		}
		return 0.0f;
	}
	else
	{
		// If this is the first time, compute the average for the minimum position
		// End of startup routine
		if(sThrottle.startup_count == THROTTLE_START_TIME)
		{
			sThrottle.startup_count++;
			// Calculate the minimum throttle position
			sThrottle.min = sThrottle.min / (THROTTLE_START_TIME - THROTTLE_START_DEADTIME);
			if((sThrottle.min > 1.0f) || (sThrottle.min < 0.3f))
			{
				sThrottle.min = THROTTLE_MIN_DEFAULT;
			}
			// Estimate the throttle maximum position
			sThrottle.max = adcGetVref() - THROTTLE_DROPOUT;
			if((sThrottle.max > 3.0f) || (sThrottle.max < 1.5f))
			{
				sThrottle.max = THROTTLE_MAX_DEFAULT;
			}
			// Calculate a scale factor to apply to the raw voltage
			sThrottle.scale_factor = (1.0f)/(sThrottle.max - sThrottle.min);


		}
		// If incoming throttle position is less than the recorded minimum,
		// Redo the startup routine
		if(Throttle_filt.Y < ( sThrottle.min - THROTTLE_RANGE_LIMIT))
		{
			sThrottle.startup_count = 0;
			return 0.0f;
		}
		// If incoming throttle position is greater than recorded maximum,
		// take that value as maximum and recalculate scale factor
		if(Throttle_filt.Y > (sThrottle.max + THROTTLE_RANGE_LIMIT))
		{
			sThrottle.max = Throttle_filt.Y;
			sThrottle.scale_factor = (1.0f)/(sThrottle.max - sThrottle.min);
		}
		// Regular throttle processing
		temp_cmd = (Throttle_filt.Y - sThrottle.min) * (sThrottle.scale_factor);
		// Clip at 0% and 100%
		if(temp_cmd < THROTTLE_OUTPUT_MIN) temp_cmd = THROTTLE_OUTPUT_MIN;
		if(temp_cmd > THROTTLE_OUTPUT_MAX) temp_cmd = THROTTLE_OUTPUT_MAX;
		if(sThrottle.state)
		{
			// Hysteresis. If throttle was on but passes below hysteresis level, turn it off
			if(temp_cmd <= THROTTLE_HYST_LOW)
			{
				temp_cmd = 0.0f;
				sThrottle.state = 0;
			}
		}
		else
		{
			// If throttle was off but passes above hysteresis level, turn it on
			if(temp_cmd >= THROTTLE_HYST_HIGH)
			{
				sThrottle.state = 1;
			}
			else
			{
				temp_cmd = 0.0f;
			}
		}
		// Rate limit (upward only! no limit on how fast the throttle can fall)
      float delta = temp_cmd - prev_output;
      if(delta > THROTTLE_SLEW_RATE)
      {
        temp_cmd = prev_output + THROTTLE_SLEW_RATE;
      }
		prev_output = temp_cmd;
		return temp_cmd;
	}
}

void throttle_pas_process(uint8_t thrnum)
{
  uint8_t current_reading;
  float thr_output;

  if(thrnum == 1)
  {
    current_reading = ADC_I_VBUS_THR1_PORT->IDR & (1<<ADC_THR1_PIN);
  }
  else if(thrnum == 2)
  {
    current_reading = ADC_THR2_AND_TEMP_PORT->IDR & (1<<ADC_THR2_PIN);
  }
  else
    return;

  if (current_reading != sPasThrottle.last_reading) {
    sPasThrottle.last_reading = current_reading;
    if (current_reading == 1) {
      // Rising edge detected
      sPasThrottle.filtered_speed = (sPasThrottle.time_counter / 1000.0f)
          * PAS_FILTER + (sPasThrottle.filtered_speed * (1 - PAS_FILTER));
      sPasThrottle.time_counter = 0;
    }
  }
  else {
    sPasThrottle.time_counter++;
  }
  thr_output = sPasThrottle.filtered_speed * sPasThrottle.scale_factor;
  if(thr_output < THROTTLE_OUTPUT_MIN) thr_output = THROTTLE_OUTPUT_MIN;
  if(thr_output > THROTTLE_OUTPUT_MAX) thr_output = THROTTLE_OUTPUT_MAX;

}
