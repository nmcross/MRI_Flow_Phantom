/*************************************************************************************
MRI Flow Phantom
(c) 2018 Nathan M. Cross

References:
https://www.dfrobot.com/wiki/index.php/LCD_KeyPad_Shield_For_Arduino_SKU:_DFR0009
https://learn.sparkfun.com/tutorials/easy-driver-hook-up-guide?_ga=2.110778565.153953877.1533438339-555965987.1530884463
http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html

https://eeenthusiast.com/arduino-lcd-tutorial-display-menu-system-scrolling-menu-changeable-variables-projects/
http://www.cohesivecomputing.co.uk/hackatronics/arduino-lcd-menu-library/

TODO: 
- Change to push on push off button control
- add volume limits to prevent disaster
- add menu system to select volume and plateau duration
- 
**************************************************************************************/
#define software_version		1.1



// ============================================================================= //
// ===== INCLUDES ============================================================== //

	#include <AccelStepper/AccelStepper.h>
	//#include <AccelStepper/MultiStepper.h>
	#include <LiquidCrystal.h>
	#include "LcdKeypad.h"
	#include "MenuData.h"
	#include "TimerOne.h"

// ============================================================================= //




// ============================================================================= //
// ===== CONFIGURATIONS ======================================================== //

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
			APP_PROCESS_MENU_CMD
		};

		byte appMode = APP_NORMAL_MODE;

		MenuManager Menu1(flowPhantomMenu_Root, menuCount(flowPhantomMenu_Root));

		char strbuf[LCD_COLS + 1]; // one line of lcd display
		byte btn;
	// ---------------------------------- //



	// ------------------------------------ //
	// ----- EasyDriver Configuration ----- //
		// EasyDriver Pin Configuration (digital pins)
		#define drv_step	A1
		#define drv_dir		A2
		#define drv_ms1		A3
		#define drv_ms2		A4
		#define drv_enable	A5

		AccelStepper stepper(AccelStepper::DRIVER, drv_step, drv_dir); 

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

// ============================================================================= //





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
			lcd.print("v");
			lcd.print(software_version);
			delay(3000);
			
		
		// ------------------------------ //
			
	
		// ----------------------- //
		// ----- Motor Setup ----- //
		
			// Configure motor pins as outputs
			pinMode(drv_step,	OUTPUT);
			pinMode(drv_dir,	OUTPUT);
			pinMode(drv_ms1,	OUTPUT);
			pinMode(drv_ms2,	OUTPUT);
			pinMode(drv_enable, OUTPUT);
	
			disable_stepper();
	
			// Set microstepping configuration described above
			digitalWrite(drv_ms1, drv_ms1_set);
			digitalWrite(drv_ms2, drv_ms2_set);
	
			stepper.setMaxSpeed(stepper_default_speed);
			stepper.setAcceleration(stepper_default_acc);
			
		// ----------------------- //
		
		
		// -------------------------- //
		// ----- Serial Monitor ----- //

			// Start Serial Monitor Output
			Serial.begin(115200); //Open Serial connection for debugging
			Serial.println("-----------------------------");
			Serial.print("MR Flow Phantom v");
			Serial.println(software_version);
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
			setBacklightBrightness(1);
			
			lcd.setCursor(0,1);             // set the LCD cursor position
			lcd.print("MR Flow Phantom");
		
		// ------------------------------ //
	
	}

// ============================================================================= //
 
 
 
 
// ============================================================================= //
// ===== MAIN LOOP ============================================================= //

	float vol = 0.3; // (mL)
	
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
						appMode = APP_NORMAL_MODE;
					}
					else if (menuMode == MENU_INVOKE_ITEM)
					{
						appMode = APP_PROCESS_MENU_CMD;

						// Indicate selected item.
						if (Menu1.getCurrentItemCmdId())
						{
							lcd.setCursor(0, 1);
							strbuf[0] = 0b01111110; // forward arrow representing input prompt.
							strbuf[1] = 0;
							lcd.print(strbuf);
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
						// clear forward arrow
						lcd.setCursor(0, 1);
						strbuf[0] = ' '; // clear forward arrow
						strbuf[1] = 0;
						lcd.print(strbuf);
					}
					break;
				}
		}


	}

// ============================================================================= //




// ============================================================================= //
// ===== FUNCTIONS ============================================================= //

	// ----------------------------------- //
	// ----- Motor Control Functions ----- //
	// ----------------------------------- //
		void enable_stepper() {
			digitalWrite(drv_enable, LOW);
		}

		void disable_stepper() {
			digitalWrite(drv_enable, HIGH);
		}
		
		bool isVolTooLarge (float vol_cc) {
			if (vol_cc > SYRINGE_MAX_VOL_CC) {
				Serial.print(F("!!!ERR!!! - Injectable Volume is too Large ("));
				Serial.print(vol_cc);
				Serial.print(">");
				Serial.print(SYRINGE_MAX_VOL_CC);
				Serial.println(")");
				return true;
			} else {
				return false;
			}
		}

		void inject_cc(AccelStepper stepper, float vol_cc, long speed = stepper_default_speed) {
			if (!isVolTooLarge(vol_cc)) {
				stepper.setMaxSpeed(speed);
	
				// zero out current position
				stepper.setCurrentPosition(0);
	
				// Output action to Serial Monitor
				Serial.print(F(" - Injecting (mL): "));
				Serial.println(vol_cc);
	
				// move to new position represent specified volume
				float ccPerStep = CALC_CC_PER_STEP;
				enable_stepper();
				stepper.runToNewPosition(round(vol_cc / ccPerStep));
	
				disable_stepper();
			}
		}

		void retract_cc(AccelStepper stepper, float vol_cc, long speed = stepper_default_speed) {
			if (!isVolTooLarge(vol_cc)) {
				stepper.setMaxSpeed(speed);
	
				// zero out current position
				stepper.setCurrentPosition(0);
	
				// Output action to Serial Monitor
				Serial.print(F(" - Retracting (mL): "));
				Serial.println(vol_cc);
	
				// move to new position represent specified volume
				float ccPerStep = CALC_CC_PER_STEP;
				enable_stepper();
				stepper.runToNewPosition( -round(vol_cc / ccPerStep));
	
				disable_stepper();
			}
		}
		
		
	// ----------------------------------- //
	// ----------------------------------- //
	
	

	// ---------------------------------- //
	// ----- Menu Manager Functions ----- //
	// ---------------------------------- //
		//----------------------------------------------------------------------//
			void outputMessageAction(String messageText) {
				Serial.print("> Action: ");
				Serial.println(messageText);
			}

		//----------------------------------------------------------------------//
		// Addition or removal of menu items in MenuData.h will require this method
		// to be modified accordingly. 
			byte processMenuCommand(byte cmdId)
			{
			  byte complete = false;  // set to true when menu command processing complete.

			  if (btn == BUTTON_SELECT_PRESSED)
			  {
				complete = true;
			  }

			  switch (cmdId)
			  {
				// TODO Process menu commands here:
				case mnuCmdprg_oscillating_vol :
					outputMessageAction(F("Change Oscillating Prg Vol"));
					break;;
				case mnuCmdprg_oscillating_dur :
					outputMessageAction(F("Change Oscillating Prg Duration"));
					break;
				case mnuCmdprg_oscillating_start :
					outputMessageAction(F(">>> Start Oscillating Prg!"));
					break;
				case mnuCmdprg_push_vol :
					outputMessageAction(F("Change Push Prg Vol"));
					break;
				case mnuCmdprg_push_dur :
					outputMessageAction(F("Change Push Prg Duration"));
					break;
				case mnuCmdprg_push_start :
					outputMessageAction(F(">>> Start Push Prg"));
					break;
				case mnuCmdprg_pull_vol :
					outputMessageAction(F("Change Pull Prg Vol"));
					break;
				case mnuCmdprg_pull_dur :
					outputMessageAction(F("Change Pull Prg Duration"));
					break;
				case mnuCmdprg_pull_start :
					outputMessageAction(F(">>> Start Pull Prg"));
					break;
				case mnuCmdsettings_idk :
					break;
				case mnuCmdsettings_resetToDefaults :
					break;
				default:
					break;
			  }

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
				byte menuCount = Menu1.getMenuItemCount();
    
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
      
				  // TODO Display config value.
				}
			  }
			}
		//----------------------------------------------------------------------//
		
	// ----------------------------------- //
	// ----------------------------------- //

// ============================================================================= //