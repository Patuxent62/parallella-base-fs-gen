// e_gpio_test03.c
// This is the eCORE side of the epiphany gpio test
// example 03. The main program initializes the Epiphany system,
// and then loads and launches this test program on an eCore.
// The test program accepts commands from the host program
// through shared memory and performs sample AXI reads and 
// writes to the SHM area as well as the AXI GPIO IP in the
// PL area of the Zynq. The physical GPIO outputs 00p through
// 08n on the parallella can be observed to change state.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

typedef struct
{
	int		cmd;
	int		status;
} shm_packet_t;

typedef struct
{
	unsigned int placekeeper;
	unsigned int coreid;
	unsigned int shmhash;
	unsigned int gpiodir;
	unsigned int gpiodat;
} core_data_t;

int main(void) 
{
	const char			ShmName[] = "gpiotest03";
	shm_packet_t		shmpacket;
	e_coreid_t			coreid;
	e_memseg_t			emem;
	core_data_t			*mydata;
	unsigned*			sharedDATA;
	unsigned*			gpioDATA;
	unsigned*			gpioDIR;
	unsigned			my_row;
	unsigned			my_col;
	int					pdone;

	// Set pointer to Global Address Space GPIO AXI data and direction registers
	// You could perform memory access to GPIO using the e_lib memory functions
	// but this demonstrates that the mapped hardware can be accessed directly 
	// in the linear ephiphany memory space.
	sharedDATA = (unsigned *) 0x8f000000;
	gpioDATA = (unsigned *) 0x8df00000;
	gpioDIR = (unsigned *) 0x8df00004;
	// Set pointer location to local core data registers
	// You could perform memory access to local heap using the e_lib 
	// memory functions but this demonstrates that direct 
	// access in the linear ephiphany memory space is possible.
	mydata = (core_data_t *) 0x6000;

	// Write initial stored data messages in core memory
	mydata->placekeeper = 0x1000;
	mydata->coreid = 0xABC123;
	mydata->shmhash = 0xABC456;
	mydata->gpiodir = 0xABC789;
	mydata->gpiodat = 0x0000BAD1;

	// Who am I? Query the CoreID from hardware.
	coreid = e_get_coreid();
	e_coords_from_coreid(coreid, &my_row, &my_col);
	mydata->coreid = (unsigned) (my_row << 16) + my_col;

	// Update placekeeper message in core memory
	mydata->placekeeper = 0x2000;

	// Attach to the shm segment
	if ( E_OK != e_shm_attach(&emem, ShmName) ) 
	{
		mydata->placekeeper = 0x3000;
		return EXIT_FAILURE;
	}

	// Update placekeeper message in core memory
	mydata->placekeeper = 0x4000;

	pdone = 0;
	do
	{
		// Get the host command from shared memory
		e_read((void*)&emem, &shmpacket, my_row, my_col, NULL, sizeof(shmpacket));
		mydata->placekeeper = 0x5000;

		switch (shmpacket.cmd)
		{
			case '1':
			{
				// Read SHM hash value from start of SHM area
				mydata->shmhash = (unsigned) *(sharedDATA);
				shmpacket.status = 1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x5010;
				break;
			}
			case '2':
			{
				// Initialize all gpio pins for output
				(*(gpioDIR)) = 0xFFFC0000;
				shmpacket.status = 1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x5020;
				break;
			}
			case '3':
			{
				// Initialize all gpio pins for input
				(*(gpioDIR)) = 0x0003FFFF;
				shmpacket.status = 1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x5030;
				break;
			}
			case '4':
			{
				// Toggle all gpio pins to 1
				(*(gpioDATA)) = 0x0003FFFF;
				shmpacket.status = 1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x5040;
				break;
			}
			case '5':
			{
				// Toggle all gpio pins to 0
				(*(gpioDATA)) = 0x00000000;
				shmpacket.status = 1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x5050;
				break;
			}
			case '6':
			{
				// Read gpio pin direction register for output
				mydata->gpiodir = (unsigned) *(gpioDIR);
				shmpacket.status = 1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x5060;
				break;
			}
			case '7':
			{
				// Read gpio pin value register for output
				mydata->gpiodat = (unsigned) *(gpioDATA);
				shmpacket.status = 1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x5070;
				break;
			}
			case 'q':
			{
				// All finished - exit now
				pdone = 1;
				shmpacket.status = -1;
				// Update placekeeper message in core memory
				mydata->placekeeper = 0x6000;
				break;
			}
			default:
			{
				if (shmpacket.cmd > 0)
				{
					// Unrecognized command
					shmpacket.status = 0;
				}
			}
		}
		
		if (shmpacket.cmd > 0)
		{
			shmpacket.cmd = 0;
			e_write((void*)&emem, &shmpacket, my_row, my_col, NULL, sizeof(shmpacket));
		}
	}
	while (pdone < 1);

	// Success	
	mydata->placekeeper = 0x7000;
	return EXIT_SUCCESS;
}
