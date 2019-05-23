#pragma once
#include <string>

#include "module.h"
#include "structures.h"

#define INT16_SGN	0x8000 	// the sign bit of a two's complement 16-bits representation 
#define INT16_BITS	0x7FFF 	// the non-sign bits of a two's complement 16-bits representation
#define BIT_17TH 	0x10000 // the 17th bit 

typedef short int16;
/**** FLAG DEFINITIONS ****/
/* in case the flags werent define somewhere else */
/**************************************************/
#ifndef CF 		// Carry Flag definition	
#define CF 0	
#endif

#ifndef ZF 		// Zero Flag definition
#define ZF 6
#endif

#ifndef SF 		// Sign Flag definition
#define SF 7
#endif

#ifndef OF 		// Overflow Flag definition
#define OF 11
#endif
/*********************/

/**** OPCODE DEFINITION ****/
/* Opcode definition according to the requirements */
#define INC_OPC 		0b01010010 	// Increment by 1 operation (register) 
#define DEC_OPC 		0b01010011 	// Decrement by 1 operation (register level)
#define NEG_OPC 		0b01010100 	// Negation operation (register level)
#define NOT_OPC 		0b01010101 	// Not operation (register level)
#define ADD_OPC 		0b01100001 	// Summation operation (value + register level) 
#define SUB_OPC 		0b01100010 	// Subtraction operation (register level - value)
#define CMP_OPC 		0b01100011 	// Comparison operation (register level - value)
#define MUL_OPC 		0b01100100 	// unsigned multiplication operation (register level * value)
#define IMUL_OPC 		0b01100101 	// signed multiplication operation (register level * value)
#define DIV_OPC 		0b01100110 	// unsigned division operation (register level / value)
#define IDIV_OPC 		0b01100111 	// signed division operation (register level / value)
#define AND_OPC 		0b01101000 	// AND bitwise operation (register level AND value)
#define OR_OPC 			0b01101001 	// OR bitwise operation (register level OR value)
#define SHL_OPC 		0b01101010 	// Shift Logical Left operation (register level << value)
#define SAL_OPC 		0b01101011 	// Shift Arithmetic Left operation (register level << value)
#define SHR_OPC 		0b01101100 	// Shift Logical Right operation (register level >> value)
#define SAR_OPC 		0b01101101 	// Shift Arithmetic Right operation (register level >> value)
#define ADD_REG_OPC 	0b10000001 	// Summation operation (fully register level)
#define SUB_REG_OPC 	0b10000010 	// Subtraction operation (fully register level)
#define CMP_REG_OPC 	0b10000011 	// Comparison operation (fully register level)
#define MUL_REG_OPC 	0b10000100 	// unsigned multiplication operation (fully register level)
#define IMUL_REG_OPC 	0b10000101 	// signed multiplication operation (fully register level)
#define DIV_REG_OPC 	0b10000110 	// unsigned division operation (fully register level)
#define IDIV_REG_OPC 	0b10000111 	// signed division operation (fully register level)
#define AND_REG_OPC 	0b10001000 	// AND bitwise operation (fully register level)
#define OR_REG_OPC 		0b10001001 	// OR bitwise operation fully register level
#define SHL_REG_OPC 	0b10001010 	// Shift Logical Left operation (fully register level)
#define SAL_REG_OPC 	0b10001011 	// Shift Arithmetic Left operation (fully register level)
#define SHR_REG_OPC 	0b10001100 	// Shift Logical Right operation (fully register level)
#define SAR_REG_OPC 	0b10001101 	// Shift Arithmetic Right operation (fully register level)
/*********************/

/* DELAY DEFINITIONS OF OPERATIONS */
/*Defining the number of clock cycles delays per operation */
#define	INC_DELAY	10 				// delay for Increment operation
#define	DEC_DELAY	10 				// delay for Decrement operation
#define	NEG_DELAY	20				// delay for Negation operation
#define	NOT_DELAY	10				// delay for Not operation
#define	ADD_DELAY	20				// delay for Summation operation
#define	SUB_DELAY	20				// delay for Subtraction operation
#define	CMP_DELAY	20				// delay for Comparison operation
#define	MUL_DELAY	50				// delay for unsigned multiplication operation
#define	IMUL_DELAY	50				// delay for signed multiplication operation
#define	DIV_DELAY	50				// delay for unsigned division operation
#define	IDIV_DELAY	50				// delay for signed division operation
#define	AND_DELAY	20				// delay for AND operation
#define	OR_DELAY	20				// delay for OR operation
#define	SHL_DELAY	15				// delay for Shift Logical Left operation
#define	SAL_DELAY	15				// delay for Shift Arithmetic Left operation
#define	SHR_DELAY	15				// delay for Shift Logical Right operation
#define	SAR_DELAY	15				// delay for Shift Arithmetic Right operation
/*********************/



class AluModule : public module
{
public:
	AluModule(string name, int priority = 0);
	void onNotify(message* m); 	// Already provided by the template 
private:
	void operate(); 				// to execute the requested operation
	void setFlag(unsigned flag); 	// to rise a given flag
	uint8_t		dst; 				// Destination register  	
	uint8_t	 	src; 				// Source register 
	uint16_t	op1; 				// value of operand 1 
	uint16_t	op2;				// value of operand 2
	uint16_t	sign; 				// to store the sign of a certain 16-bits signed number
	uint16_t	tmp16; 				// unsigned temporal variable 
	int16_t 	tmp16s; 			// signed temporal variable
	uint16_t	c_outLSB; 			// carry out for SHL / SAL operations
	unsigned	utmp; 				// unsigned temporal variable 
	unsigned	c_outMSB; 			// carry out for SHR / SAR operation
	int	 		tmp; 				// signed temporal variable
	int 		delay; 				// delay variable
};

