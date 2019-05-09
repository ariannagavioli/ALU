#pragma once
#include <string>

#include "module.h"
#include "structures.h"

#define UINT16_MAX 	0xFFFF
#define INT16_SGN	0x8000
#define INT16_BITS	0x7FFF
#define BIT_17TH 	0x10000

#define INC			0b01010010
#define DEC			0b01010011
#define NEG			0b01010100
#define NOT			0b01010101
#define ADD			0b01100001
#define SUB			0b01100010
#define CMP			0b01100011
#define MUL			0b01100100
#define IMUL		0b01100101
#define DIV			0b01100110
#define IDIV		0b01100111
#define AND			0b01101000
#define OR			0b01101001
#define SHL			0b01101010
#define SAL			0b01101011
#define SHR			0b01101100
#define SAR			0b01101101
#define ADD_REG		0b10000001
#define SUB_REG		0b10000010
#define CMP_REG		0b10000011
#define MUL_REG		0b10000100
#define IMUL_REG 	0b10000101
#define DIV_REG		0b10000110
#define IDIV_REG	0b10000111
#define AND_REG		0b10001000
#define OR_REG		0b10001001
#define SHL_REG		0b10001010
#define SAL_REG		0b10001011
#define SHR_REG  	0b10001100
#define SAR_REG  	0b10001101


class AluModule : public module
{
public:
	AluModule(string name, int priority = 0);
	void onNotify(message* m);
private:
	void operate();
	uint8_t dst;
	uint16_t op1;
	uint16_t op2;
	uint16_t sign;
	// unsigned utmp;
	// int tmp;
};
