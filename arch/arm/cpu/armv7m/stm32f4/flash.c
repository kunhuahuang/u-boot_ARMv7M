/*
 * (C) Copyright 2015
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/stm32.h>

#define STM32_FLASH_KEY1	0x45670123
#define STM32_FLASH_KEY2	0xCDEF89AB

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

unsigned long flash_init (void)
{
	int i;

	for(i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
		flash_info[i].flash_id = FLASH_STM32F4;
		flash_info[i].sector_count = CONFIG_SYS_MAX_FLASH_SECT;
		flash_info[i].size = (1 * 1024 * 1024);
		/* Manual layout of sectors */
		flash_info[i].start[0] = CONFIG_SYS_FLASH_BASE + (i * 1024 * 1024);
		flash_info[i].start[1] = flash_info[i].start[0] + (16 * 1024);
		flash_info[i].start[2] = flash_info[i].start[1] + (16 * 1024);
		flash_info[i].start[3] = flash_info[i].start[2] + (16 * 1024);
		flash_info[i].start[4] = flash_info[i].start[3] + (16 * 1024);
		flash_info[i].start[5] = flash_info[i].start[4] + (64 * 1024);
		flash_info[i].start[6] = flash_info[i].start[5] + (128 * 1024);
		flash_info[i].start[7] = flash_info[i].start[6] + (128 * 1024);
		flash_info[i].start[8] = flash_info[i].start[7] + (128 * 1024);
		flash_info[i].start[9] = flash_info[i].start[8] + (128 * 1024);
		flash_info[i].start[10] = flash_info[i].start[9] + (128 * 1024);
		flash_info[i].start[11] = flash_info[i].start[10] + (128 * 1024);
	}

	return 0x200000;
}

void flash_print_info (flash_info_t *info)
{
	printf("size: %lu\n sector count: %d\n flash_id: %lu\n start: %lx\n protect: %x\n",
			info->size, info->sector_count, info->flash_id, info->start[0], info->protect[0]);
	return;
}

int flash_erase(flash_info_t * info, int first, int last)
{
	unsigned char bank;
	unsigned int i;

	if(info == &flash_info[0]) {
		bank = 0;
	} else if(info == &flash_info[1]) {
		bank = 1;
	} else {
		return -1;
	}

	STM32_FLASH->key = STM32_FLASH_KEY1;
	STM32_FLASH->key = STM32_FLASH_KEY2;

	for(i = first; i <= last; i++) {
		while(STM32_FLASH->sr & STM32_FLASH_SR_BSY);

		STM32_FLASH->cr |= STM32_FLASH_CR_SER;
		if(bank == 0) {
			STM32_FLASH->cr |= (i << STM32_FLASH_CR_SNB_OFFSET);
		} else {
			STM32_FLASH->cr |= ((0x10 | i) << STM32_FLASH_CR_SNB_OFFSET);
		}
		STM32_FLASH->cr |= STM32_FLASH_CR_STRT;

		while(STM32_FLASH->sr & STM32_FLASH_SR_BSY);

		STM32_FLASH->cr &= (~STM32_FLASH_CR_SER);
		STM32_FLASH->cr |= STM32_FLASH_CR_LOCK;
	}

	return 0;
}

int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	unsigned int i;

	while(STM32_FLASH->sr & STM32_FLASH_SR_BSY);

	STM32_FLASH->key = STM32_FLASH_KEY1;
	STM32_FLASH->key = STM32_FLASH_KEY2;

	STM32_FLASH->cr |= STM32_FLASH_CR_PG;
	/* Don't use memcpy to have per-byte writes only */
	for(i = 0; i < cnt; i++) {
		*(uchar*)(addr + i) = src[i];
		while(STM32_FLASH->sr & STM32_FLASH_SR_BSY);
	}
	STM32_FLASH->cr &= (~STM32_FLASH_CR_PG);
	STM32_FLASH->cr |= STM32_FLASH_CR_LOCK;

	return 0;
}
