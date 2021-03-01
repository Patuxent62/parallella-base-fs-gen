// This is the eCore side of the epiphany gpio test
// example 01. The eCore program sets GPIO pin direction GP00 to input
// and then reads the value on the pin and returns it via shared memory.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

int main(void) 
{
	const char			ShmName[] = "gpiotest01"; 
	char				coreres[256] = { 0 };
	e_memseg_t			emem;
	e_coreid_t			coreid;
	unsigned			my_row;
	unsigned			my_col;
	unsigned*			lmem;
	unsigned*			gpioDATA;
	unsigned*			gpioDIR;
	unsigned			temp;

	// Set pointer to Global Address Space GPIO AXI data and direction registers
	gpioDATA = (unsigned *) 0x8df00000;
	gpioDIR = (unsigned *) 0x8df00004;
	// Set pointer to local core placekeeper register address
	// the placekeeper is used to track the progress of the eCore program
	lmem = (unsigned *) 0x6000;
	// Write initial simple placekeeper message in core memory
	(*(lmem)) = 0x1000;

	// Who am I? Query the CoreID from hardware.
	coreid = e_get_coreid();
	e_coords_from_coreid(coreid, &my_row, &my_col);
	(*(lmem)) = 0x2000; // Update placekeeper

	// Attach to the shm segment
	if ( E_OK != e_shm_attach(&emem, ShmName) ) 
	{
		(*(lmem)) = 0x2500; // Update placekeeper
		return EXIT_FAILURE;
	}
	(*(lmem)) = 0x3000; // Update placekeeper

	// Read the current GPIO direction register contents
	temp = (unsigned) *(gpioDIR);
	(*(lmem)) = 0x4000; // Update placekeeper

	// Update the state of bit 0 (GPIO pin GP00) to 1
	// This sets the direction for that pin to: Input
	temp = temp | 0x00000001;
	// Write to the AXI GPIO diretcion register
	(*(gpioDIR)) = temp;
	(*(lmem)) = 0x5000; // Update placekeeper

	// Read  the current GPIO pin data value register contents
	temp = (unsigned) *(gpioDATA);
	// Mask off bit 0
	temp = temp & 0x00000001;
	(*(lmem)) = 0x6000; // Update placekeeper

	// Write the pin value read from AXI to shared memory
	sprintf(coreres, "eCore: GPIO pin GP00 input value is %d", temp);
	e_write((void*)&emem, coreres, my_row, my_col, NULL, sizeof(coreres));
	(*(lmem)) = 0x7000; // Update placekeeper
	return EXIT_SUCCESS;
}
