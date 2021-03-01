// gpio_test03.c
// This is the HOST side of the epiphany gpio test
// example 03. The program initializes the Epiphany system,
// and then loads and launches a test program on an eCore.
// The test program accepts commands from the host program
// through shared memory and performs sample AXI reads and 
// writes to the SHM area as well as the AXI GPIO IP in the
// PL area of the Zynq. The physical GPIO pins GP00 through
// GN08 on the parallella can be observed to change state.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

#include <e-hal.h>

typedef struct
{
	int		cmd;
	int		status;
} shm_packet_t;

typedef struct
{
	unsigned int	placekeeper;
	unsigned int	coreid;
	unsigned int	shmhash;
	unsigned int	gpiodir;
	unsigned int	gpiodat;
} core_data_t;

void reset_terminal_mode(void);
void set_terminal_mode(struct termios *oldval);

const char			ShmName[] = "gpiotest03";
struct termios		oldsettings;

int main(int argc, char *argv[])
{
	shm_packet_t	shmpacket;
	e_mem_t			mbuf;
	e_platform_t	platform;
	e_epiphany_t	dev;
	core_data_t		edata;
	int				cmd;
	unsigned int	lmem32;

	// Uncomment to set the epiphany SDK to provide more debug info
//	e_set_loader_verbosity(H_D1);
//	e_set_host_verbosity(H_D1);

	// initialize local buffer
	shmpacket.cmd = 0;
	shmpacket.status = 0;

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	printf("Initializing...\n");
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
	printf("Platform version= %s\n",platform.version);
	printf("Chip geometry= %d rows x %d cols\n",platform.rows,platform.cols);

	// Allocate a buffer in shared external memory
	// for message passing between eCore and host.
	printf("Allocating shared memory...\n");
	if (e_shm_alloc(&mbuf, ShmName, sizeof(shmpacket)) != E_OK)
	{
		printf("Initial allocation failed\n");
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

    printf("Opening workgroup (1,1)-(1,1)...\n");
    if (e_open(&dev, 1, 1, 1, 1) == E_OK)
    {
        printf("Resetting workgroup...\n");
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
	    printf("Loading and executing the eCore program...\n");
	    if ( E_OK != e_load("e_gpio_test03.elf", &dev, 0, 0, E_TRUE) ) 
	    {
		    printf("Failed to load eCore with e_gpio_test03.elf\n");
			e_close(&dev);
			e_shm_release(ShmName);
			e_finalize();
			return EXIT_FAILURE;
	    }
	    printf("\n");

	    // Wait for core program execution to finish, then
	    // read message from shared buffer.
	    printf("*** Warning: do not drive any parallella GPIO pins\n");
	    printf("(GP00 through GN08) with low impedance voltage sources\n");
	    printf("if executing Commands 02, 04, or 05. Doing so\n");
	    printf("will permanently damage the parallella ***\n\n");
	    usleep(400000);
	    printf("Execution should be in progress. Commands are:\n");	    
	    printf("1:  Read marker at start of SHM table\n");
	    printf("2:  Set GPIO pin directions to output\n");
	    printf("3:  Set GPIO pin directions to input\n");
	    printf("4:  Set GPIO pin values to 1\n");
	    printf("5:  Set GPIO pin values to 0\n");
	    printf("6:  Read GPIO pin direction register\n");
	    printf("7:  Read GPIO pin value(input) register\n");
	    printf("q:  Quit the polling loop\n\n");

		// Conduct polled commands
		// Change terminal I/O from cooked to raw mode
		set_terminal_mode(&oldsettings);
		// Perform character polling loop
		do
		{
			printf("Enter Command (1, 2, 3, 4, 5, 6, 7, q): ");
			cmd = getc(stdin);
			shmpacket.cmd = cmd;
			printf("%c \r\n",cmd);
			e_write(&mbuf, 0, 0, 0, (const void*) &shmpacket, sizeof(shmpacket));
			printf("\r");
			usleep(100000);
			e_read(&mbuf, 0, 0, 0, &shmpacket, sizeof(shmpacket));
			if (shmpacket.status == 1)
				printf("->Success\r\n");
			if (shmpacket.status == -1)
				printf("->eCore Exiting...\r\n");
		}
		while (cmd != 'q');
		// Change terminal I/O from raw to cooked mode
		reset_terminal_mode();
	    printf("\n");
	    usleep(50000);

	    // Reading epiphany routine stored data
	    printf("Reading eCore program stored data...\n");
        e_read(&dev, 0, 0, 0x6000, &edata, sizeof(edata));
	    printf("e-Core data placekeeper code is 0x%08x\n",edata.placekeeper);
	    printf("e-Core data coreid is 0x%08x\n",edata.coreid);
	    printf("e-Core data shm hash is 0x%08x\n",edata.shmhash);
	    printf("e-Core data gpio dir reg is 0x%08x\n",edata.gpiodir);
	    printf("e-Core data gpio input reg is 0x%08x\n",edata.gpiodat);
	    printf("\n");

	    printf("Closing workgroup...\n");
	    e_close(&dev);
    }
    else
    {
		printf("Epiphany workgroup open operation failed.\n");
    }
     
	// Release the allocated buffer and finalize the
	// e-platform connection.
	e_shm_release(ShmName);
	e_finalize();
	return 0;
}

void reset_terminal_mode(void)
{
	// Restore old terminal settings
	tcsetattr(0, TCSANOW, &oldsettings);
}

void set_terminal_mode(struct termios *oldval)
{
	// Sets terminal mode to RAW
	struct termios		newsettings;
	
	// preserve the current terminal settings
	tcgetattr(0, oldval);
	
	// make a copy with modified character mode and enable it
	memcpy(&newsettings, oldval, sizeof(newsettings));
	cfmakeraw(&newsettings);
	tcsetattr(0, TCSANOW, &newsettings);
	
	// register cleanup handler
	// Note: this is done to restore terminal state if program
	// somehow terminates during polling loop
	atexit(reset_terminal_mode);
}

