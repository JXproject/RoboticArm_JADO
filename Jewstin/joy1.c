#pragma debuggerWindows("joystickSimple");
#include "JoystickDriver.c"

task main()
{
  //Integer variable that allows you to specify a "deadzone" where values (both positive or negative)
  //less than the threshold will be ignored.
  int threshold = 10;

  //Loop Forever
  while(1 == 1)
  {
    //Get the Latest joystick values
    getJoystickSettings(joystick);
   displayString(0,"%f",joystick.joy1_x1);
   motor[motorA]=(int)(joystick.joy1_x1/128.0*70);
    //if(abs(joystick.joy1_y2) > threshold)     // If the right analog stick's Y-axis readings are either above or below the threshold...
    //{
    //display(0,"$$");  // ...the right motor is assigned a power level equal to the right analog stick's Y-axis reading.
    //}
    //else                                      // Else the readings are within the threshold, so
    //{
    //  motor[rightMotor] = 0;                  // ...the right motor is stopped with a power level of 0.
    //}


    //if(abs(joystick.joy1_y1) > threshold)     // If the left analog stick's Y-axis readings are either above or below the threshold...
    //{
    // display(0,"ad"); // ...the left motor is assigned a power level equal to the left analog stick's Y-axis reading.
    //}
    //else                                      // Else if the readings are within the threshold...
    //{
    //  motor[leftMotor] = 0;                   // ...the left motor is stopped with a power level of 0.
    //}
  }
}
