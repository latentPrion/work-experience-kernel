#include <stdint.h>
#include <arch/ia32/arch.h>

struct sX8632GdtEntry
{
	uint16_t	limitLow;
	uint16_t	baseLow;
	uint8_t	baseMid;
	uint8_t	opts;
	uint8_t	gran;
	uint8_t	baseHigh;
} __attribute__((packed));

struct sX8632GdtPtr
{
	uint16_t		limit;
	struct sX8632GdtEntry	*baseAddr;
} __attribute__((packed));

struct sX8632GdtEntry		x8632Gdt[] =
{
    /* Mandatory NULL descriptor at position 0.
     * As required by Intel.
     */
	{0, 0, 0, 0, 0, 0},
	// Kernel descriptors.
	{0xFFFF, 0, 0, 0x9A, 0xCF, 0},
	{0xFFFF, 0, 0, 0x92, 0xCF, 0},
	// Userspace descriptors.
	{0xFFFF, 0, 0, 0xFA, 0xCF, 0},
	{0xFFFF, 0, 0, 0xF2, 0xCF, 0},
	// Space for 8 LDTs. Use the LDTs to hold TSSs.
	{0,0,0,0,0, 0},
	{0,0,0,0,0, 0},
	{0,0,0,0,0, 0},
	{0,0,0,0,0, 0}
};

struct sX8632GdtPtr		x8632GdtPtr =
{
	(sizeof(struct sX8632GdtEntry) * 9) - 1,
	x8632Gdt
};

void lgdt(struct sX8632GdtPtr *gdt_ptr)
{
    asm volatile(
        "lgdt (%0)\n"
        :: "r" (gdt_ptr)
        : "memory");
}

int arch_init(void)
{
    lgdt(&x8632GdtPtr);
    return 0;
}
