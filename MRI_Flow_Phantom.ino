
/*************************************************************************************
MRI Flow Phantom
(c) 2018 Nathan M. Cross

References:
https://www.dfrobot.com/wiki/index.php/LCD_KeyPad_Shield_For_Arduino_SKU:_DFR0009
https://learn.sparkfun.com/tutorials/easy-driver-hook-up-guide?_ga=2.110778565.153953877.1533438339-555965987.1530884463
http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html

	Menus:
	https://eeenthusiast.com/arduino-lcd-tutorial-display-menu-system-scrolling-menu-changeable-variables-projects/
	http://www.cohesivecomputing.co.uk/hackatronics/arduino-lcd-menu-library/

TODO: 
- started to add code for manipulating config which stores the settings for the device
- need to finish code for starting and stopping functions
- need to dev code for displaying settings on screen
- Change to push on push off button control
- ensure push is first action in oscillation to prevent wet mess
**************************************************************************************/




// ============================================================================= //
// ===== INCLUDES ============================================================== //

  	//#include <AccelStepper/AccelStepper.h>
  	#include <AccelStepper.h>
	//#include "AccelStepper.h"
	//#include <AccelStepper/MultiStepper.h>
	#include <LiquidCrystal.h>
	#include "LcdKeypad.h"
	#include "MenuData.h"
	#include "TimerOne.h"
	#include "Config.h"

// ============================================================================= //




// ============================================================================= //
// ===== CONFIGURATIONS ======================================================== //

	// ---------------------------------------- //
	// ----- System Current Configuration ----- //
		Config currentConfig;
		//currentConfig.load();
	// ---------------------------------------- //
	


	// ------------------------------------- //
	// ----- DF Robot LCD Keypad Setup ----- //
		LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
	// ------------------------------------- //



	// ---------------------------------- //
	// ----- LCD Menu Configuration ----- //
		enum AppModeValues
		{
			APP_NORMAL_MODE,
			APP_MENU_MODE,
			APP_MENU_MODE_END,
			APP_PROCESS_MENU_CMD,
			APP_PROGRAM_RUNNING
		};

		byte appMode = APP_NORMAL_MODE;

		MenuManager Menu1(flowPhantomMenu_Root, menuCount(flowPhantomMenu_Root));

		char strbuf[LCD_COLS + 1]; // one line of lcd display
		byte btn;
	// ---------------------------------- //



	



	
// ============================================================================= //


bool temp = false;


// ============================================================================= //
// ===== SETUP ================================================================= //

	void setup(){
	
		
		// ------------------------------ //
		// ----- LCD Welcome Screen ----- //
		
			backLightOn();
			// set up the LCD's number of columns and rows:
			lcd.begin(LCD_COLS, LCD_ROWS);

			// Write welcome message
			lcd.setCursor(0,0);             // set the LCD cursor position
			lcd.print("MR Flow Phantom");
			lcd.setCursor(5,1);
			lcd.print(softwareVersion);
			delay(3000);
			
		// ------------------------------ //
			
		
		
		// -------------------------- //
		// ----- Serial Monitor ----- //

			// Start Serial Monitor Output
			Serial.begin(115200); //Open Serial connection for debugging
			Serial.println("-----------------------------");
			Serial.print("MR Flow Phantom v");
			Serial.println(softwareVersion);
			Serial.println();
			
		// -------------------------- //
	
	
		// ------------------------------ //
		// ----- Menu Manager Setup ----- //
		
			// clear welcome message
			lcd.clear();
		
			// fall in to menu mode by default.
			appMode = APP_MENU_MODE;
			refreshMenuDisplay(REFRESH_DESCEND);

			// Use soft PWM for backlight, as hardware PWM must be avoided for some LCD shields.
			Timer1.initialize();
			Timer1.attachInterrupt(lcdBacklightISR, 500);
			setBacklightBrightness(4);
			
			lcd.setCursor(0,1);             // set the LCD cursor position
			lcd.print("MR Flow Phantom");
		
		// ------------------------------ //
	
	}

// ============================================================================= //
 
 
 
 
// ============================================================================= //
// ===== MAIN LOOP ============================================================= //

	float vol = 0.3; // (mL)
	bool process_menu_cmd_justCalled = false;
	
	void loop(){
		
		btn = getButton();

		switch (appMode)
		{
			case APP_NORMAL_MODE :
				if (btn == BUTTON_UP_LONG_PRESSED)
				{
					appMode = APP_MENU_MODE;
					refreshMenuDisplay(REFRESH_DESCEND);
				}
				break;
			case APP_MENU_MODE :
				{
					byte menuMode = Menu1.handleNavigation(getNavAction, refreshMenuDisplay);

					if (menuMode == MENU_EXIT)
					{
						lcd.clear();
						lcd.setCursor(0,0);
						lcd.print("MR Flow Phantom");
						lcd.setCursor(0,1);
						lcd.print("Hold UP for menu");
						appMode = APP_MENU_MODE_END;
						
					}
					else if (menuMode == MENU_INVOKE_ITEM)
					{
						appMode = APP_PROCESS_MENU_CMD;
						process_menu_cmd_justCalled = true;

						// Indicate selected item.
						if (Menu1.getCurrentItemCmdId())
						{
							lcd.setCursor(0, 1);
							strbuf[0] = 0b01111110; // forward arrow representing input prompt.
							strbuf[1] = 0;
							lcd.print(strbuf);
							if (Menu1.getCurrentItemIndex()==2)
							{
								lcd.setCursor(2,1);
								lcd.print(F("          "));
								lcd.setCursor(2,1);
								lcd.print(F("ACTIVE!!!"));
							}
						} 
					}
					break;
				}
			case APP_PROCESS_MENU_CMD :
				{
					byte processingComplete = processMenuCommand(Menu1.getCurrentItemCmdId());

					if (processingComplete)
					{
						appMode = APP_MENU_MODE;
						process_menu_cmd_justCalled = false;
						// clear forward arrow
						lcd.setCursor(0, 1);
						strbuf[0] = ' '; // clear forward arrow
						strbuf[1] = 0;
						lcd.print(strbuf);
					}
					break;
				}
			case APP_PROGRAM_RUNNING :
				{
					break;
				}
			case APP_MENU_MODE_END :
				{
					if (btn == BUTTON_UP_SHORT_RELEASE || btn == BUTTON_UP_LONG_RELEASE)
					{
						appMode = APP_NORMAL_MODE;
					}
					break;
				}
		}


	}

// ============================================================================= //




// ============================================================================= //
// ===== FUNCTIONS ============================================================= //

	
	
	

	// ---------------------------------- //
	// ----- Menu Manager Functions ----- //
	// ---------------------------------- //
		//----------------------------------------------------------------------//
			void outputMessageAction(String messageText) {
				Serial.print("> Action: ");
				Serial.println(messageText);
			}
			
			void lcdPrintSetting(long measure, int unitIndex) {
				switch (unitIndex) {
					case 0:	//mL
					{
						lcd.setCursor(2,1);
						lcd.print(F("        "));
						lcd.setCursor(2,1);
						lcd.print(measure);
						lcd.setCursor(5,1);
						lcd.print("mL");
						break;;
					}
					case 1:	//sec
					{
						lcd.setCursor(2,1);
						lcd.print(F("        "));
						lcd.setCursor(2,1);
						lcd.print(measure);
						lcd.setCursor(7,1);
						lcd.print("s");
						break;;
					}
				}
				
			}
			
			void lcdPrintBlankSetting(void) {
				lcd.setCursor(2,1);
				lcd.print(F("            "));
			}

		//----------------------------------------------------------------------//
		// Addition or removal of menu items in MenuData.h will require this method
		// to be modified accordingly. 
			byte processMenuCommand(byte cmdId)
			{
			  byte complete = false;  // set to true when menu command processing complete.
			  byte configChanged = false;

			  // when button pushed again, close out menu command, and mark action complete
			  if (btn == BUTTON_SELECT_PRESSED)
			  {
				complete = true;
			  }
			  
			  
			  switch (cmdId)
			  {
				// TODO Process menu commands here:
				case mnuCmdprg_oscillating_vol :
				{
					if (process_menu_cmd_justCalled)
					{
						currentConfig.debugPrintState();
						
						outputMessageAction(F("Change Oscillating Prg Vol"));
						lcdPrintSetting(currentConfig.getOscVol(), 0);
					}

					configChanged = true;
					if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepUpOscVol());
						lcdPrintSetting(currentConfig.getOscVol(), 0);
						
						currentConfig.debugPrintState();
					}
					else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepDnOscVol());
						lcdPrintSetting(currentConfig.getOscVol(), 0);
						
						currentConfig.debugPrintState();
					}
					else
					{
						configChanged = false;
					}
					break;;
				}
					
				case mnuCmdprg_oscillating_dur :
				{
					if (process_menu_cmd_justCalled)
					{
						currentConfig.debugPrintState();
						
						outputMessageAction(F("Change Oscillating Prg Duration"));
						lcdPrintSetting(currentConfig.getOscDur(), 1);
					}

					configChanged = true;
					if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) {
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepUpOscDur());
						lcdPrintSetting(currentConfig.getOscDur(), 1);
						
						currentConfig.debugPrintState();
					} else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) {
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepDnOscDur());
						lcdPrintSetting(currentConfig.getOscDur(), 1);
						
						currentConfig.debugPrintState();
					} else {
						configChanged = false;
					}
					
					break;
				}
					
				case mnuCmdprg_oscillating_start :
				{
					
					if (btn == BUTTON_SELECT_PRESSED)
					{
						// STOP    STOP    STOP
						// Complete Motor Oscillation Movement
						outputMessageAction(F(">>> STOP Oscillating Prg!"));
						currentConfig.stopOsc();
						
						// end processing of this menu action
						complete = true;
					}
					else if (process_menu_cmd_justCalled)
					{
						// START   START   START
						// Start Motor Oscillation Movement
						outputMessageAction(F(">>> START Oscillating Prg!"));
						
						// Indicate status on LCD
							lcd.setCursor(2,1);
							lcd.print(F("          "));
							lcd.setCursor(2,1);
							lcd.print(F("ACTIVE!!!"));
							
						// begin oscillation
						currentConfig.startOsc();
						
					} else {
						// Perform motion segment for this itteration
						currentConfig.continueOsc();
					}
					
					break;
				}
					
				case mnuCmdprg_push_vol :
				
					if (process_menu_cmd_justCalled)
					{
						outputMessageAction(F("Change Push Prg Vol"));
						lcdPrintSetting(currentConfig.getPushVol(), 0);
					}

					configChanged = true;
					if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepUpPushVol());
						lcdPrintSetting(currentConfig.getPushVol(), 0);
					}
					else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepDnPushVol());
						lcdPrintSetting(currentConfig.getPushVol(), 0);
					}
					else
					{
						configChanged = false;
					}
					
					break;
					
				case mnuCmdprg_push_dur :
					if (process_menu_cmd_justCalled)
					{
						outputMessageAction(F("Change Push Prg Duration"));
						lcdPrintSetting(currentConfig.getPushDur(), 1);
					}

					configChanged = true;
					if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepUpPushDur());
						lcdPrintSetting(currentConfig.getPushDur(), 1);
					}
					else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepDnPushDur());
						lcdPrintSetting(currentConfig.getPushDur(), 1);
					}
					else
					{
						configChanged = false;
					}
					
					break;
					
				case mnuCmdprg_push_start :
					currentConfig.debugPrintState();
					if (process_menu_cmd_justCalled)
					{
						outputMessageAction(F(">>> Start Push Prg"));
					}
					
					break;
					
				case mnuCmdprg_pull_vol :
					if (process_menu_cmd_justCalled)
					{
						outputMessageAction(F("Change Pull Prg Vol"));
						lcdPrintSetting(currentConfig.getPullVol(), 0);
					}

					configChanged = true;
					if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepUpPullVol());
						lcdPrintSetting(currentConfig.getPullVol(), 0);
					}
					else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepDnPullVol());
						lcdPrintSetting(currentConfig.getPullVol(), 0);
					}
					else
					{
						configChanged = false;
					}
					break;
					
				case mnuCmdprg_pull_dur :
					if (process_menu_cmd_justCalled)
					{
						outputMessageAction(F("Change Pull Prg Duration"));
						lcdPrintSetting(currentConfig.getPullDur(), 1);
					}

					configChanged = true;
					if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepUpPullDur());
						lcdPrintSetting(currentConfig.getPullDur(), 1);
					}
					else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
					{
						Serial.print(F("  new val: "));
						Serial.println(currentConfig.stepDnPullDur());
						lcdPrintSetting(currentConfig.getPullDur(), 1);
					}
					else
					{
						configChanged = false;
					}
					
					break;
					
				case mnuCmdprg_pull_start :
					if (process_menu_cmd_justCalled)
					{
						outputMessageAction(F(">>> Start Pull Prg"));
					}
					
					break;
				case mnuCmdsettings_idk :
					break;
				case mnuCmdsettings_resetToDefaults :
					break;
				default:
					break;
			  }
			  
			// check if above code changed configurations
			if (configChanged && cmdId != mnuCmdsettings_resetToDefaults)
			{
// 				lcd.setCursor(2, 1);
// 				lcd.print(rpad(strbuf, currentConfig.getSettingStr(cmdId))); // Display config value.
			}
			
			// save configurations out
			if (complete)
			{
				currentConfig.save();
 			}
			  
			  process_menu_cmd_justCalled = false;
			  return complete;
			}
			
			
		//----------------------------------------------------------------------//


		//----------------------------------------------------------------------//
		// Callback to convert button press to navigation action.
			byte getNavAction()
			{
			  byte navAction = 0;
			  byte currentItemHasChildren = Menu1.currentItemHasChildren();
  
			  if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED) navAction = MENU_ITEM_PREV;
			  else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED) navAction = MENU_ITEM_NEXT;
			  else if (btn == BUTTON_SELECT_PRESSED || (btn == BUTTON_RIGHT_PRESSED && currentItemHasChildren)) navAction = MENU_ITEM_SELECT;
			  else if (btn == BUTTON_LEFT_PRESSED) navAction = MENU_BACK;
			  return navAction;
			}
		//----------------------------------------------------------------------//


		//----------------------------------------------------------------------//
			const char EmptyStr[] = "";

			// Callback to refresh display during menu navigation, using parameter of type enum DisplayRefreshMode.
			void refreshMenuDisplay (byte refreshMode)
			{
			  char nameBuf[LCD_COLS+1];

			/*
			  if (refreshMode == REFRESH_DESCEND || refreshMode == REFRESH_ASCEND)
			  {
				byte menuCount = Menu1.get
				Count();
    
				// uncomment below code to output menus to serial monitor
				if (Menu1.currentMenuHasParent())
				{
				  Serial.print("Parent menu: ");
				  Serial.println(Menu1.getParentItemName(nameBuf));
				}
				else
				{
				  Serial.println("Main menu:");
				}
    
				for (int i=0; i<menuCount; i++)
				{
				  Serial.print(Menu1.getItemName(nameBuf, i));

				  if (Menu1.itemHasChildren(i))
				  {
					Serial.println("->");
				  }
				  else
				  {
					Serial.println();
				  }
				}
			  }
			*/
			


			  lcd.setCursor(0, 0);
			  if (Menu1.currentItemHasChildren())
			  {
				rpad(strbuf, Menu1.getCurrentItemName(nameBuf));
				strbuf[LCD_COLS-1] = 0b01111110;            // Display forward arrow if this menu item has children.
				lcd.print(strbuf);
				lcd.setCursor(0, 1);
				lcd.print(rpad(strbuf, EmptyStr));          // Clear config value in display
			  }
			  else
			  {
				byte cmdId;
				rpad(strbuf, Menu1.getCurrentItemName(nameBuf));
				
				
				// Print Settings below setting change functions
				if (strstr(Menu1.getCurrentItemName(nameBuf),"1.1.1")) {
					lcdPrintSetting(currentConfig.getOscVol(), 0);
				} else if (strstr(Menu1.getCurrentItemName(nameBuf),"1.1.2")) {
					lcdPrintSetting(currentConfig.getOscDur(), 1);
				} else if (strstr(Menu1.getCurrentItemName(nameBuf),"1.2.1")) {	
					lcdPrintSetting(currentConfig.getPushVol(), 0);
				} else if (strstr(Menu1.getCurrentItemName(nameBuf),"1.2.2")) {
					lcdPrintSetting(currentConfig.getPushDur(), 1);
				} else if (strstr(Menu1.getCurrentItemName(nameBuf),"1.3.1")) {
					lcdPrintSetting(currentConfig.getPullVol(), 0);
				} else if (strstr(Menu1.getCurrentItemName(nameBuf),"1.3.2")) {
					lcdPrintSetting(currentConfig.getPullDur(), 1);
				} else {
					lcdPrintBlankSetting();
				}
				// Reset cursor position before continuing
				lcd.setCursor(0, 0);
						
    
				if ((cmdId = Menu1.getCurrentItemCmdId()) == 0)
				{
				  strbuf[LCD_COLS-1] = 0b01111111;          // Display back arrow if this menu item ascends to parent.
				  lcd.print(strbuf);
				  lcd.setCursor(0, 1);
				  lcd.print(rpad(strbuf, EmptyStr));        // Clear config value in display.
				}
				else
				{
				  lcd.print(strbuf);
				  lcd.setCursor(0, 1);
				  lcd.print(" ");
				}
			  }
			}
		//----------------------------------------------------------------------//
		
	// ----------------------------------- //
	// ----------------------------------- //

// ============================================================================= //
