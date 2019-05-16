#pragma once
#include <string>

#include "module.h"
#include "structures.h"

#define INT16_SGN	0x8000
#define INT16_BITS	0x7FFF
#define BIT_17TH 	0x10000

typedef short int16;
/* FLAG DEFINITIONS */
#ifndef CF
#define CF 0
#endif

#ifndef ZF
#define ZF 6
#endif

#ifndef SF
#define SF 7
#endif

#ifndef OF
#define OF 11
#endif
/*********************/

/* OPCODE DEFINITION */
#define INC_OPC 		0b01010010
#define DEC_OPC 		0b01010011
#define NEG_OPC 		0b01010100
#define NOT_OPC 		0b01010101
#define ADD_OPC 		0b01100001
#define SUB_OPC 		0b01100010
#define CMP_OPC 		0b01100011
#define MUL_OPC 		0b01100100
#define IMUL_OPC 		0b01100101
#define DIV_OPC 		0b01100110
#define IDIV_OPC 		0b01100111
#define AND_OPC 		0b01101000
#define OR_OPC 			0b01101001
#define SHL_OPC 		0b01101010
#define SAL_OPC 		0b01101011
#define SHR_OPC 		0b01101100
#define SAR_OPC 		0b01101101
#define ADD_REG_OPC 	0b10000001
#define SUB_REG_OPC 	0b10000010
#define CMP_REG_OPC 	0b10000011
#define MUL_REG_OPC 	0b10000100
#define IMUL_REG_OPC 	0b10000101
#define DIV_REG_OPC 	0b10000110
#define IDIV_REG_OPC 	0b10000111
#define AND_REG_OPC 	0b10001000
#define OR_REG_OPC 		0b10001001
#define SHL_REG_OPC 	0b10001010
#define SAL_REG_OPC 	0b10001011
#define SHR_REG_OPC 	0b10001100
#define SAR_REG_OPC 	0b10001101
/*********************/

/* DELAY DEFINITIONS OF OPERATIONS */
#define	INC_DELAY	10
#define	DEC_DELAY	10
#define	NEG_DELAY	20
#define	NOT_DELAY	10
#define	ADD_DELAY	20
#define	SUB_DELAY	20
#define	CMP_DELAY	20
#define	MUL_DELAY	50
#define	IMUL_DELAY	50
#define	DIV_DELAY	50
#define	IDIV_DELAY	50
#define	AND_DELAY	20
#define	OR_DELAY	20
#define	SHL_DELAY	15
#define	SAL_DELAY	15
#define	SHR_DELAY	15
#define	SAR_DELAY	15
/*********************/



class AluModule : public module
{
public:
	AluModule(string name, int priority = 0);
	void onNotify(message* m);
private:
	void operate();
	void setFlag(unsigned flag);
	uint8_t		dst;
	uint8_t	 	src;
	uint16_t	op1;
	uint16_t	op2;
	uint16_t	sign;
	uint16_t	tmp16;
	int16_t 	tmp16s;
	uint16_t	c_outLSB;
	unsigned	utmp;
	unsigned	c_outMSB;
	int	 		tmp;
	int 		delay;
};

// struct decode_registers alu_regs;