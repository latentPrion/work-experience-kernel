
#include <stdarg.h>
#include <arch/ia32/arch.h>
#include <libc/stdio.h>
#include <libc/string.h>
#include <vga.h>
#include <rs232.h>
#include <core.h>

#define PRINTF_BUFFER_NBYTES (PAGE_SIZE * 2)

/* This is our VGA alphanumeric mode driver instance.
 *
 * This is what represents the underlying VGA device that we are printing to.
 * See the source code in vga.c to see how the VGA driver interacts with the
 * underlying VGA alphanumeric mode framebuffer device.
 */
vga_alphanum_dev_t vga;

/* This is our serial driver instance which we use to print output in machine
 * queue since machine queue only listens for output from the machines via
 * serial.
 */
rs232_dev_t rs2320;

/* EXPLANATION:
 * We need someplace to expand the printf formatting into, so let's reserve a
 * buffer of memory for that purpose. This is where we'll expand printf format
 * specifiers.
 */
static char printf_buffer[PRINTF_BUFFER_NBYTES];

int printf_init(void)
{
    int err;

    /** EXPLANATION:
     * Initialize the VGA driver and the VGA device instance, then clear the
     * screen and we're ready to go.
     */

    // This line below sets up the basic VGA driver so you can print messages.
    err = vga_alphanum_init(&vga);
    if (err != 0) {
        return err;
    }

    /* This line initializes the serial device to 115200 baud 8n1 and prepares
     * it for kernel messages.
     */
    err = rs232_initialize(&rs2320, 0);
    if (err != 0) {
        return err;
    }

    /* This line below clears the screen. Uncomment it if you want to see
     * the messages that the bootloader printed before we cleared it.
     */
    vga_alphanum_clear(&vga);
    return 0;
}

int8_t printf_is_initialized(void)
{
    return vga_alphanum_is_initialized(&vga) && rs232_is_initialized(&rs2320);
}

int printf(const char *fmt, ...)
{
    int nprinted;
    va_list args;

    if (fmt == NULL) { return -1; }

    if (!printf_is_initialized()) {
        return 0;
    }

    va_start(args, fmt);
    nprinted = vprintf(fmt, args);
    va_end(args);

    return nprinted;
}

int vprintf(const char *fmt, va_list args)
{
    int nprinted;

    /** EXPLANATION:
     * This is the function that will actually finally print to the underlying
     * VGA driver.
     */

    if (fmt == NULL) { return -1; }

    if (!printf_is_initialized()) {
        return 0;
    }

    nprinted = vsnprintf(printf_buffer, PRINTF_BUFFER_NBYTES, fmt, args);
    if (nprinted <= 0) {
        return nprinted;
    }

    /* Now that we've expanded the printf formatting, give the buffer containing
     * the expanded, processed string to the driver, and have the driver echo
     * the string out to the VGA device.
     */
    rs232_puts(&rs2320, printf_buffer);
    return vga_alphanum_puts(&vga, printf_buffer);
}

int snprintf(char *buff, size_t maxlen, const char *fmt, ...)
{
    int nprinted;
    va_list args;

    if (buff == NULL || fmt == NULL) { return -1; }
    if (maxlen == 0) { return 0; }

    if (!printf_is_initialized()) {
        return 0;
    }

    va_start(args, fmt);
    nprinted = vsnprintf(printf_buffer, PRINTF_BUFFER_NBYTES, fmt, args);
    va_end(args);

    return nprinted;
}

static int sprintnum(
	char *buff, size_t buffsize, uintptr_t num, const uint8_t base,
	int8_t isSigned, int8_t doUpperCase
	)
{
	const int	TMPBUFF_SIZE=28;
	char	b[TMPBUFF_SIZE];
	uintptr_t		nDigitsPrinted=0, buffCursor=0;
	char		hexBaseChar = 'a';

	if (base != 10 && base != 16) {
		return -1;
	};

	if (num == 0)
	{
		buff[buffCursor++] = '0';
		return buffCursor;
	};

	if (isSigned)
	{
		intptr_t snum = (intptr_t)num;

		if (snum < 0)
		{
			buff[buffCursor++] = '-';
			snum = -snum;
			num = (uintptr_t)snum;
		};
	};

	if (base > 10 && doUpperCase) {
		hexBaseChar = 'A';
	};

	for (; num; num /= base)
	{
		uint8_t digit = num % base;

		if (digit < 10) {
			b[nDigitsPrinted++] = digit + '0';
		} else {
			b[nDigitsPrinted++] = digit - 10 + hexBaseChar;
		};
	};

	for (uintptr_t i=0; i<nDigitsPrinted && buffCursor < buffsize; i++) {
		buff[buffCursor++] = b[nDigitsPrinted - i - 1];
	};

	return buffCursor;
}

static inline uintptr_t handleNullPointerArg(char *buff, uintptr_t maxLength)
{
	strncpy(buff, "(null)", maxLength);
	return strnlen("(null)", maxLength);
}

__attribute__((unused))
static uint16_t getNumberOfFormatArgsN(char *format, uintptr_t maxLength)
{
	uintptr_t		i;
	uint16_t		ret=0;

	for (i=0; i < maxLength && format[i] != '\0'; i++)
	{
		if (format[i] != '%') { continue; };

		i++;
		if (format[i] == '\0') { return 0; };

		switch (format[i])
		{
		case '%':
		case '-':
			break;

		default:
			ret++;
			break;
		};
	};

	return ret;
}

int vsnprintf(char *buff, size_t buffMax, const char *str, va_list args)
{
	// Number of chars consumed in the output buffer.
	size_t buffIndex;
	// Number of chars printed in the current loop iteration.
    int nPrinted;

	/**	CAVEAT:
	 * If you ever change the format specifiers (add new ones, take some
	 * away, etc) be sure to check and update the format-specifier-counter
	 * function above so that the value it returns for a format-specifier
	 * count is consistent with the format specifiers that this function
	 * actually takes.
	 **/
	for (
		buffIndex=0;
		(*str != '\0') && (buffIndex < buffMax);
		str++, buffIndex+=nPrinted)
	{
		nPrinted = 0;

		if (*str != '%')
		{
			buff[buffIndex] = *str;
			nPrinted++;
			continue;
		};

		str++;

		// Avoid the exploit "foo %", which would cause buffer overread.
		if (*str == '\0') { return -1; };

		switch (*str)
		{
		case '%':
			buff[buffIndex] = *str;
			nPrinted++;
			break;

		case 'i':
		case 'd': {
			intptr_t snum = va_arg(args, intptr_t);

			nPrinted += sprintnum(
				&buff[buffIndex], buffMax - buffIndex,
				snum, 10, 1, 0);
			break;
		}

		case 'u': {
			uintptr_t unum = va_arg(args, uintptr_t);

			nPrinted += sprintnum(
				&buff[buffIndex], buffMax - buffIndex,
				unum, 10, 0, 0);
			break;
		}

		case 'x':
		case 'p': {
			uintptr_t		unum = va_arg(args, uintptr_t);
			char	*prefix;

			prefix = (*str == 'x')
				? "0x"
				: "pX";

			strncpy(&buff[buffIndex], prefix, buffMax - buffIndex);
			nPrinted += strnlen(prefix, buffMax - buffIndex);

			nPrinted += sprintnum(
				&buff[buffIndex + nPrinted],
				buffMax - (buffIndex + nPrinted),
				unum, 16, 0, 0);
			break;
		}

		case 'X': {
			uintptr_t unum = va_arg(args, uintptr_t);
			char	*prefix = "0x";

			strncpy(&buff[buffIndex], prefix, buffMax - buffIndex);
			nPrinted += strnlen(prefix, buffMax - buffIndex);

			nPrinted += sprintnum(
				&buff[buffIndex + nPrinted],
				buffMax - (buffIndex + nPrinted),
				unum, 16, 0, 1);
			break;
		}

		case 'P': {
            typedef uint64_t paddr_t;
            const unsigned int __VADDR_NBITS__=32, __BITS_PER_BYTE__=8;
			paddr_t *ppnum = va_arg(args, paddr_t*);
			char	*prefix = "Px";

			strncpy(&buff[buffIndex], prefix, buffMax - buffIndex);
			nPrinted += strnlen(prefix, buffMax - buffIndex);

			if (sizeof(*ppnum) > (__VADDR_NBITS__ / __BITS_PER_BYTE__)) {
				paddr_t pnum = *ppnum;

				pnum >>= __VADDR_NBITS__;

				nPrinted += sprintnum(
					&buff[buffIndex + nPrinted],
					buffMax - (buffIndex + nPrinted),
					(pnum & 0xFFFFFFFF), 16, 0, 1);

				buff[buffIndex + nPrinted++] = '_';
			};

			nPrinted += sprintnum(
				&buff[buffIndex + nPrinted], buffMax - buffIndex,
				(*ppnum & 0xFFFFFFFF), 16, 0, 1);
			break;
		}

		// Non recursive string printing.
		case 's': {
			char	*u8Str;
			uintptr_t		u8Strlen;

			u8Str = va_arg(args, char *);

			// Handle NULL pointers passed as string args:
			if ((uintptr_t)u8Str < PAGE_SIZE)
			{
				nPrinted += handleNullPointerArg(
					&buff[buffIndex], buffMax - buffIndex);

				break;
			};

			u8Strlen = strnlen(u8Str, buffMax - buffIndex);
			strncpy(&buff[buffIndex], u8Str, u8Strlen);
			nPrinted += u8Strlen;
			break;
		}

/* Disable support for recursive strings. */
#if 0
		// Same as 's', but recursively parses format strings.
		case 'r': {
			char	*fmtStr;
			uint16_t		numFormatArgs;

			fmtStr = va_arg(args, char *);

			// Handle NULL pointers passed a string args:
			if ((uintptr_t)fmtStr < PAGE_SIZE)
			{
				nPrinted += handleNullPointerArg(
					&buff[buffIndex], buffMax - buffIndex);

				break;
			};

			nPrinted += expandPrintfFormatting(
				&buff[buffIndex], buffMax - buffIndex,
				fmtStr, args);

			numFormatArgs = getNumberOfFormatArgsN(
				fmtStr, buffMax - buffIndex);

			/* Advance the vararg pointer past the args for the
			 * recursive format string argument (basically discard
			 * them).
			 **/
			for (uintptr_t i=0; i<numFormatArgs; i++)
				{ va_arg(args, uintptr_t); };

			break;
		}
#endif

/* Disable support for options. */
#if 0
		// Options.
		case '-':
			str++;
			for (; *str != ' '; str++)
			{
				switch (*str)
				{
				case 'n':
					FLAG_SET(
						*printfFlags,
						DEBUGPIPE_FLAGS_NOLOG);
					break;

				default:
					break;
				};
			};
			break;
#endif

		default:
			va_arg(args, uintptr_t);
			break;
		};
	};

    buff[buffIndex++] = '\0';
	return buffIndex;
}
