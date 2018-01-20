/*
 * Example code to read the contents of one register from PWM
 * PCA9685
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpsse.h>

#define ADDR    0x40		//7-bit W/r device default address
#define SIZE	0x01		// Size of PWM registers in bytes
#define WCMD	"\x80\x00"	// Write: select MODE1 (0x00)
#define RCMD	"\x81"		// Read command
#define FOUT	"pwm.bin"	// Output file

int main(void)
{
	FILE *fp = NULL;
	char *data = NULL;
	int retval = EXIT_FAILURE;
	struct mpsse_context *pwm = NULL;

	if((pwm = MPSSE(I2C, FOUR_HUNDRED_KHZ, MSB)) != NULL && pwm->open)
	{
		printf("%s initialized at %dHz (I2C)\n", GetDescription(pwm), GetClock(pwm));

		/* Write to  PWM the start address of which register to read */
		Start(pwm);
		Write(pwm, WCMD, sizeof(WCMD) - 1);

		if(GetAck(pwm) == ACK)
		{
			/* Send the PWM read command */
			Start(pwm);
			Write(pwm, RCMD, sizeof(RCMD) - 1);

			if(GetAck(pwm) == ACK)
			{
				/* Read in SIZE bytes from the PWM chip */
				data = Read(pwm, SIZE);
				if(data)
				{
					fp = fopen(FOUT, "wb");
					if(fp)
					{
						fwrite(data, 1, SIZE, fp);
						fclose(fp);

						printf("Dumped %d bytes to %s\n", SIZE, FOUT);
						retval = EXIT_SUCCESS;
					}

					free(data);
				}

				/* Tell libmpsse to send NACKs after reading data */
				SendNacks(pwm);

				/* Read in one dummy byte, with a NACK */
				Read(pwm, 1);
			}
		} else {
			fprintf(stderr, "ERROR: Expected ACK missing from PWM: %s\n", ErrorString(pwm));
		}

		Stop(pwm);
	}
	else
	{
		printf("Failed to initialize MPSSE: %s\n", ErrorString(pwm));
	}

	Close(pwm);

	return retval;
}
