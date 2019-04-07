
#include <stdint.h>
#include <libc/coretypes.h>
#include <libc/string.h>
#include <core.h>

void *memset(void *_ptr, int value, size_t count)
{
	if (_ptr == NULL)
	{
		panic("memset: dest %p, n %d, caller %x.\n",
			_ptr, count, __builtin_return_address(0));
	};

	for (; count; count--) {
		((uint8_t*)_ptr)[count-1] = (uint8_t)value;
	};

	return _ptr;
}

void *memcpy(void *dest, const void *src, size_t count)
{
	if (dest == NULL || src == NULL)
	{
		panic("memcpy: dest %p, src %p, n %d, caller %x.\n",
			dest, src, count, __builtin_return_address(0));
	};

	if (dest == src || count == 0) { return dest; };

	for (uintptr_t i=0; i<count; i++) {
		((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
	};

	return dest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t n)
{
	size_t			i;
	const uint8_t		*p1=(const uint8_t *)ptr1,
				*p2=(const uint8_t *)ptr2;

	if (ptr1 == NULL || ptr2 == NULL)
	{
		panic("memcmp: ptr1 %p, ptr2 %p, n %d, caller %x.\n",
			ptr1, ptr2, n, __builtin_return_address(0));
	};

	if (ptr1 == ptr2 || n == 0) { return 0; };
	if (n == 0) { return 0; };

	for (i=0; i<n; i++) {
		if (p1[i] != p2[i]) { break; };
	};

	return p1[i] - p2[i];
}

void *memmove(void *dest, const void *src, size_t n)
{
	if (dest == NULL || src == NULL)
	{
		panic("memmove: dest %p, src %p, n %d, caller %x.\n",
			dest, src, n, __builtin_return_address(0));
	};

	if (src == dest || n == 0) { return dest; };
	if (src > dest) {
		return memcpy(dest, src, n);
	} else
	{
		uint8_t		*_dest=(uint8_t *)dest;
		const uint8_t	*_src=(uint8_t *)src;

		for (; n; n--) {
			_dest[n - 1] = _src[n - 1];
		};

		return dest;
	};
}

char *strncpy(char *dest, const char *src, size_t count)
{
	if (dest == NULL || src == NULL)
	{
		panic("strncpy8: dest %p, src %p, caller %x.\n",
			dest, src, __builtin_return_address(0));
	};

	uintptr_t		n=0;

	for (; count && src[n]; count--, n++) {
		dest[n] = src[n];
	};
	for (; count; count--, n++) {
		dest[n] = '\0';
	}

	return dest;
}

int strncmp(const char *str1, const char *str2, int count)
{
	if (str1 == str2) { return 0; };

	if (str1 == NULL || str2 == NULL)
	{
		panic("strncmp8: str1 %p, str2 %p, caller %x.\n",
			str1, str2, __builtin_return_address(0));
	};

	uintptr_t		n=0;

	for (; count > 0; count--, n++)
	{
		if (str1[n] == '\0' || str2[n] == '\0') { break; };
		if (str1[n] != str2[n]) {
			return str1[n] - str2[n];
		};
	};

	if (count)
	{
		if ((str1[n] && (str2[n] == '\0'))
			|| ((str1[n] == '\0') && str2[n]))
		{
			return str1[n] - str2[n];
		};
	};

	return 0;
}

size_t strnlen(const char *str1, size_t maxLen)
{
	size_t		len;

	if (str1 == NULL)
	{
		panic("strnlen8: str1 %p, caller %x.\n",
			str1, __builtin_return_address(0));
	};

	for (len=0; len < maxLen && str1[len] != '\0'; len++) {};
	return len;
}
