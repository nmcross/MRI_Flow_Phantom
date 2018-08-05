
/*************************************************************************************



**************************************************************************************/


// EasyDriver Pin Configuration (digital pins)
#define drv_step	A1
#define drv_dir		A2
#define drv_ms1		A3
#define drv_ms2		A4
#define drv_enable	A5


//Declare variables for functions
char user_input;
int x;
int y;
int state;


void setup(){
	pinMode(drv_step,	OUTPUT);
	pinMode(drv_dir,	OUTPUT);
	pinMode(drv_ms1,	OUTPUT);
	pinMode(drv_ms2,	OUTPUT);
	pinMode(drv_enable, OUTPUT);
	resetEDPins(); //Set step, direction, microstep and enable pins to default states
	Serial.begin(115200); //Open Serial connection for debugging
	Serial.println("Begin motor control");
	Serial.println();
	//Print function list for user selection
	Serial.println("Enter number for control option:");
	Serial.println("1. Turn at default microstep mode.");
	Serial.println("2. Reverse direction at default microstep mode.");
	Serial.println("3. Turn at 1/8th microstep mode.");
	Serial.println("4. Step forward and reverse directions.");
	Serial.println();
	

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