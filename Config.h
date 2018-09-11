#ifndef CONFIG_H_
#define CONFIG_H_

#include "Arduino.h"

const char softwareName[] = "MR Flow Phantom ";
const char softwareVersion[]  = "v1.1";
const char NameAndVersion[] = "MR Flow Phantom v1.1";



#define prg_osc_phaseDuration_default   90 // seconds
#define prg_osc_volume_default          50 // mL
#define prg_osc_status_default          false // false = 'off', true = 'on'

#define prg_push_duration_default       90 // seconds
#define prg_push_volume_default         50 // mL
#define prg_push_status_default         false // false = 'off', true = 'on'

#define prg_pull_duration_default       90 // seconds
#define prg_pull_volume_default         50 // mL
#define prg_pull_status_default         false // false = 'off', true = 'on'

#define displayBrightness_default		2

#define VOLUME_INCREMENT				5 //mL
#define VOLUME_MAX						50 //mL
#define VOLUME_MIN						5 //mL
#define VOLUME_DIGITS					3 // (including '-' sign)
#define DURATION_INCREMENT				30 //s
#define DURATION_MAX					2700 //s
#define DURATION_MIN					30 //s
#define DURATION_DIGITS					4 


class Config
{
  public:
    char appNameAndVersion[sizeof(NameAndVersion)];
  



	Config(void);
	
	long stepUpOscVol(void);
	long stepDnOscVol(void);
	long getOscVol(void);
	
	long stepUpOscDur(void);
	long stepDnOscDur(void);
	long getOscDur(void);
	
	long stepUpPushVol(void);
	long stepDnPushVol(void);
	long getPushVol(void);
	
	long stepUpPushDur(void);
	long stepDnPushDur(void);
	long getPushDur(void);
	
	long stepUpPullVol(void);
	long stepDnPullVol(void);
	long getPullVol(void);
	
	long stepUpPullDur(void);
	long stepDnPullDur(void);
	long getPullDur(void);
	
	char *getSettingStr(byte cmdId);

    
    //char *getFormattedStr(byte cmdId);  // Returns formatted config value associated with menu command id.

    void save(void);                        // Saves config to EEPROM.
    void load(void);                        // Loads config from EEPROM.
    void setDefaults(void);                 // Sets config to default values.
    void copyTo(Config *dest);				// Copies current instance to destination instance.
	
  private:
	void stepUpVol(long *vol);		//mL
	void stepDnVol(long *vol);		//mL
	
	void stepUpDur(long *dur);		//s
	void stepDnDur(long *dur);		//s

	byte		displayBrightness;   // 1=25%, 2=50%, 3=75%, 4=100%
	
	long        prg_osc_phaseDuration;
	long		prg_osc_volume;
	bool        prg_osc_status;

	long        prg_push_duration;
	long		prg_push_volume;
	bool        prg_push_status;

	long        prg_pull_duration;
	long		prg_pull_volume;
	bool        prg_pull_status;
};

#endif

