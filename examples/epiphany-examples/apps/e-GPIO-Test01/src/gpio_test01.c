// This is the HOST side of the epiphany gpio test
// example 01. The program initializes the Epiphany system,
// and then loads and launches the GPIO eCore test program 01 on
// an eCore. The eCore program sets GPIO pin direction GP00 to input
// and then reads the value on the pin and returns it via shared memory.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <e-hal.h>

const unsigned ShmSize = 128;
const char ShmName[] = "gpiotest01";

int main(int argc, char *argv[])
{
	char			buf[ShmSize];
	e_mem_t			mbuf;
	e_platform_t	platform;
	e_epiphany_t	dev;
	unsigned int	lmem;

	// Uncomment to set the epiphany SDK to provide more debug info
//	e_set_loader_verbosity(H_D1);
//	e_set_host_verbosity(H_D1);

	// initialize local buffer
	memset(buf, 0, ShmSize);

	// initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
	printf("Initializing...\n");
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
	printf("Platform version= %s\n",platform.version);
	printf("Number of chips= %d\n",platform.num_chips);
	printf("Number of Extmems= %d\n",platform.num_emems);
	printf("Chip geometry= %d rows x %d cols\n",platform.rows,platform.cols);

	// Allocate a buffer in shared external memory
	// for message passing from eCore to host.
	printf("Allocating shared memory...\n");
	if (e_shm_alloc(&mbuf, ShmName, ShmSize) != E_OK)
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
	// Load an initial message to shared memory
	// This will get replaced with eCore response
	// and will never be seen if all goes well
	sprintf(buf,"-Initial host message-");
	e_write(&mbuf, 0, 0, 0, (const void*)buf, sizeof(buf));
	printf("\n");

    printf("Opening workgroup (1,1)-(1,1)...\n");
    if (e_open(&dev, 1, 1, 1, 1) == E_OK)
    {
        printf("Success!\n");
        printf("Resetting workgroup...\n");
        if (e_reset_group(&dev) != E_OK)
        {
			printf("Reset operation failed.\n");
			e_close(&dev);
			e_shm_release(ShmName);
			e_finalize();
			return EXIT_FAILURE;
        }

	    // Load the device program onto the selected eCore
	    // and launch after loading.
	    printf("Loading and executing program...\n");
	    if ( E_OK != e_load("e_gpio_test01.elf", &dev, 0, 0, E_TRUE) ) 
	    {
		    printf("Failed to load e_gpio_test01.elf\n");
			e_close(&dev);
			e_shm_release(ShmName);
			e_finalize();
			return EXIT_FAILURE;
	    }

	    // Wait for core program execution to finish, then
	    // read message from shared buffer.
	    printf("Waiting for epiphany execution...\n");
	    usleep(500000);
	    printf("Exectution should be complete.\n");
	    printf("\n");
	    printf("Reading SHM result returned from eCore:\n");
		e_read(&mbuf, 0, 0, 0, (void*)buf, sizeof(buf));
	    printf("%s\n",buf);
	    // Reading placekeeper code
	    printf("Reading eCore placekeeper code...\n");
        e_read(&dev, 0, 0, 0x6000, &lmem, sizeof(lmem));
	    printf("Placekeeper code is 0x%08x\n",lmem);
	    printf("\n");

	    printf("Closing workgroup...\n");
	    e_close(&dev);
    }
    else
    {
            printf("Workgroup 1,1-1,1 could not be opened.\n");
    }

	printf("Releasing SHM and finalizing cleanup...\n");
	e_shm_release(ShmName);
	e_finalize();
	return 0;
}
