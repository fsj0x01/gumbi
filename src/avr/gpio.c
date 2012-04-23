#include "gpio.h"
#include "mcp23s17.h"

/* Handler for GPIO mode. */
void gpio(void)
{
	uint8_t loop = TRUE, i = 0, index = 0;
	uint8_t errbuf[BLOCK_SIZE] = { 0 };
	uint8_t rx[BLOCK_SIZE] = { 0 };

	mcp23s17_enable();

	while(loop)
	{
		read_data((uint8_t *) &hgpio, sizeof(hgpio));
		
		for(i=0; i<hgpio.num_cmd && loop; i++)
		{
			switch(hgpio.cmd[i].action)
			{
				case HIGH:
					configure_pin_immediate(hgpio.cmd[i].pin, 'w');
					set_pin_immediate(hgpio.cmd[i].pin, 1);
					break;
				case LOW:
					configure_pin_immediate(hgpio.cmd[i].pin, 'w');
					set_pin_immediate(hgpio.cmd[i].pin, 0);
					break;
				case READ:
					configure_pin_immediate(hgpio.cmd[i].pin, 'r');
					rx[index++] = get_pin(hgpio.cmd[i].pin);
					//write_data((uint8_t *) &c, 1);
					break;
				case EXIT:
					loop = FALSE;
					break;
				default:
					snprintf((char *) &errbuf, sizeof(errbuf), "The specified GPIO action 0x%X (%d/%d) is not supported", hgpio.cmd[i].action, i, hgpio.num_cmd);
					nack();
					write_string((char *) &errbuf);
					break;
			}
		}

		ack();

		if(index > 0)
		{
			write_data((uint8_t *) &rx, index);
			index = 0;
		}
	}
	
	mcp23s17_disable();

	return;
}