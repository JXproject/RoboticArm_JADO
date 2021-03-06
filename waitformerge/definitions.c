#include "NXTServo-lib-UW.c"
#include "definitions.h"

task main()
{
	SensorType[S4] = sensorI2CCustom9V;
	Point a;
	AngleSet b;
	AngleSet c;
	c.alpha = 120;
	a.x = 200;
	a.y = 100;
	a.z = 100;
	calcAngleSet(a, b);
	displayString(1, "%.2f", b.alpha);
	moveJ2(b);
	/*
	(-90, 39)
	(-20, 1)
	(45, 46)
	(90, 60)

	*/
	wait1Msec(100000);
}


bool calcAngleSet(Point& input, AngleSet& outputAngles)
{
	// Author: Dustin Hu
	// Date: November 17th, 2016
	// Purpose: To calculate the set of angles
	// Input: The point to calculate to and the output angle set to outpu to
	// Output: True if successfully calculated
	// Note: It is assumed that this is a valid point
	// But it is not assumed that the angles that will be produced
	// will be valid
	// Disclaimer: I have done a similar project
	// where I had to calculate the angles of a limb before
	bool output = false;
	float L = calcL(input)
	float alpha = calcAlpha(input, L);
	float beta = calcBeta(input, L);
	float theta = calcTheta(input);
	if (areAnglesValid(alpha, beta))
	{
		output = true;
		outputAngles.alpha = radToDeg(alpha);
		outputAngles.beta = radToDeg(beta);
		outputAngles.theta = radToDeg(theta);
	}
	return output;
}



float calcL(Point& input)
{
	// Author: Dustin Hu
	// Date: November 17th, 2016
	// Purpose: To calculate the length of the line
	// Connecting the end effector and the origin (Check diagrams)
	return sqrt(input.x * input.x + input.z * input.z);
}

float calcTheta(Point& input)
{
	// Author: Dusti nHu
	// Date: November 17th, 2016
	// Purpose: to calculate the theta (Base angle) of the point
	return atan2(input.y, input.z);
}

float calcAlpha(Point& input, float L)
{
	// Nov 17, 2016
	// Dustin Hu
	return calcAlpha1(input) + calcAlpha2(input, L);
}

float calcAlpha1(Point& input)
{
	// Author: Dustin Hu
	// Date: November 17th, 2016
	// Purpose: to calculate alpha1
	return atan2(input.z, input.x);
}


float calcAlpha2(Point& input, float L)
{
	// Author: Dustin Hu
	// Date: November 17th, 2016
	// Purpose: Calculate alpha2 using cosine law

	float numerator = (FOREARM * FOREARM) - (SHOULDER * SHOULDER) - (L * L);
	float denominator = -2.0 * SHOULDER * L;
	return acos(numerator/denominator);
}

float calcBeta(Point& input, float L)
{
	// Author: Dustin Hu
	// Date: November 18th, 2016
	// Purpose: To calculate beta, the angle between the shouder
	// and the forearm
	float numerator = (L * L) - (SHOULDER * SHOULDER) - (FOREARM * FOREARM);
	float denominator = -2.0 * SHOULDER * FOREARM;
	return acos(numerator/denominator);
}


bool isAlphaValid(float alpha)
{
	// Author: Dustin Hu
// Daet: November 18th, 2016
// Purpose: to check if alpha is valid
// Input: An angle Alpha in radians
// Note: Assumes that there is a range of motion of pi/2 rad
// With a pi/6 segment chopped off at the top
// And a pi/3 segment chopped of at the bottom

	bool output = false;
	// To make reading easier
	alpha = radToDeg(alpha);

	if (alpha > 60 && alpha < 150)
	{
		output = true;
	}
	return output;
}

bool isBetaValid(float beta)
{
	// Author: Dustin Hu
// Date: November 20th, 2016
// Purpose: To check if the beta angle is valid
// Input: The angle beta in radians
// Note: Beta must be between -pi/2 and pi/2, with
// 0 defined as being parallel with J2
	bool output = false;
	// To make reading easier
 	beta = radToDeg(beta);
 	if (beta > -90 && beta < 90)
 	{
 		output = true;
 	}
 	return output;
}

bool areAnglesValid(float alpha, float beta)
{
	// Author: Dustin Hu
// date: November 20th, 2016
// Purpose: To validate the angle set
// Input: The angle set to validate
// Note: Theta doesn't need to be valid because all thetas are valid
// The base can rotate to any angle
	bool output = false;
	if (isAlphaValid(alpha) && isBetaValid(beta))
	{
		output = true;
	}
	return output;
}

void moveJ2(AngleSet& input)
{
	// Author: Dustin Hu
// Date: November 20th, 2016
// Purpose: To move the second joint to the desired angle
// Input: The angle set to move to
	// It is assumed that the angleSet is valid
	setServoPosition(S4, 1, 0.0333 * input.alpha * input.alpha - 3 * input.alpha - 30 );
}
void moveJ3(AngleSet& input)
{
	// Author: Dustin Hu
// date: November 20, 2016
// Purpose: To move the 3rd joint
// It is assumed that the angle set is valid
	setServoPosition(S4, 2, -0.00001 * pow(input.beta, 3)
				-0.0015 * input.beta * input.beta
				+ 0.9472 * input.beta 
				+ 25);
															
}

float radToDeg(float rad)
{
	return rad * (180.0/PI);
}

int getECValue()
{
	return nMotorEncoder[J1];
}

int getDistance()
{
	return SensorValue[S_ULTRA];
}

float angleToEC(float angle)
{
	return angle * FULL_ROTATION_EC_VALUE - getECValue();
}

void zeroECValue()
{
	nmotorEncoder[J1] = 0;
}

void rotate(bool clockwise, int power)
{
	if (!clockwise)
		power = -1 * power;
	motor[J1] = power;
}

void moveToTarget(int targetEC, int tolerance)
{
	int moveSpeed = 75;
	// PD & Sigmoidy here
	bool cw = true;
	if (targetEC > FULL_ROTATION_EC_VALUE/2.0)
	{
		cw= false;
		targetEC = FULL_ROTATION_EC_VALUE/2.0	- targetEC;
	}
	rotate(cw, moveSpeed);
	
	while ((fabs(getECValue()) - fabs(targetEC)) <= tolerance);
	
	rotate(false, 0);
}

void zeroZAxis()
{
	float minDist = 255;
	int targetEC = 0;
	int tolerance = 0;
	int power = 40;
	
	zeroECValue();
	rotate (true, power);
	while (getECValue() <= FULL_ROTATION_EC_VALUE)
	{
		if (getDistance() < minDist)
		{
			minDist = getDistance();
			targetEC = getECValue();
		}	
	}
	rotate (true, 0);
	zeroECValue();
	moveToTarget(targetEC, tolerance);
	zeroECValue();	
}

bool isPointValid(Point p)
{
	float distBetween;
	float pytho = sqrt(p.x*p.x + p.y*p.y);
	bool isOk = false;
	if (p.z > (-(L1+L2)*sin(PI/6.0)) && p.z < (L1+L2)*cos(PI/6.0))
	{
		distBetween = sqrt(pow(p.x,2)+pow(p.y,2)+pow(p.z,2));
		if (distBetween > L1 && distBetween < (L1+L2))
		{
			isOk = true;
		}
	}
	else if (p.z < (-(L1+L2)*sin(PI/6.0)))
	{
		distBetween = sqrt(pow(pytho-L1*cos(PI/6.0),2)+ pow(p.z-sin(PI/6.0),2));
		if ((p.x >= 0 && p.x-L1*cos(PI/6.0) >= 0) || (p.x < 0 && p.x-L1*cos(PI/6.0) < 0))
		{
			if (distBetween < L2)
			{
				isOk = true;
			}
		}
	}
	else if (p.z > (L1+L2)*cos(PI/6.0))
	{
		
		distBetween = sqrt(pow(pytho-L1*sin(PI/6.0),2)+ pow(p.z-cos(PI/6.0),2));
		if ((p.x >= 0 && p.x-L1*sin(PI/6.0) >= 0) || (p.x < 0 && p.x-L1*sin(PI/6.0) < 0))
		{
			if (distBetween < L2)
			{
				isOk = true;
			}
		}
	}
	return isOk;
}
