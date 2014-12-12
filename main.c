#include <msp430.h> 
#include "rangeFinder.h"
#include "testMethods.h"
#include "motorDriver.h"

/*
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    initRangeFinders();
    initMotors();
    stop();

    P1DIR |= LEFT_LED|RIGHT_LED;
    P1OUT &= ~(LEFT_LED|RIGHT_LED);


    int fBuffer[BUFFER_LN];
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

        P1OUT &= ~(LEFT_LED|RIGHT_LED);
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
    		if(lMedian < 0x01FF) // Thar be nothin' off the port side captain! > 5 in
    		{// Commence conrner rounding turn
    			lWheelSpeed = 2600;
    			rWheelSpeed = 5000;
    		}
    		else if(lMedian < 0x0266) // Captain! We are getting into unmapped waters best bring land back in sight
    		{
    			P1OUT |= LEFT_LED;
    			P1OUT &= ~RIGHT_LED;
    			rWheelSpeed = 4270;
    			lWheelSpeed = 4040;
    		}
    		else if(lMedian > 0x02E4) // Yar! Land ho off the port side captain we are goint to hit it
    		{
    			P1OUT |= RIGHT_LED;
    			P1OUT &= ~LEFT_LED;
    			rWheelSpeed = 3900;
    			lWheelSpeed = 4040;
    		}
    		else // Clear waters ahead sir, looks like smooth sailing
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
    		// Land is coming up fast captain. Hard turn to starboard
    		P1OUT |= RIGHT_LED|LEFT_LED;
    		setLeftWheel(5080,FORWARD);
    		setRightWheel(5260, BACKWARD);
    		runTime = 450;
    	}
    	go();
    	waitMiliseconds(runTime);
    }
}
