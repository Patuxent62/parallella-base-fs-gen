// This is the HOST side of the epiphany gpio test
// example 02. Via the command line, the user passes to the host 
// program a GPIO bank pin position and I/O output value to set for 
// the GPIO pin. The program initializes the Epiphany system, and then 
// loads and launches the GPIO eCore test program 02 on an eCore. The 
// eCore program sets the GPIO pin direction for the specified pin to 
// output and then sets the value on the pin to off or on as specified. 
// The GPIO bank bit position and value for the pin are passed in 
// shared memory to the eCore program.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <e-hal.h>

typedef struct
{
	unsigned		bitpos; // bit position 00 (GP00) to 17 (GN09)
	unsigned		state; // bit state- either 0 or 1
	int				status; // 0 - Failure  1 - success
} shm_packet_t;

const char ShmName[] = "gpiotest02";

int main(int argc, char *argv[])
{
	e_mem_t			mbuf;
	e_epiphany_t	dev;
	shm_packet_t	packet;
	int				p1,p2;
	unsigned int	lmem;

	// Uncomment to set the epiphany SDK to provide more debug info
//	e_set_loader_verbosity(H_D1);
//	e_set_host_verbosity(H_D1);

	// Read the command line input and error check it
	if (argc < 3)
	{
		printf("gpio_test02- Sets a user selected eCore GPIO pin to\n");
		printf("output a user specified on/off state\n\n");
		printf("Format is: \n");
		printf("gpio_test02.elf <bitpos> <value>\n");
		printf("Where: \n");
		printf("<bitpos> = integer from 0 to 17 decimal\n");
		printf("<value> = integer 0-(off, 0V) or 1-(on, 2.5V)\n\n");
	    printf("*** Warning: do not drive any parallella GPIO pins\n");
	    printf("set as outputs by this program with low impedance\n");
	    printf("voltage sources. Doing so will permanently damage\n");
	    printf("the parallella ***\n\n");
		return EXIT_FAILURE;
	}
	// Read input into shared memory packet and error check
	p1 = sscanf(argv[1],"%u",&packet.bitpos);
	p2 = sscanf(argv[2],"%u",&packet.state);
	packet.status = 0;
	if ((p1 < 1) || (p2 < 1))
	{
		printf("gpio_test02.elf <bitpos> <value>\n");
		printf("<bitpos> = integer from 0 to 17 decimal\n");
		printf("<value> = integer 0 or 1\n");
		return EXIT_FAILURE;
	}
	if ((packet.bitpos > 17) || (packet.state > 1))
	{
		printf("gpio_test02.elf <bitpos> <value>\n");
		printf("<bitpos> = integer from 0 to 17 decimal\n");
		printf("<value> = integer 0 or 1\n");
		return EXIT_FAILURE;
	}

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	e_init(NULL);
	e_reset_system();

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	if (e_shm_alloc(&mbuf, ShmName, sizeof(packet)) != E_OK)
	{
		printf("Initial eCore SHM allocation failed\n");
        printf("Error string is %s\n",strerror(errno));
		printf("Attempting an SHM attach operation...\n");
		if (e_shm_attach(&mbuf, ShmName) != E_OK)
		{
			printf("Attach failed. Failed to allocate shared memory.\n");
			printf("Error string is %s\n", strerror(errno));
			e_finalize();
			return EXIT_FAILURE;
		}
	}

	// Write the command packet to shared memory prior to launching 
	// the eCore program, which will then asynchronously read 
	// and execute the command
	e_write(&mbuf, 0, 0, 0, (const void*) &packet, sizeof(packet));

	// Open epiphany platform workgroup
    if (e_open(&dev, 1, 1, 1, 1) == E_OK)
    {
        if (e_reset_group(&dev) != E_OK)
        {
			printf("Epiphany workgroup reset operation failed.\n");
			e_close(&dev);
			e_shm_release(ShmName);
			e_finalize();
			return EXIT_FAILURE;
        }

	    // Load the device program onto the selected eCore
	    // and launch after loading.
	    if ( E_OK != e_load("e_gpio_test02.elf", &dev, 0, 0, E_TRUE) ) 
	    {
		    printf("Failed to load eCore program e_gpio_test01.elf\n");
			e_close(&dev);
			e_shm_release(ShmName);
			e_finalize();
			return EXIT_FAILURE;
	    }

	    // Wait for core program execution to finish, then
	    // read message from shared buffer.
	    printf("eCore setting GPIO bit %d to %d.\n",packet.bitpos,packet.state);
	    usleep(500000);
		e_read(&mbuf, 0, 0, 0, (void*) &packet, sizeof(packet));
		if (packet.status == 1)
			printf(" ->Success\n");
		else
			printf(" ->Failed\n");
	    // Reading placekeeper code
        e_read(&dev, 0, 0, 0x6000, &lmem, sizeof(lmem));
	    printf("eCore placekeeper code is 0x%08x\n",lmem);
	    e_close(&dev);
    }
    else
    {
            printf("Workgroup 1,1-1,1 could not be opened.\n");
    }

	e_shm_release(ShmName);
	e_finalize();
	return 0;
}
