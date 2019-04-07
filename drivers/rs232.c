#include <stddef.h>
#include <libc/size_t.h>
#include <rs232.h>

uint8_t io_read8(uint16_t port)
{
    uint8_t ret;

    asm volatile (
        "inb %1,%0"
            :"=a"(ret)
            :"Nd"(port));

    return ret;
}

void io_write8(uint16_t port, uint8_t val)
{
	asm volatile("outb %0,%1"
        :
        :"a"(val), "Nd" (port));
}

int8_t rs232_is_initialized(rs232_dev_t *d)
{
    return (d->id == 0 || d->id == 1)
        && (d->port == 0x3f8 || d->port == 0x2f8);
}

int rs232_initialize(rs232_dev_t *d, int which)
{
    if (d == NULL) { return -1; }

    d->id = which;

	switch (which) {
	case 0:
		d->port = 0x3f8;
		break;

	case 1:
		d->port = 0x2f8;
		break;

	default:
		return -1;
	};

	/* This next portion of code with the I/O port transactions is taken
	 * from the seL4 microkernel, and maintains their copyright.
	 */
	while (!(io_read8(d->port + 5) & 0x60)); /* wait until not busy */

	io_write8(d->port + 1, 0x00); /* disable generating interrupts */
	io_write8(d->port + 3, 0x80); /* line control register: command: set divisor */
	io_write8(d->port,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
	io_write8(d->port + 1, 0x00); /* set high byte of divisor to 0x00 */
	io_write8(d->port + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
	io_write8(d->port + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

	io_read8(d->port);     /* clear recevier d->port */
	io_read8(d->port + 5); /* clear line status d->port */
	io_read8(d->port + 6); /* clear modem status d->port */

	return 0;
}

int rs232_puts(rs232_dev_t *d, const char *str)
{
    size_t i;

	for (i=0; str[i] != '\0'; i++)
	{
		while (d->port && !(io_read8(d->port + 5) & 0x20));
		io_write8(d->port, str[i]);
	};

    return i;
}
