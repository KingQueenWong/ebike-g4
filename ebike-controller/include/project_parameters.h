/******************************************************************************
 * Filename: project_parameters.h
 ******************************************************************************

 Copyright (c) 2019 David Miller

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

#ifndef PROJECT_PARAMETERS_H_
#define PROJECT_PARAMETERS_H_

#if 1

/***  ADC Configuration Variable IDs ***/
#define CONFIG_ADC_PREFIX           (0x0000)
#define CONFIG_ADC_NUMVARS          (5)
#define CONFIG_ADC_INV_TIA_GAIN     (0x0001)
#define CONFIG_ADC_VBUS_RATIO       (0x0002)
#define CONFIG_ADC_THERM_FIXED_R    (0x0003)
#define CONFIG_ADC_THERM_R25        (0x0004)
#define CONFIG_ADC_THERM_B          (0x0005)
/*** ADC Default Values ***/
#define DFLT_ADC_INV_TIA_GAIN       (20.0f) // 0.001 Ohms, 50x INA213 gain, 1/(50*.001) = 20
#define DFLT_ADC_VBUS_RATIO         (33.36246f) // 1 / (3.09kOhm / (100 + 3.09kOhm)) = 33.36246
#define DFLT_ADC_THERM_FIXED_R      (10000.0f) // 10k resistor
#define DFLT_ADC_THERM_R25          (10000.0f) // Thermistor is 10k at 25degC
#define DFLT_ADC_THERM_B            (3984.0f) // Thermistor Beta value (NTCALUG02A103G)

/*** FOC Variable IDs ***/
#define CONFIG_FOC_PREFIX           (0x0100)
#define CONFIG_FOC_NUMVARS          (8)
#define CONFIG_FOC_KP               (0x0101)
#define CONFIG_FOC_KI               (0x0102)
#define CONFIG_FOC_KD               (0x0103)
#define CONFIG_FOC_KC               (0x0104)
#define CONFIG_FOC_OUTMIN           (0x0105)
#define CONFIG_FOC_OUTMAX           (0x0106)
#define CONFIG_FOC_PWM_FREQ         (0x0107)
#define CONFIG_FOC_PWM_DEADTIME     (0x0108)
/*** FOC Default Values ***/
#define DFLT_FOC_KP                 (0.1f)
#define DFLT_FOC_KI                 (0.001f)
#define DFLT_FOC_KD                 (0.0f)
#define DFLT_FOC_KC                 (0.05f)
#define DFLT_FOC_OUTMIN             (-0.95f)
#define DFLT_FOC_OUTMAX             (0.95f)
#define DFLT_FOC_PWM_FREQ           (20000)
#define DFLT_FOC_PWM_DEADTIME       (500)

/*** Main Variable IDs ***/
#define CONFIG_MAIN_PREFIX          (0x0200)
#define CONFIG_MAIN_NUMVARS         (16)
#define CONFIG_MAIN_RAMP_SPEED      (0x0201)
#define CONFIG_MAIN_COUNTS_TO_FOC   (0x0202)
#define CONFIG_MAIN_SPEED_TO_FOC    (0x0203)
#define CONFIG_MAIN_SWITCH_EPS      (0x0204)
#define CONFIG_MAIN_NUM_USB_OUTPUTS (0x0205)
#define CONFIG_MAIN_USB_SPEED       (0x0206)
#define CONFIG_MAIN_USB_CHOICE_1    (0x0207)
#define CONFIG_MAIN_USB_CHOICE_2    (0x0208)
#define CONFIG_MAIN_USB_CHOICE_3    (0x0209)
#define CONFIG_MAIN_USB_CHOICE_4    (0x020A)
#define CONFIG_MAIN_USB_CHOICE_5    (0x020B)
#define CONFIG_MAIN_USB_CHOICE_6    (0x020C)
#define CONFIG_MAIN_USB_CHOICE_7    (0x020D)
#define CONFIG_MAIN_USB_CHOICE_8    (0x020E)
#define CONFIG_MAIN_USB_CHOICE_9    (0x021F)
#define CONFIG_MAIN_USB_CHOICE_10   (0x0210)
/*** Main Default Values ***/
#define DFLT_MAIN_RAMP_SPEED        (5.0f)
#define DFLT_MAIN_COUNTS_TO_FOC     (1000)
#define DFLT_MAIN_SPEED_TO_FOC      (10.0f)
#define DFLT_MAIN_SWITCH_EPS        (0.00833333f) // About 3 degrees
#define DFLT_MAIN_NUM_USB_OUTPUTS   (5)
#define DFLT_MAIN_USB_SPEED         (0) // Slowest (50 Hz)
#define DFLT_MAIN_USB_CHOICE_1      (1) // Ia
#define DFLT_MAIN_USB_CHOICE_2      (2) // Ib
#define DFLT_MAIN_USB_CHOICE_3      (3) // Ic
#define DFLT_MAIN_USB_CHOICE_4      (7) // Throttle
#define DFLT_MAIN_USB_CHOICE_5      (11)// Vbus
#define DFLT_MAIN_USB_CHOICE_6      (4) // Ta
#define DFLT_MAIN_USB_CHOICE_7      (5) // Tb
#define DFLT_MAIN_USB_CHOICE_8      (6) // Tc
#define DFLT_MAIN_USB_CHOICE_9      (9) // HallAngle
#define DFLT_MAIN_USB_CHOICE_10     (18)// HallState

/*** Throttle Variable IDs ***/
#define CONFIG_THRT_PREFIX          (0x0300)
#define CONFIG_THRT_NUMVARS         (12)
#define CONFIG_THRT_TYPE1           (0x0301)
#define CONFIG_THRT_MIN1            (0x0302)
#define CONFIG_THRT_MAX1            (0x0303)
#define CONFIG_THRT_HYST1           (0x0304)
#define CONFIG_THRT_FILT1           (0x0305)
#define CONFIG_THRT_RISE1           (0x0306)
#define CONFIG_THRT_TYPE2           (0x0307)
#define CONFIG_THRT_MIN2            (0x0308)
#define CONFIG_THRT_MAX2            (0x0309)
#define CONFIG_THRT_HYST2           (0x030A)
#define CONFIG_THRT_FILT2           (0x030B)
#define CONFIG_THRT_RISE2           (0x030C)
/*** Throttle Default Values ***/
#define DFLT_THRT_TYPE1             (0) // Analog ADC type
#define DFLT_THRT_MIN1              (0.85f)
#define DFLT_THRT_MAX1              (2.20f)
#define DFLT_THRT_HYST1             (0.025f)
#define DFLT_THRT_FILT1             (2.0f)
#define DFLT_THRT_RISE1             (0.0005f)
#define DFLT_THRT_TYPE2             (2) // None
#define DFLT_THRT_MIN2              (0.85f)
#define DFLT_THRT_MAX2              (2.20f)
#define DFLT_THRT_HYST2             (0.025f)
#define DFLT_THRT_FILT2             (2.0f)
#define DFLT_THRT_RISE2             (0.0005f)

/*** Limit Variable IDs ***/
#define CONFIG_LMT_PREFIX           (0x0400)
#define CONFIG_LMT_NUMVARS          (8)
#define CONFIG_LMT_VOLT_MIN         (0x0401)
#define CONFIG_LMT_VOLT_MAX         (0x0402)
#define CONFIG_LMT_PHASE_I_MAX      (0x0403)
#define CONFIG_LMT_PHASE_REGEN_MAX  (0x0404)
#define CONFIG_LMT_BATT_I_MAX       (0x0405)
#define CONFIG_LMT_BATT_REGEN_MAX   (0x0406)
#define CONFIG_LMT_FET_TEMP_MAX     (0x0407)
#define CONFIG_LMT_MOTOR_TEMP_MAX   (0x0408)
/*** Limit Default Values ***/
#define DFLT_LMT_VOLT_MIN           (48.0f) // 3.0V x 16 cells
#define DFLT_LMT_VOLT_MAX           (67.2f) // 4.2V x 16 cells
#define DFLT_LMT_PHASE_I_MAX        (25.0f)
#define DFLT_LMT_PHASE_REGEN_MAX    (10.0f)
#define DFLT_LMT_BATT_I_MAX         (25.0f)
#define DFLT_LMT_BATT_REGEN_MAX     (10.0f)
#define DFLT_LMT_FET_TEMP_MAX       (90.0f)
#define DFLT_LMT_MOTOR_TEMP_MAX     (90.0f)

/*** Motor Configuration Variable IDs ***/
#define CONFIG_MOTOR_PREFIX         (0x0500)
#define CONFIG_MOTOR_NUMVARS        (7)
#define CONFIG_MOTOR_HALL1          (0x0501)
#define CONFIG_MOTOR_HALL2          (0x0502)
#define CONFIG_MOTOR_HALL3          (0x0503)
#define CONFIG_MOTOR_HALL4          (0x0504)
#define CONFIG_MOTOR_HALL5          (0x0505)
#define CONFIG_MOTOR_HALL6          (0x0506)
#define CONFIG_MOTOR_POLEPAIRS      (0x0507)
/*** Motor Default Values ***/
// For Ebikeling 700C front 1200W motor
#define DFLT_MOTOR_HALL1            (0.743786f)
#define DFLT_MOTOR_HALL2            (0.089677f)
#define DFLT_MOTOR_HALL3            (0.905861f)
#define DFLT_MOTOR_HALL4            (0.412525f)
#define DFLT_MOTOR_HALL5            (0.593083f)
#define DFLT_MOTOR_HALL6            (0.240492f)
#define DFLT_MOTOR_POLEPAIRS        (23)

#endif

#if 0
/*** ADC Defaults ***/
#define RSHUNT_INV              (1000.0f) // Inverse of 0.001 Ohms
#define INAGAIN_INV             (0.02f) // Inverse of 50x gain (INA213 current amplifier)
#define CURRENT_AMP_INV         (20.0f) // Above two multiplied together - multiply volts by this to get amps
#define VBUS_RTOP               (100000.0f) // 100k resistor on top
#define VBUS_RBOT               (3090.0f)   // 3.09k resistor on bottom
#define VBUS_RESISTOR_RATIO     (33.36246f) // Inverse of resistor gain (3.09 / 103.09)

#define TEMP_FIXED_RESISTOR     (10000.0f) // Fixed resistance of the thermistor voltage divider
#define THERM_R25               (10000.0f) // Thermistor resistance at 25 degC (NTCALUG02A103G)
#define THERM_B_VALUE           (3984.0f)  // Beta value of thermistor (NTCALUG02A103G)

/***  FOC Library Defaults ***/
#define FOC_KP                  (419430) // 0.1
#define FOC_KI                  (4194) // 0.001
#define FOC_KD                  (0)
#define FOC_KC                  (209715) // 0.05
#define FOC_OUTMIN              (-65536) // -1
#define FOC_OUTMAX              (65536) // +1

#define FOC_KP_F                (0.1f)
#define FOC_KI_F                (0.001f)
#define FOC_KD_F                (0.0f)
#define FOC_KC_F                (0.05f)
#define FOC_OUTMIN_F            (-0.95f)
#define FOC_OUTMAX_F            (0.95f)

/***  Defaults for Main ***/
#define DEFAULT_PWM_FREQ        (20000)
#define DEFAULT_PWM_FREQ_F      (20000.0f)
#define DEFAULT_FILT_CUTOFF_F   (2000.0f)
#define DEFAULT_FILT_Q_F        (0.707f) // 2nd order Butterworth

#define RAMP_CALLFREQ           (20000)
#define RAMP_CALLFREQF          (20000.0f)
#define RAMP_DEFAULTSPEED       (5)
#define RAMP_DEFAULTSPEEDF      (5.0f)

#define SPEED_COUNTS_TO_FOC     (1000)
#define MIN_SPEED_TO_FOC        (10.0f)
#define FOC_SWITCH_ANGLE_EPS    (0.00833333333f) // about 3 degrees


#define DEFAULT_USB_OUTPUTS         (5)
#define DEFAULT_USB_ASSIGNMENTS     {1, 2, 3, 7, 11, 4, 5, 6, 9, 18}
#define USB_PREFIX_LENGTH           (4)
#define DEFAULT_USB_PREFIX          "DB05"
#define DEFAULT_SERIAL_DATA_RATE    (400) // (20kHz/400 = 50Hz)
#endif

// Throttle setting
#define FULLSCALE_THROTTLE      (5.0f) // Amps

// For the Hall sensor detection routine
#define HALL_DETECT_RAMP_SPEED          (5.0f) // 5 Hz = 300 eRPM = 13 RPM
#define HALL_DETECT_MIN_TRANSITIONS     (16)
#define HALL_DETECT_TRANSITIONS_TO_AVG  (16)
#define HALL_DETECT_TIMEOUT_MS          (1500)

/*** Throttle Defaults ***/
#define PAS_PPR                     (12) // pulses per rotation (number of magnets)

//#define THROTTLE_START_TIME         (1000)
//#define THROTTLE_START_DEADTIME     (500)
//#define THROTTLE_RANGE_LIMIT        (0.05f)
#define THROTTLE_MIN_DEFAULT        (0.85f)
#define THROTTLE_MAX_DEFAULT        (2.20f)
//#define THROTTLE_DROPOUT            (0.72f

#define THROTTLE_HYST_DEFAULT       (0.025f)
#define THROTTLE_HYST_MIN           (0.001f)
#define THROTTLE_HYST_MAX           (0.1f)
#define THROTTLE_FILT_DEFAULT       (2.0f)
#define THROTTLE_FILT_MIN           (0.1f)
#define THROTTLE_FILT_MAX           (499.9f)
#define THROTTLE_FILT_Q_DEFAULT     (0.707f)
#define THROTTLE_SAMPLING_RATE      (1000.0f)
// Limit the throttle climb rate to 50% / second
// The update rate is 1000Hz, so the rate limit is actually .05% per update
#define THROTTLE_RISE_DEFAULT       (0.0005f)
#define THROTTLE_RISE_MIN           (0.00005f) // Minimum of 5% / sec
#define THROTTLE_RISE_MAX           (0.01f) // Maximum of 1000% / sec

#define THROTTLE_OUTPUT_MIN         (0.00f)
#define THROTTLE_OUTPUT_MAX         (0.99f)


// Angle definitions - integer
// This set of defines are the integer values of angles
// as defined for a 16 bit unsigned integer.
#define U16_0_DEG       ((uint16_t)0)
#define U16_30_DEG      ((uint16_t)5461)
#define U16_60_DEG      ((uint16_t)10923)
#define U16_90_DEG      ((uint16_t)16384)
#define U16_120_DEG     ((uint16_t)21845)
#define U16_150_DEG     ((uint16_t)27307)
#define U16_180_DEG     ((uint16_t)32768)
#define U16_210_DEG     ((uint16_t)38229)
#define U16_240_DEG     ((uint16_t)43691)
#define U16_270_DEG     ((uint16_t)49152)
#define U16_300_DEG     ((uint16_t)54613)
#define U16_330_DEG     ((uint16_t)60075)

// Angle definitions - floating point
// This set of defines are the integer values of angles
// as defined for a single-precision float.
// Arbitrary choice of 6 significant figures.
#define F32_0_DEG       (0.0f)
#define F32_30_DEG      (0.0833333f)
#define F32_60_DEG      (0.166667f)
#define F32_90_DEG      (0.250000f)
#define F32_120_DEG     (0.333333f)
#define F32_150_DEG     (0.416667f)
#define F32_180_DEG     (0.500000f)
#define F32_210_DEG     (0.583333f)
#define F32_240_DEG     (0.666667f)
#define F32_270_DEG     (0.750000f)
#define F32_300_DEG     (0.833333f)
#define F32_330_DEG     (0.916667f)

// Interrupt priority settings
// Lowest number takes precedence
// Multiple interrupt sources can use the same priority level,
// but only a lower number interrupt will override a currently
// responding IRQ function.
#define PRIO_SYSTICK    (3)
#define PRIO_PWM        (0)
#define PRIO_HALL       (1)
#define PRIO_ADC        (2)
#define PRIO_APPTIMER   (3)
#define PRIO_HBD_UART   (4)
#define PRIO_PAS        (5)
#define PRIO_USB        (6)


#if 0
// Hall state change table
// This state table corresponds to a forward rotating motor,
// with cable connections as follows:
// Hall A (PC6) -> NineContinent Green wire
// Hall B (PC7) -> NineContinent Blue wire
// Hall C (PC8) -> NineContinent Yellow wire
//#define FORWARD_HALL_TABLE	{ 1, 5, 4, 6, 2, 3 }
#define FORWARD_HALL_TABLE      { 2, 6, 4, 5, 1, 3 } // Ebikeling 700C 1200W motor
// Same thing but reversed
//#define REVERSE_HALL_TABLE	{ 3, 2, 6, 4, 5, 1 }
#define REVERSE_HALL_TABLE      { 1, 5, 4, 6, 2, 3} // Ebikeline 700C 1200W motor
// Inverse lookup tables: Useful for determining rotation direction
// State is the table index, value is the previous state for that rotation direction
//#define FORWARD_HALL_INVTABLE	{ 0, 3, 6, 2, 5, 1, 4, 0}
//#define REVERSE_HALL_INVTABLE	{ 0, 5, 3, 1, 6, 4, 2, 0}
// For the Ebikeling 700C 1200W motor
#define FORWARD_HALL_INVTABLE   { 0, 5, 3, 1, 6, 4, 2, 0}
#define REVERSE_HALL_INVTABLE   { 0, 3, 6, 2, 5, 1, 4, 0}

// Hall Angle correlations
// This table equates the 6 valid Hall states (1 - 6) with
// motor rotation angles. The Hall state is assumed to be
// equivalent to the angle that is halfway between state
// changes. Since the state changes are spaced every 60�
// around the motor, and the transitions start at zero,
// the halfway points are at 30�, 90�, 150�, 210�, 270�,
// and 330�.
// Angle associations per state are...
//			330->030: State 1
//			030->090: State 5
// 			090->150: State 4
//			150->210: State 6
//			210->270: State 2
//			270->330: State 3

#define HALL_ANGLES_INT     {   U16_0_DEG,  /* State 0 - undefined */	\
                                U16_180_DEG,/* State 1 */		\
                                U16_60_DEG, /* State 2 */		\
                                U16_120_DEG,/* State 3 */ 		\
                                U16_300_DEG,/* State 4 */		\
                                U16_240_DEG,/* State 5 */		\
                                U16_0_DEG,  /* State 6 */		\
                                U16_0_DEG } /* State 7 - undefined */

// And the same for floating point
/*
 #define HALL_ANGLES_FLOAT	            {   F32_0_DEG,\
                                            F32_180_DEG,\
                                            F32_60_DEG,\
                                            F32_120_DEG,\
                                            F32_300_DEG,\
                                            F32_240_DEG,\
                                            F32_0_DEG,\
                                            F32_0_DEG	}
 */

/*
 #define HALL_ANGLES_FORWARD_FLOAT       {   F32_0_DEG,\
                                            F32_150_DEG,\
                                            F32_30_DEG,\
                                            F32_90_DEG,\
                                            F32_270_DEG,\
                                            F32_210_DEG,\
                                            F32_330_DEG,\
                                            F32_0_DEG }
 */
// For Ebikeling 700C front 1200W motor
#define HALL_ANGLES_FORWARD_FLOAT       {   F32_0_DEG,\
                                            (0.743786f),\
                                            (0.089677f),\
                                            (0.905861f),\
                                            (0.412525f),\
                                            (0.593083f),\
                                            (0.240492f),\
                                            F32_0_DEG }

/*
 #define HALL_ANGLES_REVERSE_FLOAT       {   F32_0_DEG, \
                                            F32_210_DEG, \
                                            F32_90_DEG, \
                                            F32_150_DEG, \
                                            F32_330_DEG, \
                                            F32_270_DEG, \
                                            F32_30_DEG, \
                                            F32_0_DEG }
 */

// For Ebikeling 700C front 1200W motor
#define HALL_ANGLES_REVERSE_FLOAT       {   F32_0_DEG, \
                                            (0.905861f), \
                                            (0.240492f), \
                                            (0.089677f), \
                                            (0.593083f), \
                                            (0.743786f), \
                                            (0.412525f), \
                                            F32_0_DEG }

// Middle of the Hall state - average of Fwd and Rev
/*
 #define HALL_ANGLES_TO_DRIVE_FLOAT      {   F32_0_DEG,\
                                            F32_270_DEG,\
                                            F32_150_DEG,\
                                            F32_210_DEG,\
                                            F32_30_DEG,\
                                            F32_330_DEG,\
                                            F32_90_DEG,\
                                            F32_0_DEG	}
 */

// For Ebikeling 700C front 1200W motor
#define HALL_ANGLES_TO_DRIVE_FLOAT      {   F32_0_DEG,\
                                            (0.824824f),\
                                            (0.165085f),\
                                            (0.997769f),\
                                            (0.502804f),\
                                            (0.668435f),\
                                            (0.326508f),\
                                            F32_0_DEG }
#endif

#endif /* PROJECT_PARAMETERS_H_ */
