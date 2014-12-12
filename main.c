#include <msp430.h> 
#include "rangeFinder.h"
#include "testMethods.h"
#include "motorDriver.h"

/*
 * main.c
 * Author: Ian R Goodbody
 * Function: Implements maze navigation
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    initRangeFinders();
    initMotors();
    stop();

    P1DIR |= LEFT_LED|RIGHT_LED;
    P1OUT &= ~(LEFT_LED|RIGHT_LED);


    int fBuffer[BUFFER_LN]; // Code probably unnecessary but it cannot hurt
    int lBuffer[BUFFER_LN];
    int rBuffer[BUFFER_LN];

    int fMedian;
    int lMedian;
    int rMedian;

    unsigned int lWheelSpeed = 4040;
    unsigned int rWheelSpeed = 4000;
    unsigned int runTime = 350;

    int i;

    fillBuffers(fBuffer, lBuffer, rBuffer);

    while(1)
    {

        P1OUT &= ~(LEFT_LED|RIGHT_LED); // turn off LEDs
    	stop();

    	for (i = BUFFER_LN; i > 0; i--)
    	{
    		readFront(fBuffer);
    		waitMiliseconds(1);
    		readLeft(lBuffer);
    		waitMiliseconds(1);
    		readRight(rBuffer);
    		waitMiliseconds(1);
    	}
    	fMedian = median(fBuffer);
    	lMedian = median(lBuffer);
    	rMedian = median(rBuffer);

    	if(fMedian < 0x01F0) // Crash into wall test; ~2.5 inch threshold
    	{
    		if(lMedian < 0x01FF) // There is no wall remotely close to the left side, 
    		{                    // Initiate sweeping turn
    			lWheelSpeed = 2600;
    			rWheelSpeed = 5000;
    		}
    		else if(lMedian < 0x0266) // Getting too far from the wall start turning in
    		{
    			P1OUT |= LEFT_LED; // Red LED on, green off
    			P1OUT &= ~RIGHT_LED;
    			rWheelSpeed = 4270;
    			lWheelSpeed = 4040;
    		}
    		else if(lMedian > 0x02E4) // Getting too close to the wall start turning out
    		{
    			P1OUT |= RIGHT_LED; // Green LED on, red off
    			P1OUT &= ~LEFT_LED;
    			rWheelSpeed = 3900;
    			lWheelSpeed = 4040;
    		}
    		else // Acceptable distance from wall, cruise forward normally
    		{
    			//P1OUT |= RIGHT_LED;
    			P1OUT &= ~(RIGHT_LED|LEFT_LED);
    			rWheelSpeed = 4000;
    			lWheelSpeed = 4040;
    		}
    		setLeftWheel(lWheelSpeed, FORWARD);
    		setRightWheel(rWheelSpeed, FORWARD);
    		runTime = 350;
    	}
    	else
    	{
    		// About to run into a wall, initiate hard right turn
    		P1OUT |= RIGHT_LED|LEFT_LED;
    		setLeftWheel(5080,FORWARD);
    		setRightWheel(5260, BACKWARD);
    		runTime = 450;
    	}
    	go();
    	waitMiliseconds(runTime);
    }
}
// Merry Christmas Captain Trimble!
