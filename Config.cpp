#include "Config.h"
#include "LcdKeypad.h"
#include "MenuData.h"
#include <avr/eeprom.h>
#include <AccelStepper.h>

const char NotImp[] = "Not Implemented";
static char strbuf[LCD_COLS+1];

Config::Config(void) {
	setDefaults();
	
	// ----------------------- //
	// ----- Motor Setup ----- //
			
	// Configure motor pins as outputs
	pinMode(drv_step,	OUTPUT);
	pinMode(drv_dir,	OUTPUT);
	pinMode(drv_ms1,	OUTPUT);
	pinMode(drv_ms2,	OUTPUT);
	pinMode(drv_enable, OUTPUT);
			
	// Set microstepping configuration described above
	digitalWrite(drv_ms1, drv_ms1_set);
	digitalWrite(drv_ms2, drv_ms2_set);
			
	
			
	stepper = new AccelStepper(AccelStepper::DRIVER, drv_step, drv_dir);
	
	stepper->setMaxSpeed(stepper_default_speed);
	stepper->setAcceleration(stepper_default_acc);
	// ----------------------- //
	
}
  

void Config::stepUpVol(float *vol) {
	*vol += VOLUME_INCREMENT;
	*vol = constrain(*vol, VOLUME_MIN, VOLUME_MAX);
}
void Config::stepDnVol(float *vol) {
	*vol -= VOLUME_INCREMENT;
	*vol = constrain(*vol, VOLUME_MIN, VOLUME_MAX);
}
void Config::stepUpDur(unsigned long *dur) {
	*dur += DURATION_INCREMENT;
	*dur = constrain(*dur, DURATION_MIN, DURATION_MAX);
}
void Config::stepDnDur(unsigned long *dur) {
	*dur -= DURATION_INCREMENT;
	*dur = constrain(*dur, DURATION_MIN, DURATION_MAX);
}
  

// ----- Oscillating: Volume ----- //
float Config::stepUpOscVol(void) {
	stepUpVol(&prg_osc_volume);
	return prg_osc_volume;
}
float Config::stepDnOscVol(void) {
	stepDnVol(&prg_osc_volume);
	return prg_osc_volume;
}
float Config::getOscVol(void) {
	return prg_osc_volume;
}
// --------------------------------- //

// ----- Oscillating: Duration ----- //
unsigned long Config::stepUpOscDur(void) {
	stepUpDur(&prg_osc_phaseDuration);
	return prg_osc_phaseDuration;
}
unsigned long Config::stepDnOscDur(void) {
	stepDnDur(&prg_osc_phaseDuration);
	return prg_osc_phaseDuration;
}
unsigned long Config::getOscDur(void) {
	return prg_osc_phaseDuration;
}
// --------------------------------- //

// ----- Oscillating: Actions ----- //
void Config::startOsc(void) {
	prg_osc_status = true;
	activeMotion = true;
	prg_osc_isPushing = true;  // always start with pushing
	
	enable_stepper();
}
void Config::stopOsc(void) {
	prg_osc_status = false;
	activeMotion = false;
	
	disable_stepper();
}
void Config::continueOsc(void) {
	// doublecheck motion is meant to be active
	if (activeMotion) {
		
		if (prg_osc_isPushing) {
			// PUSHING
			
			inject_cc(prg_osc_volume, prg_osc_phaseDuration);
			prg_osc_isPushing = false;
		} else {
			// PULLING
			retract_cc(prg_osc_volume, prg_osc_phaseDuration);
			prg_osc_isPushing = true;
		}
		
		
	} else {
		msgERR(F("continueOsc when activeMotion false"));
		stopOsc();
		
	}
}
// --------------------------------- //



// ----- Push: Volume ----- //
float Config::stepUpPushVol(void) {
	stepUpVol(&prg_push_volume);
	return prg_push_volume;
}
float Config::stepDnPushVol(void) {
	stepDnVol(&prg_push_volume);
	return prg_push_volume;
}
float Config::getPushVol(void) {
	return prg_push_volume;
}
// --------------------------------- //

// ----- Push: Duration ----- //
unsigned long Config::stepUpPushDur(void) {
	stepUpDur(&prg_push_duration);
	return prg_push_duration;
}
unsigned long Config::stepDnPushDur(void) {
	stepDnDur(&prg_push_duration);
	return prg_push_duration;
}
unsigned long Config::getPushDur(void) {
	return prg_push_duration;
}
// --------------------------------- //

// ----- Push: Actions ----- //
void Config::startPush(void) {
	prg_push_status = true;
	activeMotion = true;
	
	if (activeMotion)
	{
		enable_stepper();
		inject_cc(prg_push_volume, prg_push_duration);
		disable_stepper();
	}
	
	stopPush();
}
void Config::stopPush(void) {
	prg_push_status = false;
	activeMotion = false;
	
	disable_stepper();
}
// --------------------------------- //


// ----- Pull: Volume ----- //
float Config::stepUpPullVol(void) {
	stepUpVol(&prg_pull_volume);
	return prg_pull_volume;
}
float Config::stepDnPullVol(void) {
	stepDnVol(&prg_pull_volume);
	return prg_pull_volume;
}
float Config::getPullVol(void) {
	return prg_pull_volume;
}
// --------------------------------- //

// ----- Pull: Duration ----- //
unsigned long Config::stepUpPullDur(void) {
	stepUpDur(&prg_pull_duration);
	return prg_pull_duration;
}
unsigned long Config::stepDnPullDur(void) {
	stepDnDur(&prg_pull_duration);
	return prg_pull_duration;
}
unsigned long Config::getPullDur(void) {
	return prg_pull_duration;
}
// --------------------------------- //

// ----- Pull: Actions ----- //
void Config::startPull(void) {
	prg_pull_status = true;
	activeMotion = true;

	if (activeMotion)
	{
		enable_stepper();
		retract_cc(prg_pull_volume, prg_pull_duration);
		disable_stepper();
	}
	
	stopPush();
}
void Config::stopPull(void) {
	prg_pull_status = false;
	activeMotion = false;
	
	disable_stepper();
}
// --------------------------------- //


// ----------------------------------- //
// ----- Motor Control Functions ----- //
// ----------------------------------- //
void Config::enable_stepper() {
	digitalWrite(drv_enable, LOW);
}

void Config::disable_stepper() {
	digitalWrite(drv_enable, HIGH);
}

bool Config::isVolTooLarge (float vol_cc) {
	if (vol_cc > SYRINGE_MAX_VOL_CC) {
		Serial.print(F("ERROR - Injectable Volume is too Large ("));
		Serial.print(vol_cc);
		Serial.print(">");
		Serial.print(SYRINGE_MAX_VOL_CC);
		Serial.println(")");
		return true;
		} else {
		return false;
	}
}

void Config::inject_cc(float vol_cc, unsigned long duration) {
	if (!isVolTooLarge(vol_cc)) {
		stepper->setMaxSpeed(calcStepperSpeed(vol_cc, duration));
		
		// zero out current position
		stepper->setCurrentPosition(0);
		
		// Output action to Serial Monitor
		String temp = F(" - Injecting (mL): ");
		msgAction(temp + vol_cc);
		
		// move to new position represent specified volume
		float ccPerStep = CALC_CC_PER_STEP;
		enable_stepper();
		motionUnit_time_last = micros();
		stepper->runToNewPosition(round(vol_cc / ccPerStep));
		disable_stepper();
		
		// determine elapsed time and output
		msgLapse(motionUnit_time_last, prg_osc_phaseDuration);
	}
}

void Config::retract_cc(float vol_cc, unsigned long duration) {
	if (!isVolTooLarge(vol_cc)) {
		stepper->setMaxSpeed(calcStepperSpeed(vol_cc, duration));
		
		// zero out current position
		stepper->setCurrentPosition(0);
		
		// Output action to Serial Monitor
		String temp = F(" - Retracting (mL): ");
		msgAction(temp + vol_cc);
		
		// move to new position represent specified volume
		float ccPerStep = CALC_CC_PER_STEP;
		enable_stepper();
		motionUnit_time_last = micros();
		stepper->runToNewPosition( -round(vol_cc / ccPerStep));
		disable_stepper();
		
		// determine elapsed time and output
		msgLapse(motionUnit_time_last, prg_osc_phaseDuration);
	}
}

float Config::calcStepperSpeed(unsigned long volume, unsigned long duration) {
	// Calculate speed to pass to AccelStepper based on duration of injection
	// Empirically measured time that it takes to execute motion which is a little longer than the setting
	// measured: 31.11, 31.13, 31.12, 31.13, 31.12, 31.13, 31.12, 31.14, 31.12 [setting: 30s]
	
	float j = 360.0;
	j /= CALC_DEG_PER_STEP;		// gives the steps needed per revolution
	j *= CALC_ROD_THREADING;	// finds steps per cm movement of syringe
	j *= CALC_CM_PER_CC;		// converts to steps per volume in CC injected
	j *= volume;				// determines steps needed for a given volume 
	j /= duration;				// finds the number of steps per sec to achieve injected rate
	return j;	// returns rate in steps/sec
	//return (360.0/CALC_DEG_PER_STEP) * CALC_ROD_THREADING * CALC_CM_PER_CC * volume / duration;
}

void Config::msgERR(String messageText) {
	Serial.print(F(">ERROR: "));
	Serial.println(messageText);
}

void Config::msgAction(String messageText) {
	Serial.print(F("> "));
	Serial.println(messageText);
}

void Config::msgLapse(unsigned long last_micros, unsigned long setting) {
	Serial.print(F(" - - elapsed: "));
	Serial.print((micros() - last_micros)/1000000.0);
	Serial.print(F("s "));
	Serial.print(F(" [setting: "));
	Serial.print(setting);
	Serial.println(F("s]"));
}

bool Config::isMotionActive(void) {
	return activeMotion;
}

char *Config::getSettingStr(byte cmdId) {
	switch (cmdId) {
		case mnuCmdprg_oscillating_vol:
		{
			//char strSize[VOLUME_DIGITS];
			//strbuf = *itoa(prg_osc_volume, strSize, 10);
			inttostr(strbuf, prg_osc_volume);
			break;
		}
		case mnuCmdprg_oscillating_dur:
		{
			inttostr(strbuf, prg_osc_phaseDuration);
			break;
		}
		case mnuCmdprg_push_vol:
		{
			inttostr(strbuf, prg_push_volume);
			break;
		}
		case mnuCmdprg_push_dur:
		{
			inttostr(strbuf, prg_push_duration);
			break;
		}
		case mnuCmdprg_pull_vol:
		{
			inttostr(strbuf, prg_pull_volume);
			break;
		}
		case mnuCmdprg_pull_dur:
		{
			inttostr(strbuf, prg_pull_duration);
			break;
		}
		case mnuCmdsettings_resetToDefaults:
		{
			break;
		}
		default:
		{
			strcpy(strbuf, NotImp);
			break;
		}
	}
	return strbuf;
}

//------------------------------------------------------------------------------
void Config::save(void)
{
  eeprom_write_block(this, (void *)0, sizeof (Config));
}

//------------------------------------------------------------------------------
void Config::load(void)
{
  // Attempt to load config from EEPROM
  eeprom_read_block(this, (void *)0, sizeof (Config));

  if (strcmp(this->appNameAndVersion, NameAndVersion) != 0)
  {
    setDefaults();
  }
}

//------------------------------------------------------------------------------
void Config::setDefaults(void)
{
  strcpy(appNameAndVersion, NameAndVersion);

  prg_osc_phaseDuration = prg_osc_phaseDuration_default;
  prg_osc_volume = prg_osc_volume_default;
  prg_osc_isPushing = prg_osc_isPushing_default;

  prg_push_duration = prg_push_duration_default;
  prg_push_volume = prg_push_volume_default;
  prg_push_status = prg_push_status_default;

  prg_pull_duration = prg_pull_duration_default;
  prg_pull_volume = prg_pull_volume_default;
  prg_pull_status = prg_pull_status_default;
  
  displayBrightness = displayBrightness_default;
  
  activeMotion = false;
  
  motionUnit_time_last = 0;
}

//------------------------------------------------------------------------------
void Config::copyTo(Config *dest)
{
  memcpy(dest, this, sizeof(Config));
}

//------------------------------------------------------------------------------
void Config::debugPrintState(void)
{
	Serial.println("-----------------------------");
	
	// Oscillating
	Serial.print("Osc:  ");
	if (prg_osc_status) {
		Serial.print("[ON]  ");
	} else {
		Serial.print("[OFF] ");
	}
	Serial.print(prg_osc_volume);
	Serial.print("mL, for ");
	Serial.print(prg_osc_phaseDuration);
	Serial.println("s");
	
	// Push
	Serial.print("Push: ");
	if (prg_push_status) {
		Serial.print("[ON]  ");
		} else {
		Serial.print("[OFF] ");
	}
	Serial.print(prg_push_volume);
	Serial.print("mL, for ");
	Serial.print(prg_push_duration);
	Serial.println("s");
	
	// Pull
	Serial.print("Pull: ");
	if (prg_pull_status) {
		Serial.print("[ON]  ");
		} else {
		Serial.print("[OFF] ");
	}
	Serial.print(prg_pull_volume);
	Serial.print("mL, for ");
	Serial.print(prg_pull_duration);
	Serial.println("s");
	
	Serial.println("-----------------------------");
}