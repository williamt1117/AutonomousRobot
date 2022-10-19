#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    InfraCollector, sensorAnalog)
#pragma config(Sensor, dgtl2,  startButton,    sensorTouch)
#pragma config(Sensor, dgtl3,  sonarSensor,    sensorSONAR_cm)
#pragma config(Sensor, dgtl5,  rightBumper,    sensorTouch)
#pragma config(Sensor, dgtl6,  leftBumper,     sensorTouch)
#pragma config(Sensor, dgtl7,  limitSwitch,    sensorTouch)
#pragma config(Sensor, dgtl10, infraredLED,    sensorDigitalOut)
#pragma config(Sensor, dgtl11, finishLED,      sensorDigitalOut)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           motorLeft,     tmotorVex393_HBridge, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port2,           motorArm,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           motorCage,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          motorRight,    tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define MAXTURNSPEED 40
#define MAXDRIVESPEED 25
#define MAXROTATIONUNITS 1100

#define DISTANCETOTARGETCM 18
#define DISTANCETOTARGETTRESHHOLD 1

#define DISTANCETOWALLCM 20

#define IRSENSORTHRESHHOLD 170

#define MOTORARMSPEED 25

#define MACHINEDELAY 1000

bool startButtonPushed;
bool switchPushed;
bool leftBumperPushed;
bool rightBumperPushed;

//updates global boolean values
void UpdateButtonPress()
{
	leftBumperPushed = false;
	rightBumperPushed = false;
	if(SensorValue(startButton))
	{
		startButtonPushed = true;
	}
	if (SensorValue(limitSwitch))
	{
		switchPushed = true;
	}
	if (SensorValue(leftBumper))
	{
		leftBumperPushed = true;
	}
	if (SensorValue(rightBumper))
	{
		rightBumperPushed = true;
	}
}

//returns a bool indicating whether the IR sensor sees infrared
bool InfraredDetected()
{
	return SensorValue[InfraCollector] <= IRSENSORTHRESHHOLD;
}

//turns motors on to drive forward
void DriveForward(int speed)
{
	motor[motorLeft] = -speed;
	motor[motorRight] = speed;
}

//turns motors on to drive backward
void DriveBackward(int speed)
{
	motor[motorLeft] = speed;
	motor[motorRight] = -speed;
}

//turns motors on to rotate counter-clockwise
void RotateCCW(int speed)
{
	motor[motorRight] = -speed;
	motor[motorLeft] = -speed;
}

//turns motors on to rotate clockwise
void RotateCW(int speed)
{
	motor[motorRight] = speed;
	motor[motorLeft] = speed;
}

//turns all movement motors off
void StopMoving()
{
	motor[motorRight] = 0;
	motor[motorLeft] = 0;
}

//rotates to face the target
void TurnTowardsTarget()
{
	//rotate CCW until IR NOT detected for 0.2s
	RotateCCW(MAXTURNSPEED);
	bool infrared = true;
	int loopCounts = 0;
	while(infrared)
	{
		//runs approx 100 times / second
		wait1Msec(10);
		//if no IR detected for 20 loops, exit
		if (InfraredDetected())
		{
			loopCounts = 0;
		}
		else
		{
			loopCounts++;
		}
		if (loopCounts >= 20)
		{
			//IR has not been detected for approx 0.2s, exit
			infrared = false;
		}
	}

	//stop rotating, wait MACHINEDELAY milliseconds
	StopMoving();
	wait1Msec(MACHINEDELAY);

	//rotate clockwise
	RotateCW(MAXTURNSPEED);

	//record encoder value when first detected IR
	while (!InfraredDetected())
	{
		//stall till detecting IR
	}
	int initialEncoderValue = getMotorEncoder(motorLeft);


	//record encoder value when IR no longer detected for a span of 0.2s
	infrared = true;
	loopCounts = 0;
	while(infrared)
	{
		//runs approx 100 times / second
		wait1Msec(10);
		//if no IR detected for 20 loops, exit
		if (InfraredDetected())
		{
			loopCounts = 0;
		}
		else
		{
			loopCounts++;
		}
		if (loopCounts >= 20)
		{
			//IR has not been detected for approx 0.2s, exit
			infrared = false;
		}
	}
	//record
	int finalEncoderValue = getMotorEncoder(motorLeft);

	//stop rotating, wait MACHINEDELAY milliseconds
	StopMoving();
	wait1Msec(MACHINEDELAY);

	//rotate back half of the difference of encoder values
	int halfDifference = abs((finalEncoderValue - initialEncoderValue) / 2);
	while (abs(finalEncoderValue - getMotorEncoder(motorLeft)) <= halfDifference)
	{
			RotateCCW(MAXTURNSPEED);
	}

	//stop rotating and wait MACHINEDELAY milliseconds
	StopMoving();
	wait1Msec(MACHINEDELAY);
}

//moves the robot forward or backward to ideal dropping proximity
void TuneProximity()
{
	if (SensorValue(sonarSensor) == DISTANCETOTARGETCM)
	{
		return; //exit
	}

	//determine whether robot is too close or too far from target (probably too close)
	bool tooFar = SensorValue(sonarSensor) > DISTANCETOTARGETCM;

	//drives forwards or backwards till we surpass DISTANCETOTARGETCM
	if (tooFar)
	{
		while (SensorValue(sonarSensor) > DISTANCETOTARGETCM || SensorValue(sonarSensor) == -1)
		{
			DriveForward(MAXDRIVESPEED);
		}
	}
	else
	{
		while (SensorValue(sonarSensor) < DISTANCETOTARGETCM || SensorValue(sonarSensor) == -1)
		{
			DriveBackward(MAXDRIVESPEED);
		}
	}
	StopMoving();
	wait1Msec(MACHINEDELAY);
}

//moves towards target and if target is in range, exits and returns true
bool ApproachTarget(int ms) //returns whether target was detected
{
		//for 'ms' milliseconds, driveForward
		int currentTime = 0;
		while (currentTime < ms)
		{
			currentTime++;
			DriveForward(MAXDRIVESPEED);
			//if sonar range is less than or equal to DISTANCETOTARGETCM, return true
			if (SensorValue(sonarSensor) <= DISTANCETOTARGETCM && SensorValue(sonarSensor) != -1)
			{
				StopMoving();
				return true;
			}
			wait1Msec(1); //forces approximately 1ms loop
		}
		StopMoving();
		wait1Msec(MACHINEDELAY);
		return false;
}

//backs up, rotates approximately 180deg and drives to arena edge for retrieval
void MoveToArenaEdge()
{
	//back up for 1s
	DriveBackward(MAXDRIVESPEED);
	wait1Msec(2000);
	StopMoving();

	wait1Msec(MACHINEDELAY);

	//turn 180deg
	int initialEncoderValue = getMotorEncoder(motorLeft);
	while (abs(getMotorEncoder(motorLeft) - initialEncoderValue) <= MAXROTATIONUNITS)
	{
		RotateCW(MAXTURNSPEED);
	}
	StopMoving();
	wait1Msec(MACHINEDELAY);

	//drive until sonar detects wall or limit switches detect wall
	while (((SensorValue(sonarSensor) >= DISTANCETOWALLCM || SensorValue(sonarSensor) == -1)) && !SensorValue(leftBumper) && !SensorValue(rightBumper))
	{
		DriveForward(MAXDRIVESPEED);
	}
	StopMoving();
	wait1Msec(MACHINEDELAY);
}

//carries out placement procedure:
//1. drops arm
//2. opens cage
//3. raises arm
//4. closes cage
void PlaceOnTarget()
{
	motor[motorArm] = -MOTORARMSPEED;
	while (!SensorValue(limitSwitch))
	{
		//wait
	}
	motor[motorArm] = 0;
	wait1Msec(MACHINEDELAY);
	motor[motorCage] = 25;
	wait1Msec(250);
	motor[motorCage] = 0;
	wait1Msec(MACHINEDELAY);
	motor[motorArm] = MOTORARMSPEED;
	wait1Msec(2000);
	motor[motorArm] = 0;
	wait1Msec(MACHINEDELAY);
	motor[motorCage] = -25;
	wait1Msec(250 + 15); //+15 to compensate for compounding error
	motor[motorCage] = 0;
}

//activates finishLED to signal completion
void SignalCompletion()
{
	SensorValue(finishLED) = 1;
}

task main()
{
	enum state
	{
		DEFAULT = 0,
		FACINGTARGET = 1,
		APPROACHING = 2,
		FINALANGLEADJUST = 3,
		DISTANCETUNING = 4,
		DROPPING = 5,
		RETRIEVING = 6
	};

	SensorValue(infraredLED) = 0;
	SensorValue(finishLED) = 0;

	state machineState = DEFAULT;

	while (1)
	{
		UpdateButtonPress();
		SensorValue(infraredLED) = InfraredDetected();	//updates

		switch (machineState)
		{
			case DEFAULT:
				if (startButtonPushed == true)
				{
					machineState = FACINGTARGET;
					SensorValue(finishLED) = 0;
					wait1Msec(2500);
				}
				break;
			case FACINGTARGET:
				TurnTowardsTarget();
				machineState = APPROACHING;
				break;
			case APPROACHING:
				bool success = ApproachTarget(1000);
				if (success)
				{
					machineState = FINALANGLEADJUST;
				}
				else
				{
					machineState = FACINGTARGET;
				}
				break;
			case FINALANGLEADJUST:
				TurnTowardsTarget();
				machineState = DISTANCETUNING;
				break;
			case DISTANCETUNING:
				TuneProximity();
				machineState = DROPPING;
				break;
			case DROPPING:
				PlaceOnTarget();
				machineState = RETRIEVING;
				break;
			case RETRIEVING:
				MoveToArenaEdge();
				SignalCompletion();
				machineState = DEFAULT;
				startButtonPushed = false;
				break;
		}
	}
}
