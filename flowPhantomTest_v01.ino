

/*************************************************************************************



**************************************************************************************/
#include <AccelStepper/AccelStepper.h>
#include <AccelStepper/MultiStepper.h>

#define software_version		1.0

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

void setup(){
	//pinMode(drv_step,	OUTPUT);
	//pinMode(drv_dir,	OUTPUT);
	pinMode(drv_ms1,	OUTPUT);
	pinMode(drv_ms2,	OUTPUT);
	pinMode(drv_enable, OUTPUT);
	
	
	digitalWrite(drv_ms1, drv_ms1_set);
	digitalWrite(drv_ms2, drv_ms2_set);
	
	// Disable motor by default
	digitalWrite(drv_enable, HIGH);
	
	stepper.setMaxSpeed(stepper_default_speed);
	stepper.setAcceleration(stepper_default_acc);

	Serial.begin(115200); //Open Serial connection for debugging
	Serial.print("MR Flow Phantom v");
	Serial.println(software_version);
	Serial.println();
	
}
 
void loop(){
	// Threaded rod is probably 20tpi = 7.874015 thread/cm
	// motor: 1.8deg/step
	// configuration: Eighth of a step
	// 1 step = 1.8/8 = 0.225deg
	// 1600 steps per rev
	// 1600 * 7.874 = 12,598.4
	inject_cc(stepper, 0.1);
	delay(200);
	retract_cc(stepper, 0.1);
	//stepper.runSpeed();
	delay(1000);
}
