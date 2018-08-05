
/*************************************************************************************

  Mark Bramwell, July 2010

  This program will test the LCD panel and the buttons.When you push the button on the shield?
  the screen will show the corresponding one.
 
  Connection: Plug the LCD Keypad to the UNO(or other controllers)

**************************************************************************************/
//#include <MultiStepper.h>
//#include <AccelStepper.h>
#include <LiquidCrystal.h>

// EasyDriver Pin Configuration (digital pins)
#define drv_ms1		13
#define drv_ms2		12
#define drv_enable	11
#define drv_step	2
#define drv_dir		3

//Declare variables for functions
char user_input;
int x;
int y;
int state;



//AccelStepper stepper(AccelStepper::FULL4WIRE, ); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

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

   // For V1.0 comment the other threshold and use the one below:
   /*
     if (adc_key_in < 50)   return btnRIGHT;  
     if (adc_key_in < 195)  return btnUP; 
     if (adc_key_in < 380)  return btnDOWN; 
     if (adc_key_in < 555)  return btnLEFT; 
     if (adc_key_in < 790)  return btnSELECT;   
   */

    return btnNONE;                // when all others fail, return this.
}

void setup(){
	pinMode(drv_step,	OUTPUT);
	pinMode(drv_dir,	OUTPUT);
	pinMode(drv_ms1,	OUTPUT);
	pinMode(drv_ms2,	OUTPUT);
	pinMode(drv_enable, OUTPUT);
	resetEDPins(); //Set step, direction, microstep and enable pins to default states
	Serial.begin(9600); //Open Serial connection for debugging
	Serial.println("Begin motor control");
	Serial.println();
	//Print function list for user selection
	Serial.println("Enter number for control option:");
	Serial.println("1. Turn at default microstep mode.");
	Serial.println("2. Reverse direction at default microstep mode.");
	Serial.println("3. Turn at 1/8th microstep mode.");
	Serial.println("4. Step forward and reverse directions.");
	Serial.println();
	
	// ACCEL_STEPPER LIBRARY
	// Stepper Setup
	//stepper.setMaxSpeed(1000);
	//stepper.setSpeed(50);
	
	// LCD Setup
   lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 
   lcd.print("Flow Phantom");  // print a simple message on the LCD
}
 
void loop(){
	while(Serial.available()){
		user_input = Serial.read(); //Read user input and trigger appropriate function
		digitalWrite(drv_enable, LOW); //Pull enable pin low to allow motor control
		if (user_input =='1')
		{
			StepForwardDefault();
		}
		else if(user_input =='2')
		{
			ReverseStepDefault();
		}
		else if(user_input =='3')
		{
			SmallStepMode();
		}
		else if(user_input =='4')
		{
			ForwardBackwardStep();
		}
		else
		{
			Serial.println("Invalid option entered.");
		}
		resetEDPins();
	}
	 
	
   //lcd.setCursor(9,1);             // move cursor to second line "1" and 9 spaces over
   //lcd.print(millis()/1000);       // display seconds elapsed since power-up
//
   //lcd.setCursor(0,1);             // move to the begining of the second line
   //lcd_key = read_LCD_buttons();   // read the buttons
//
   //switch (lcd_key){               // depending on which button was pushed, we perform an action
//
       //case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen
            //lcd.print("RIGHT ");
            //break;
       //}
       //case btnLEFT:{
             //lcd.print("LEFT   "); //  push button "LEFT" and show the word on the screen
             //break;
       //}    
       //case btnUP:{
             //lcd.print("UP    ");  //  push button "UP" and show the word on the screen
             //break;
       //}
       //case btnDOWN:{
             //lcd.print("DOWN  ");  //  push button "DOWN" and show the word on the screen
             //break;
       //}
       //case btnSELECT:{
             //lcd.print("SELECT");  //  push button "SELECT" and show the word on the screen
             //break;
       //}
       //case btnNONE:{
             //lcd.print("NONE  ");  //  No action  will show "None" on the screen
             //break;
       //}
   //}
}



//Reset Easy Driver pins to default states
void resetEDPins()
{
	digitalWrite(drv_step, LOW);
	digitalWrite(drv_dir, LOW);
	digitalWrite(drv_ms1, LOW);
	digitalWrite(drv_ms2, LOW);
	digitalWrite(drv_enable, HIGH);
}

//Default microstep mode function
void StepForwardDefault()
{
	Serial.println("Moving forward at default step mode.");
	digitalWrite(drv_dir, LOW); //Pull direction pin low to move "forward"
	for(x= 1; x<1000; x++)  //Loop the forward stepping enough times for motion to be visible
	{
		digitalWrite(drv_step,HIGH); //Trigger one step forward
		delay(1);
		digitalWrite(drv_step,LOW); //Pull step pin low so it can be triggered again
		delay(1);
	}
	Serial.println("Enter new option");
	Serial.println();
}

//Reverse default microstep mode function
void ReverseStepDefault()
{
	Serial.println("Moving in reverse at default step mode.");
	digitalWrite(drv_dir, HIGH); //Pull direction pin high to move in "reverse"
	for(x= 1; x<1000; x++)  //Loop the stepping enough times for motion to be visible
	{
		digitalWrite(drv_step,HIGH); //Trigger one step
		delay(1);
		digitalWrite(drv_step,LOW); //Pull step pin low so it can be triggered again
		delay(1);
	}
	Serial.println("Enter new option");
	Serial.println();
}

// 1/8th microstep foward mode function
void SmallStepMode()
{
	Serial.println("Stepping at 1/8th microstep mode.");
	digitalWrite(drv_dir, LOW); //Pull direction pin low to move "forward"
	digitalWrite(drv_ms1, HIGH); //Pull drv_ms1, and drv_ms2 high to set logic to 1/8th microstep resolution
	digitalWrite(drv_ms2, HIGH);
	for(x= 1; x<1000; x++)  //Loop the forward stepping enough times for motion to be visible
	{
		digitalWrite(drv_step,HIGH); //Trigger one step forward
		delay(1);
		digitalWrite(drv_step,LOW); //Pull step pin low so it can be triggered again
		delay(1);
	}
	Serial.println("Enter new option");
	Serial.println();
}

//Forward/reverse stepping function
void ForwardBackwardStep()
{
	Serial.println("Alternate between stepping forward and reverse.");
	for(x= 1; x<5; x++)  //Loop the forward stepping enough times for motion to be visible
	{
		//Read direction pin state and change it
		state=digitalRead(drv_dir);
		if(state == HIGH)
		{
			digitalWrite(drv_dir, LOW);
		}
		else if(state ==LOW)
		{
			digitalWrite(drv_dir,HIGH);
		}
		
		for(y=1; y<1000; y++)
		{
			digitalWrite(drv_step,HIGH); //Trigger one step
			delay(1);
			digitalWrite(drv_step,LOW); //Pull step pin low so it can be triggered again
			delay(1);
		}
	}
	Serial.println("Enter new option:");
	Serial.println();
}