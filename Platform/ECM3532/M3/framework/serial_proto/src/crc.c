#include <stdint.h>
#include "crc.h"
/**
 * crc8_populate_table
 *
 * @table:	table to be filled.
 */
#define CRC8_TABLE_SIZE (256)
#define CRC8_POLY (0x1D)

uint8_t crc8_table [CRC8_TABLE_SIZE];

void crc8_populate_table()
{
	int i, j;
	const uint8_t msbit = 0x80;
	uint8_t t = msbit;
	crc8_table[0] = 0;
	for (i = 1; i < CRC8_TABLE_SIZE; i *= 2) {
		t = (t << 1) ^ (t & msbit ? CRC8_POLY : 0);
		for (j = 0; j < i; j++)
			crc8_table[i+j] = crc8_table[j] ^ t;
	}
}

uint8_t crc8(uint8_t *pdata, uint16_t len)
{
  uint8_t crc = 0;
	/* loop over the buffer data */
	while (len-- > 0)
		crc = crc8_table[(crc ^ *pdata++) & 0xff];
	return crc;
}

crcType calCrc(uint8_t *buf, uint16_t len)
{
  return crc8(buf, len);
}

void crcInit(void)
{
  crc8_populate_table();
}
