/*

	shift.c
	Süleyman Savas
	2013-07-09

*/

#include <stdlib.h>
#include <stdio.h>

#include "shift.h"
#include "e_lib.h"
#include "idct.h" 
#include "common_buffers.h"
#include "actors.h"

// Actor instantiation
shift_t shift;

// Port instantiation
inputPort_t X;
outputPort_t Y;

// Core struct instantiaton
core_t me;

unsigned timerValue = 0;

void init();

void main(){

// Configure the timers
 e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX); 

 // Start the timer (countdown from 0xFFFFFFFF)
 e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);

	init();
	int i = 0;

	// wait for the go (which means actors are connected)
	while(Mailbox.pGo[me.corenum] == 0);

timerValue = e_ctimer_get(E_CTIMER_0);
Mailbox.pTimer0[me.corenum] = E_CTIMER_MAX - timerValue;
e_ctimer_stop(E_CTIMER_0);

e_ctimer_set(E_CTIMER_1, E_CTIMER_MAX); 
e_ctimer_start(E_CTIMER_1, E_CTIMER_CLK);

	while(i < IN_BUFFER_SIZE)
	{
		// action #1
		int a = port_read(&X);
		int b = port_read(&X);
		int c = port_read(&X);
		int d = port_read(&X);

#if 1
		port_write(&Y, a >> 6); i++;
		port_write(&Y, b >> 6); i++;
		port_write(&Y, c >> 6); i++;
		port_write(&Y, d >> 6); i++;
		// end of action #1
#endif

#if 0
		Mailbox.pOutputBuffer[i] = a >> 6; i++;
		Mailbox.pOutputBuffer[i] = b >> 6; i++;
		Mailbox.pOutputBuffer[i] = c >> 6; i++;
		Mailbox.pOutputBuffer[i] = d >> 6; i++;
#endif

	}

#if 0
*Mailbox.pOutputReady = 13;
#endif 

// Get timer value, find the elapsed time and stop 
timerValue = e_ctimer_get(E_CTIMER_1);
Mailbox.pTimer1[me.corenum] = E_CTIMER_MAX - timerValue;
e_ctimer_stop(E_CTIMER_1);
}

void init(){
	
	// Init core enumerations
	me.coreID  = e_get_coreid();
	e_coords_from_coreid(me.coreID, &me.row, &me.col);
	//me.row     = me.row - E_FIRST_CORE_ROW;
	//me.col     = me.col - E_FIRST_CORE_COL;
	//me.corenum = me.row * E_COLS_IN_CHIP + me.col;
    me.corenum = me.row * e_group_config.group_cols + me.col;
	// We need an adjustment to the corenum because of corenum 12 being unused
    // O.J: dafuq will be negative on core 0
	me.corenum--;

	// Initialize the mailbox shared buffer pointers
	Mailbox.pBase = (void *)  MAILBOX_ADDRESS;
	Mailbox.pGo = Mailbox.pBase + offsetof(mbox_t, go[0]);
	Mailbox.pReady = Mailbox.pBase + offsetof(mbox_t, ready[0]);
	Mailbox.pClocks = Mailbox.pBase + offsetof(mbox_t, clocks);
	
#if 0
	// ## For debugging ##
Mailbox.pOutputBuffer = Mailbox.pBase + offsetof(mbox_t, output_buffer[0]);
Mailbox.pOutputReady = Mailbox.pBase + offsetof(mbox_t, output_ready);
#endif

	Mailbox.pTimer0 = Mailbox.pBase + offsetof(mbox_t, timer0);
	Mailbox.pTimer1 = Mailbox.pBase + offsetof(mbox_t, timer1);

	me.count = 0;
	
	// Init the ports
	init_input_port(&X);
	init_output_port(&Y);

	// Set the port pointers of the actor struct
	shift.X = (me.coreID << 20) | (int) &X;
	shift.Y = (me.coreID << 20) | (int) &Y;

	shift.coreID = me.coreID;

	// Set the global actor pointer
	actors.shift = (me.coreID << 20) | (int) &shift;

	// Init the host-accelerator sync signals
	// Let the host know that we are ready (for sync)	
	Mailbox.pReady[me.corenum] = 1;
}
