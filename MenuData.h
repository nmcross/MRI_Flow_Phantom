#ifndef _flowPhantomMenu_
#define _flowPhantomMenu_
#include "MenuManager.h"
#include <avr/pgmspace.h>

/*
Generated using LCD Menu Builder at http://lcd-menu-bulder.cohesivecomputing.co.uk/.
*/

enum flowPhantomMenuCommandId
{
  mnuCmdBack = 0,
  mnuCmdprg,
  mnuCmdprg_oscillating,
  mnuCmdprg_oscillating_vol,
  mnuCmdprg_oscillating_dur,
  mnuCmdprg_oscillating_start,
  mnuCmdprg_one_Push,
  mnuCmdprg_push_vol,
  mnuCmdprg_push_dur,
  mnuCmdprg_push_start,
  mnuCmdprg_one_Pull,
  mnuCmdprg_pull_vol,
  mnuCmdprg_pull_dur,
  mnuCmdprg_pull_start,
  mnuCmdsettings,
  mnuCmdsettings_idk,
  mnuCmdsettings_resetToDefaults
};

PROGMEM const char flowPhantomMenu_back[] = "Back";
PROGMEM const char flowPhantomMenu_exit[] = "Exit";

PROGMEM const char flowPhantomMenu_1_1_1[] = "1.1.1 Volume(mL";
PROGMEM const char flowPhantomMenu_1_1_2[] = "1.1.2 Phase Dur";
PROGMEM const char flowPhantomMenu_1_1_3[] = "1.1.3 StartOsc!";
PROGMEM const MenuItem flowPhantomMenu_List_1_1[] = {{mnuCmdprg_oscillating_vol, flowPhantomMenu_1_1_1}, {mnuCmdprg_oscillating_dur, flowPhantomMenu_1_1_2}, {mnuCmdprg_oscillating_start, flowPhantomMenu_1_1_3}, {mnuCmdBack, flowPhantomMenu_back}};

PROGMEM const char flowPhantomMenu_1_2_1[] = "1.2.1 Volume(mL";
PROGMEM const char flowPhantomMenu_1_2_2[] = "1.2.2 Time(min)";
PROGMEM const char flowPhantomMenu_1_2_3[] = "1.2.3 StartPush";
PROGMEM const MenuItem flowPhantomMenu_List_1_2[] = {{mnuCmdprg_push_vol, flowPhantomMenu_1_2_1}, {mnuCmdprg_push_dur, flowPhantomMenu_1_2_2}, {mnuCmdprg_push_start, flowPhantomMenu_1_2_3}, {mnuCmdBack, flowPhantomMenu_back}};

PROGMEM const char flowPhantomMenu_1_3_1[] = "1.3.1 Volume(mL";
PROGMEM const char flowPhantomMenu_1_3_2[] = "1.3.2 Time(min)";
PROGMEM const char flowPhantomMenu_1_3_3[] = "1.3.3 StartPull";
PROGMEM const MenuItem flowPhantomMenu_List_1_3[] = {{mnuCmdprg_pull_vol, flowPhantomMenu_1_3_1}, {mnuCmdprg_pull_dur, flowPhantomMenu_1_3_2}, {mnuCmdprg_pull_start, flowPhantomMenu_1_3_3}, {mnuCmdBack, flowPhantomMenu_back}};

PROGMEM const char flowPhantomMenu_1_1[] = "1.1 Oscillating";
PROGMEM const char flowPhantomMenu_1_2[] = "1.2 One Push";
PROGMEM const char flowPhantomMenu_1_3[] = "1.3 One Pull";
PROGMEM const MenuItem flowPhantomMenu_List_1[] = {{mnuCmdprg_oscillating, flowPhantomMenu_1_1, flowPhantomMenu_List_1_1, menuCount(flowPhantomMenu_List_1_1)}, {mnuCmdprg_one_Push, flowPhantomMenu_1_2, flowPhantomMenu_List_1_2, menuCount(flowPhantomMenu_List_1_2)}, {mnuCmdprg_one_Pull, flowPhantomMenu_1_3, flowPhantomMenu_List_1_3, menuCount(flowPhantomMenu_List_1_3)}, {mnuCmdBack, flowPhantomMenu_back}};

PROGMEM const char flowPhantomMenu_2_1[] = "2.1 not sure wh";
PROGMEM const char flowPhantomMenu_2_2[] = "2.2 Reset";
PROGMEM const MenuItem flowPhantomMenu_List_2[] = {{mnuCmdsettings_idk, flowPhantomMenu_2_1}, {mnuCmdsettings_resetToDefaults, flowPhantomMenu_2_2}, {mnuCmdBack, flowPhantomMenu_back}};

PROGMEM const char flowPhantomMenu_1[] = "1 Programs";
PROGMEM const char flowPhantomMenu_2[] = "2 Settings";
PROGMEM const MenuItem flowPhantomMenu_Root[] = {{mnuCmdprg, flowPhantomMenu_1, flowPhantomMenu_List_1, menuCount(flowPhantomMenu_List_1)}, {mnuCmdsettings, flowPhantomMenu_2, flowPhantomMenu_List_2, menuCount(flowPhantomMenu_List_2)}, {mnuCmdBack, flowPhantomMenu_exit}};

/*
case mnuCmdprg_oscillating_vol :
	break;
case mnuCmdprg_oscillating_dur :
	break;
case mnuCmdprg_oscillating_start :
	break;
case mnuCmdprg_push_vol :
	break;
case mnuCmdprg_push_dur :
	break;
case mnuCmdprg_push_start :
	break;
case mnuCmdprg_pull_vol :
	break;
case mnuCmdprg_pull_dur :
	break;
case mnuCmdprg_pull_start :
	break;
case mnuCmdsettings_idk :
	break;
case mnuCmdsettings_resetToDefaults :
	break;
*/

/*
<?xml version="1.0"?>
<RootMenu xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <Config IdPrefix="mnuCmd" 
          VarPrefix="flowPhantomMenu" 
          UseNumbering="true" 
          IncludeNumberHierarchy="true"
          MaxNameLen="15" 
          MenuBackFirstItem="false" 
          BackText="Back" 
          ExitText="Exit" 
          AvrProgMem="true"/>
  <MenuItems>

    <Item Id="prg" Name="Programs">
      <MenuItems>
        <Item Id="prg_oscillating" Name="Oscillating">
          <MenuItems>
            <Item Id="prg_oscillating_vol" Name="Volume(mL)"/>
            <Item Id="prg_oscillating_dur" Name="Phase Dur(min)"/>
            <Item Id="prg_oscillating_start" Name="StartOsc!"/>
          </MenuItems>
        </Item>

        <Item Id="prg_one_Push" Name="One Push">
          <MenuItems>
            <Item Id="prg_push_vol" Name="Volume(mL)"/>
            <Item Id="prg_push_dur" Name="Time(min)"/>
            <Item Id="prg_push_start" Name="StartPush!"/>
          </MenuItems>
        </Item>
        
        <Item Id="prg_one_Pull" Name="One Pull">
          <MenuItems>
            <Item Id="prg_pull_vol" Name="Volume(mL)"/>
            <Item Id="prg_pull_dur" Name="Time(min)"/>
            <Item Id="prg_pull_start" Name="StartPull!"/>
          </MenuItems>
        </Item>
      </MenuItems>
    </Item>

    <Item Id="settings" Name="Settings">
      <MenuItems>
        <Item Id="settings_idk" Name="not sure what should go here..."/>
        <Item Id="settings_resetToDefaults" Name="Reset"/>
      </MenuItems>
    </Item>

  </MenuItems>
</RootMenu>
*/
#endif
