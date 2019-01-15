#ifndef CONFIG_H_
#define CONFIG_H_

#include "Arduino.h"
#include <AccelStepper.h>

const char softwareName[] = "MR Flow Phantom ";
const char softwareVersion[]  = "v1.1";
const char NameAndVersion[] = "MR Flow Phantom v1.1";


// ------------------------------------ //
// ----- EasyDriver Configuration ----- //
// EasyDriver Pin Configuration (digital pins)
#define drv_step	A1
#define drv_dir		A2
#define drv_ms1		A3
#define drv_ms2		A4
#define drv_enable	A5



// Define Microstepping
//MS1	MS2	Microstep Resolution
//L		L	Full Step (2 Phase)
//H		L	Half Step
//L		H	Quarter Step
//H		H	Eigth Step
#define drv_ms1_set		HIGH
#define drv_ms2_set		HIGH
// ------------------------------------ //


// -------------------------------------------- //
// ----- Movement Calculations & Settings ----- //
#define stepper_default_speed 1000
#define stepper_default_acc   5000


// Threaded rod is probably 20tpi = 7.874015 thread/cm
// motor: 1.8deg/step
// configuration: Eighth of a step
// 1 step = 1.8/8 = 0.225deg
// 1600 steps per rev
// 1600 * 7.874015 = 12,598.425 steps per cm
#define CALC_ROD_THREADING		7.874015			// threads/cm
#define CALC_DEG_PER_STEP		0.225				// deg/step
#define CALC_STEPS_PER_CM		( (360.0/CALC_DEG_PER_STEP) * CALC_ROD_THREADING )
#define CALC_CM_PER_CC			( 9.0 / 50 )			// 90mm/50cc
#define CALC_CC_PER_STEP		( 1.0 / (CALC_STEPS_PER_CM * CALC_CM_PER_CC) )

#define SYRINGE_MAX_VOL_CC		8.0 / CALC_CM_PER_CC	// cc, actual total movable length is 107mm
// -------------------------------------------- //


#define prg_osc_phaseDuration_default   30 // seconds
#define prg_osc_volume_default          40 // mL
#define prg_osc_isPushing_default       true // 

#define prg_push_duration_default       30 // seconds
#define prg_push_volume_default         40 // mL
#define prg_push_status_default         false // false = 'off', true = 'on'

#define prg_pull_duration_default       30 // seconds
#define prg_pull_volume_default         40 // mL
#define prg_pull_status_default         false // false = 'off', true = 'on'

#define displayBrightness_default		2

#define VOLUME_INCREMENT				5 //mL
#define VOLUME_MAX						50 //mL
#define VOLUME_MIN						5 //mL
#define VOLUME_DIGITS					3 // (including '-' sign)
#define DURATION_INCREMENT				5 //s
#define DURATION_MAX					2700 //s
#define DURATION_MIN					15 //s
#define DURATION_DIGITS					4 





class Config
{
  public:
    char appNameAndVersion[sizeof(NameAndVersion)];
	char *getSettingStr(byte cmdId);

	Config(void);
	
	float stepUpOscVol(void);
	float stepDnOscVol(void);
	float getOscVol(void);
	
	unsigned long stepUpOscDur(void);
	unsigned long stepDnOscDur(void);
	unsigned long getOscDur(void);
	
	void startOsc(void);
	void stopOsc(void);
	void continueOsc(void);
	
	
	float stepUpPushVol(void);
	float stepDnPushVol(void);
	float getPushVol(void);
	
	unsigned long stepUpPushDur(void);
	unsigned long stepDnPushDur(void);
	unsigned long getPushDur(void);
	
	void startPush(void);
	void stopPush(void);
	
	
	float stepUpPullVol(void);
	float stepDnPullVol(void);
	float getPullVol(void);
	
	unsigned long stepUpPullDur(void);
	unsigned long stepDnPullDur(void);
	unsigned long getPullDur(void);
	
	void startPull(void);
	void stopPull(void);
	
	
	bool isMotionActive(void);
	
	
	
    //char *getFormattedStr(byte cmdId);  // Returns formatted config value associated with menu command id.

    void save(void);                        // Saves config to EEPROM.
    void load(void);                        // Loads config from EEPROM.
    void setDefaults(void);                 // Sets config to default values.
    void copyTo(Config *dest);				// Copies current instance to destination instance.
	
	void debugPrintState(void);
	
  private:
	AccelStepper *stepper;
	
	void stepUpVol(float *vol);		//mL
	void stepDnVol(float *vol);		//mL
	void stepUpDur(unsigned long *dur);		//s
	void stepDnDur(unsigned long *dur);		//s
	
	void enable_stepper(void);
	void disable_stepper(void);
	bool isVolTooLarge(float vol_cc);
	void inject_cc(float vol_cc, unsigned long duration);
	void retract_cc(float vol_cc, unsigned long duration);
	float calcStepperSpeed(unsigned long volume, unsigned long duration);
	
	void msgERR(String messageText);
	void msgAction(String messageText);
	void msgLapse(unsigned long last_micros, unsigned long setting);
	
	bool activeMotion;
	unsigned long motionUnit_time_last;

	
	unsigned long   prg_osc_phaseDuration;
	float			prg_osc_volume;
	bool			prg_osc_isPushing;  // either pushing or pulling
	bool			prg_osc_status;
	

	unsigned long   prg_push_duration;
	float			prg_push_volume;
	bool			prg_push_status;

	unsigned long   prg_pull_duration;
	float			prg_pull_volume;
	bool			prg_pull_status;
	
	byte			displayBrightness;   // 1=25%, 2=50%, 3=75%, 4=100%
};

#endif

