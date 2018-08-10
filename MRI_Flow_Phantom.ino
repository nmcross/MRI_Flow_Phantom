/*************************************************************************************
MRI Flow Phantom
(c) 2018 Nathan M. Cross

References:
https://www.dfrobot.com/wiki/index.php/LCD_KeyPad_Shield_For_Arduino_SKU:_DFR0009
https://learn.sparkfun.com/tutorials/easy-driver-hook-up-guide?_ga=2.110778565.153953877.1533438339-555965987.1530884463
http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
http://www.schmalzhaus.com/EasyDriver/Examples/EasyDriverExamples.html

https://eeenthusiast.com/arduino-lcd-tutorial-display-menu-system-scrolling-menu-changeable-variables-projects/

TODO: 
- Change to push on push off button control
- add volume limits to prevent disaster
- add menu system to select volume and plateau duration
- 
**************************************************************************************/
#define software_version		1.0


#include <AccelStepper/AccelStepper.h>
//#include <AccelStepper/MultiStepper.h>
#include <LiquidCrystal.h>



// ------------------------------------- //
// ----- DF Robot LCD Keypad Setup ----- //
	LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
	
	// define some values used by the panel and buttons
	int lcd_key     = 0;
	int adc_key_in  = 0;
	
	#define btnRIGHT  0
	#define btnUP     1
	#define btnDOWN   2
	#define btnLEFT   3
	#define btnSELECT 4
	#define btnNONE   5
	
	int read_LCD_buttons(){               // read the buttons
		adc_key_in = analogRead(0);       // read the value from the sensor 

		// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
		// we add approx 50 to those values and check to see if we are close
		// We make this the 1st option for speed reasons since it will be the most likely result

		if (adc_key_in > 1000) return btnNONE; 

		// For V1.1 us this threshold
		if (adc_key_in < 50)   return btnRIGHT;  
		if (adc_key_in < 250)  return btnUP; 
		if (adc_key_in < 450)  return btnDOWN; 
		if (adc_key_in < 650)  return btnLEFT; 
		if (adc_key_in < 850)  return btnSELECT;  

		return btnNONE;                // when all others fail, return this.
	}
// ------------------------------------- //



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
	#define ROD_THREADING	7.874015
	#define DEG_PER_STEP	0.225
	#define STEPS_PER_CM	((360/DEG_PER_STEP) * ROD_THREADING)
	#define CM_PER_CC		1 // ???is this right???
	#define CC_PER_STEP		( 1 / (STEPS_PER_CM * CM_PER_CC) )
// -------------------------------------------- //



// ----------------------------------- //
// ----- Motor Control Functions ----- //
void enable_stepper() {
	digitalWrite(drv_enable, LOW);
}

void disable_stepper() {
	digitalWrite(drv_enable, HIGH);
}

void inject_cc(AccelStepper stepper, float vol_cc, long speed = stepper_default_speed) {
	stepper.setMaxSpeed(speed);
	
	// zero out current position
	stepper.setCurrentPosition(0);
	
	// Output action to Serial Monitor
	Serial.print(" - Injecting (mL): ");
	Serial.println(vol_cc);
	
	// move to new position represent specified volume
	float ccPerStep = CC_PER_STEP;
	enable_stepper();
	stepper.runToNewPosition(round(vol_cc / ccPerStep));
	
	disable_stepper();
}

void  retract_cc(AccelStepper stepper, float vol_cc, long speed = stepper_default_speed) {
	stepper.setMaxSpeed(speed);
	
	// zero out current position
	stepper.setCurrentPosition(0);
	
	// Output action to Serial Monitor
	Serial.print(" - Retracting (mL): ");
	Serial.println(vol_cc);
	
	// move to new position represent specified volume
	float ccPerStep = CC_PER_STEP;
	enable_stepper();
	stepper.runToNewPosition( -round(vol_cc / ccPerStep));
	
	disable_stepper();
}
// ----------------------------------- //



// ================= //
// ===== SETUP ===== //
void setup(){
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

	// Start Serial Monitor Output
	Serial.begin(115200); //Open Serial connection for debugging
	Serial.print("MR Flow Phantom v");
	Serial.println(software_version);
	Serial.println();
	
	// Start LCD Keypad
	lcd.begin(16, 2);               // start the library
	lcd.setCursor(0,0);             // set the LCD cursor   position
	lcd.print("MR Flow Phantom");
	lcd.setCursor(5,1);
	lcd.print("v");
	lcd.print(software_version);
	delay(3000);
	
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Push 'select' ");
	lcd.setCursor(8,1);
	lcd.print("to begin");
	
}
// ================= //
 
 
 
// ===================== //
// ===== MAIN LOOP ===== //
float vol = 0.3; // (mL)
void loop(){
	

	if (read_LCD_buttons() == btnSELECT) {
		inject_cc(stepper, vol);
		delay(50);
		retract_cc(stepper, vol);
	}

}
// ===================== //