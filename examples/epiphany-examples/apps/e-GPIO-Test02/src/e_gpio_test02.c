// This is the eCore side of the epiphany gpio test
// example 02. The eCore program sets the GPIO pin direction for the 
// specified pin to output and then sets the value on the pin to off 
// or on as specified. The GPIO bank bit position and value for 
// the pin are passed in shared memory to the eCore program.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

typedef struct
{
	unsigned		bitpos; // bit position 00 (GP00) to 17 (GN09)
	unsigned		state; // bit state- either 0 or 1
	int				status; // 0 - Failure  1 - success
} shm_packet_t;

int main(void) 
{
	const char			ShmName[] = "gpiotest02"; 
	e_memseg_t			emem;
	e_coreid_t			coreid;
	shm_packet_t		packet;
	unsigned			my_row;
	unsigned			my_col;
	unsigned*			lmem;
	unsigned*			gpioDATA;
	unsigned*			gpioDIR;
	unsigned			gpio_bitmask;
	unsigned			gpio_bitvalue;
	unsigned			dtemp;
	unsigned			vtemp;

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

	// Read the SHM packet for the GPIO command
	e_read((void*)&emem, &packet, my_row, my_col, NULL, sizeof(packet));
	(*(lmem)) = 0x4000; // Update placekeeper

	// Calcuate the data register bitmask and value by shifting 
	// to the correct GPIO bit position
	gpio_bitmask = ~(1 << packet.bitpos);
	gpio_bitvalue = packet.state << packet.bitpos;
	(*(lmem)) = 0x5000; // Update placekeeper

	// Read the current GPIO direction and value register contents
	// Note: Xilinx AXI GPIO 2.0 IP documentation states that 
	// reading the value register for pins configured as outputs always 
	// returns 0, however, the actual IP appears to return the current  
	// value of the ouput, which is necessary to preserve the state
	// of other outputs using the logic below
	dtemp = (unsigned) *(gpioDIR);
	vtemp = (unsigned) *(gpioDATA);
	(*(lmem)) = 0x6000; // Update placekeeper

	// Mask off the state of the selected direction bit, making it 0
	// This sets the direction for that pin alone to: Output
	dtemp = dtemp & gpio_bitmask;
	// Mask off the selected value bit and update its value
	// The logic below attempts to preserve the state of all outputs
	// provided that the last state of the outputs is returned in
	// the GPIO value register read above
	vtemp = (vtemp & gpio_bitmask) | gpio_bitvalue;
	(*(lmem)) = 0x7000; // Update placekeeper
	
	// Write to the AXI GPIO direction register
	(*(gpioDIR)) = dtemp;
	// Write to the AXI GPIO value register
	(*(gpioDATA)) = vtemp;
	(*(lmem)) = 0x8000; // Update placekeeper

	// Update status in SHM packet and write packet
	packet.status = 1;
	e_write((void*)&emem, &packet, my_row, my_col, NULL, sizeof(packet));
	(*(lmem)) = 0x9000; // Update placekeeper
	return EXIT_SUCCESS;
}
