/*
 * Crc.h
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#ifndef CRC_H_
#define CRC_H_

#include "stm32f10x.h"

class Crc {
public:
	static uint16_t calc(uint8_t *p, uint16_t length);
private:
	const static uint8_t _HI[];
	const static uint8_t _LO[];
};

extern Crc crc;

#endif /* CRC_H_ */
