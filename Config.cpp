#include "Config.h"
#include "LcdKeypad.h"
#include "MenuData.h"
#include <avr/eeprom.h>

const char NotImp[] = "Not Implemented";
static char strbuf[LCD_COLS+1];

Config::Config(void) {
	setDefaults();
}
  


void Config::stepUpVol(long *vol) {
	*vol += VOLUME_INCREMENT;
	*vol = constrain(*vol, VOLUME_MIN, VOLUME_MAX);
}
void Config::stepDnVol(long *vol) {
	*vol -= VOLUME_INCREMENT;
	*vol = constrain(*vol, VOLUME_MIN, VOLUME_MAX);
}
void Config::stepUpDur(long *dur) {
	*dur += DURATION_INCREMENT;
	*dur = constrain(*dur, DURATION_MIN, DURATION_MAX);
}
void Config::stepDnDur(long *dur) {
	*dur -= DURATION_INCREMENT;
	*dur = constrain(*dur, DURATION_MIN, DURATION_MAX);
}
  

// ----- Oscillating: Volume ----- //
long Config::stepUpOscVol(void) {
	stepUpVol(&prg_osc_volume);
	return prg_osc_volume;
}
long Config::stepDnOscVol(void) {
	stepDnVol(&prg_osc_volume);
	return prg_osc_volume;
}
long Config::getOscVol(void) {
	return prg_osc_volume;
}
// --------------------------------- //


// ----- Oscillating: Duration ----- //
long Config::stepUpOscDur(void) {
	stepUpDur(&prg_osc_phaseDuration);
	return prg_osc_phaseDuration;
}
long Config::stepDnOscDur(void) {
	stepDnDur(&prg_osc_phaseDuration);
	return prg_osc_phaseDuration;
}
long Config::getOscDur(void) {
	return prg_osc_phaseDuration;
}
// --------------------------------- //


// ----- Push: Volume ----- //
long Config::stepUpPushVol(void) {
	stepUpVol(&prg_push_volume);
	return prg_push_volume;
}
long Config::stepDnPushVol(void) {
	stepDnVol(&prg_push_volume);
	return prg_push_volume;
}
long Config::getPushVol(void) {
	return prg_push_volume;
}
// --------------------------------- //


// ----- Push: Duration ----- //
long Config::stepUpPushDur(void) {
	stepUpDur(&prg_push_duration);
	return prg_push_duration;
}
long Config::stepDnPushDur(void) {
	stepDnDur(&prg_push_duration);
	return prg_push_duration;
}
long Config::getPushDur(void) {
	return prg_push_duration;
}
// --------------------------------- //


// ----- Pull: Volume ----- //
long Config::stepUpPullVol(void) {
	stepUpVol(&prg_pull_volume);
	return prg_pull_volume;
}
long Config::stepDnPullVol(void) {
	stepDnVol(&prg_pull_volume);
	return prg_pull_volume;
}
long Config::getPullVol(void) {
	return prg_pull_volume;
}
// --------------------------------- //


// ----- Pull: Duration ----- //
long Config::stepUpPullDur(void) {
	stepUpDur(&prg_pull_duration);
	return prg_pull_duration;
}
long Config::stepDnPullDur(void) {
	stepDnDur(&prg_pull_duration);
	return prg_pull_duration;
}
long Config::getPullDur(void) {
	return prg_pull_duration;
}
// --------------------------------- //



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
//char *Config::getFormattedStr(byte cmdId)
//{
  //char intbuf[8];
  //
  //// switch (cmdId)
  //// {
  ////   case mnuCmdT1Hours :
  ////   case mnuCmdT1Mins :
  ////   case mnuCmdT1Secs :
  ////     toTimeStr(strbuf, timer1ReloadValue);
  ////     break;
  ////   case mnuCmdT2Hours :
  ////   case mnuCmdT2Mins :
  ////   case mnuCmdT2Secs :
  ////     toTimeStr(strbuf, timer2ReloadValue);
  ////     break;
  ////   case mnuCmdT3Hours :
  ////   case mnuCmdT3Mins :
  ////   case mnuCmdT3Secs :
  ////     toTimeStr(strbuf, timer3ReloadValue);
  ////     break;
  ////   case mnuCmdResetToDefaults:
  ////     strbuf[0] = 0;
  ////     break;
  ////   case mnuCmdAlarmDuration:
  ////     fmt(strbuf, 2, inttostr(intbuf, alarmDuration), " seconds");
  ////     break;
  ////   case mnuCmdButtonBeep :
  ////     if (buttonBeep)
  ////     {
  ////       fmt(strbuf, 1, "on");
  ////     }
  ////     else
  ////     {
  ////       fmt(strbuf, 1, "off");
  ////     }
  ////     break;
  ////   case mnuCmdDisplayBrightness :
  ////   {
  ////     byte brightnessPC = ((unsigned short) displayBrightness * 100) / 4;
  ////     inttostr(intbuf, brightnessPC);
  ////     fmt(strbuf, 2, intbuf, "%");
  ////     break;
  ////   }
  ////   default:
  ////     strcpy(strbuf, NotImp);
  ////     break;
  //// }
   //return strbuf;
//}


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
  prg_osc_status = prg_osc_status_default;

  prg_push_duration = prg_push_duration_default;
  prg_push_volume = prg_push_volume_default;
  prg_push_status = prg_push_status_default;

  prg_pull_duration = prg_pull_duration_default;
  prg_pull_volume = prg_pull_volume_default;
  prg_pull_status = prg_pull_status_default;
  
  displayBrightness = displayBrightness_default;

}


//------------------------------------------------------------------------------
void Config::copyTo(Config *dest)
{
  memcpy(dest, this, sizeof(Config));
}

