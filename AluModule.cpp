#include <cstring>
#include <iostream>

#include "AluModule.h"


using namespace std;

AluModule::AluModule(string name, int priority):module(name, priority) {
	string Alu = "Alu";
	
	//This module is the one which triggers the whole system, so it creates a message in its constructor
	if(getName() == Alu){
		message* myMessage = new message();
		myMessage->valid = 1;
		myMessage->timestamp = getTime();
		strcpy(myMessage->source, getName().c_str());
		strcpy(myMessage->dest, "Fetch");
		myMessage->magic_struct = NULL;
		
		sendWithDelay(myMessage, 10);
	}
}

//All custom code must be called from this method
void AluModule::onNotify(message* m) {
	//Don't forget to check if the message was for me since we are in a broadcast environment
	if(m->dest == getName()){
		string Alu = "Alu";
		
		
		cout << "Message " << m->id << ": Sent at " << m->timestamp << ", current timestamp " << getTime() << ". My name is " << getName() << "." << endl;
		
		//Send some test messages
		if(m->timestamp < 50) {
			if(getName() == Alu){
				message* myMessage = new message();
				myMessage->valid = 1;
				myMessage->timestamp = getTime();
				strcpy(myMessage->source, getName().c_str());
				strcpy(myMessage->dest, "Fetch");

				/*** OUR CODE ***/
				operate();

				myMessage->magic_struct = NULL;
				
				sendWithDelay(myMessage, 10);
			}
			else{
				message* myMessage = new message();
				myMessage->valid = 1;
				myMessage->timestamp = getTime();
				strcpy(myMessage->source, getName().c_str());
				strcpy(myMessage->dest, "Alu");
				myMessage->magic_struct = NULL;
				
				sendWithDelay(myMessage, 10);
			}
		}
	}
}

void AluModule::operate() {

	sign = 0;
	global_regs.flag &= !(1 << CF); // carry flag
	global_regs.flag &= !(1 << ZF);	// zero flag
	global_regs.flag &= !(1 << SF);	// sign flag
	global_regs.flag &= !(1 << OF);	// Overflow flag
	dst = alu_regs.operand2;

	switch(alu_regs.opcode){
		case INC :			// INC
			/*According to the book, the flags after this operation aren't modify */
			uint8_t src = alu_regs.operand1;
			global_regs.general_regs[src]++;
			break;

		case DEC : 			// DEC
			/*According to the book, the flags after this operation aren't modify */
			uint8_t src = alu_regs.operand1;
			global_regs.general_regs[src]--;
			break;

		case NEG :			// NEG: two complement
			/* Flags not specified on the book, not even operation, review operation */
			unsigned tmp;
			uint8_t src = alu_regs.operand1;

			tmp = global_regs.general_regs[src];
			tmp = 0 - tmp;
			global_regs.general_regs[src] = tmp;
			break;

		case NOT :			// NOT: negate bitwise
			unsigned tmp;
			uint8_t src = alu_regs.operand1;

			tmp = global_regs.general_regs[src];
			tmp ~= tmp;
			global_regs.general_regs[src] = tmp;
			break;

		case ADD :			//ADD
			int tmp;

			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op1 + op2;
			global_regs.general_regs[dst] = tmp;

			if(int16_t(tmp) == 0) 
				setFlag(ZF);

			if(int16_t(tmp < 0))
				setFlag(SF);

			if(unsigned(tmp) > UINT16_MAX)
				setFlag(OF);
			break;

		case SUB :			//SUB
			int tmp;

			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;
			global_regs.general_regs[dst] = tmp;

			if(int16_t(tmp) == 0)
				setFlag(ZF);

			if(int16_t(tmp < 0))
				setFlag(SF);

			if(uint16_t(tmp) > UINT16_MAX)
				setFlag(OF);
			break;

		case CMP :			//CMP
			int tmp;

			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;

			if(int16_t(tmp) == 0)
				setFlag(ZF);

			if(int16_t(tmp) < 0)
				setFlag(SF);

			if(uint16_t(tmp) >= UINT16_MAX)
				setFlag(OF);
			break;

		case MUL :			// MUL
			unsigned tmp;

			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op1 * op2;
			global_regs.general_regs[dst] = tmp;
			
			if(int16_t(tmp) == 0)		// According to x86 ISA, this is undefined 
				setFlag(ZF);
			
			if(uint16_t(tmp) > UINT16_MAX)
				setFlag(OF);
			break;

		case IMUL :			//IMUL
			unsigned tmp;

			op1 = alu_regs.operand1;						// Cast to unsigned 15 bits 
			op2 = global_regs.general_regs[dst];
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN);	// Final sign is result of xor of the operands signs
			op1 &= INT16_BITS;
			op2 &= INT16_BITS;

			tmp = op1 * op2;

			if(uint16_t(tmp) >= INT16_BITS)					// The MSB
				setFlag(OF);

			if(sign)				
				setFlag(SF);

			if(int16_t(tmp) == 0)
				setFlag(ZF);

			tmp = tmp | sign;
			global_regs.general_regs[dst] = int16_t(tmp);

			break;

		case DIV :			//DIV
			unsigned tmp;
			
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op2 / op1;
			global_regs.general_regs[dst] = tmp;
			
			/* x86 ISA Ref: flags are undefined  */
			if(uint16_t(tmp) == 0)
				setFlag(ZF);

			// x86, this is undefined (every flag)
			// if(uint16(tmp) >= UINT16_MAX)
			// 	setFlag(OF);
			/************************************/

			break;

		case IDIV :			//IDIV
			unsigned tmp;

			op1 = alu_regs.operand1;						// I am putting to zero the first bit
			op2 = global_regs.general_regs[dst];
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN);	// Final sign is result of xor of the operands signs

			op1 &= INT16_BITS;
			op2 &= INT16_BITS;

			tmp = op2 / op1;

			if(uint16_t(tmp) > INT16_BITS)					// The 16th bit is for the sign!
				setFlag(OF);

			/* x86 ISA Ref: flags are undefined  */
			if(sign)				
				setFlag(SF);

			if(int16(tmp) == 0)
				setFlag(ZF);
			/**************************************/

			tmp = tmp | sign;
			global_regs.general_regs[dst] = int16_t(tmp);

			break;

		case AND :			//AND
			uint16_t tmp;

			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op1 & op2;
			global_regs.general_regs[dst] = tmp;
			sign = tmp & INT16_SGN;
			
			if(sign)
				setFlag(SF);

			if(tmp == 0)
				setFlag(ZF);
			break;

		case OR :			//OR
			uint16_t tmp, sign;
			
			op1 = alu_regs.operand1;
			op2 = alu_regs.general_regs[dst];
			tmp = op1 | op2;
			global_regs.general_regs[dst] = tmp;
			sign = tmp & INT16_SGN;

			if(sign)
				setFlag(SF);

			if (tmp == 0) 
				setFlag(ZF);
			break;

		case SHL :			//SHL
			unsigned tmp, c_out;
			uint16_t sign; 
			
			op1 = alu_regs.operand1;
			op2 = alu_regs.general_regs[dst];
			tmp = op2 << op1;
			sign = tmp & INT16_SGN;
			c_out = tmp & BIT_17TH;
			global_regs.general_regs[dst] = tmp;

			if (sign != 0)
				setFlag(SF);

			if (int16_t(tmp) == 0)
				setFlag(ZF);

			if (c_out == BIT_17TH)
				setFlag(CF);
			break;

		case SAL :			//SAL
			unsigned tmp, c_out;
			uint16_t sign; 

			op1 = alu_regs.operand1;
			op2 = alu_regs.general_regs[dst];
			tmp = op2 << op1;
			sign = tmp & INT16_SGN;
			c_out = tmp & BIT_17TH;
			global_regs.general_regs[dst] = uint16_t(tmp);

			if (sign != 0)
				setFlag(SF);

			if (int16_t(tmp) == 0)
				setFlag(ZF);

			if (c_out == BIT_17TH)
				setFlag(CF);
			break;

		case SHR :			//SHR
			uint16_t c_out, sign;
			
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];

			while (op1) {
				c_out = op2 & 1;
				op2 >>= 1;
				op1--;
			}
			
			sign = op2 & INT16_SGN;
			global_regs.general_regs[dst] = tmp;

			if (sign) 
				setFlag(SF);

			if (op2 == 0)
				setFlag(ZF);

			if (c_out)
				setFlag(CF);

			break;
		
		case SAR :			//SAR
			int tmp2 = alu_regs.operand1 & INT16_SGN;
			tmp = ((alu_regs.operand1 & UINT16_MAX) >> 1);
			tmp |= tmp2;
			break;

		/*Register - Register*/
		case ADD_REG :			// ADD REG
			int tmp;

			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op1 + op2;
			global_regs.general_regs[dst] = tmp;

			if (int16_t(tmp) == 0)
				setFlag(ZF);

			if(int16_t(tmp) < 0)
				setFlag(SF);

			if (uint16_t(tmp) > UINT16_MAX)
				setFlag(OF);

			break;

		case SUB_REG :			// SUB REG
			int tmp;
			
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;
			global_regs.general_regs[dst] = tmp;

			if(uint16_t(tmp) == 0)
				setFlag(ZF);
			
			if(int16_t(tmp) < 0)
				setFlag(SF);

			if(uint16_t(tmp) > UINT16_MAX)
				setFlag(OF);
			break;

		case CMP_REG : 			// CMP REG
			int tmp; 

			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;

			if (uint16_t(tmp) == 0)
				setFlag(ZF);

			if (int16_t(tmp) < 0)
				setFlag(SF);
			
			if (uint16_t(tmp) > UINT16_MAX)
				setFlag(OF);

			break;

		case MUL_REG : 			// MUL REG
			unsigned tmp;
			
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op1 * op2;
			global_regs.general_regs[dst] = tmp;

			if(int16_t(tmp) == 0)
				setFlag(ZF);

			if (uint16_t(tmp) > UINT16_MAX)
				setFlag(OF);				
			break;

		case IMUL_REG :			// IMUL REG
			unsigned tmp;

			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst]
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN); 

			op1 &= INT16_BITS;
			op2 &= INT16_BITS;

			tmp = op1 * op2;

			if (uint16_t(tmp) > INT16_BITS)
				setFlag(OF);

			if (sign)
				setFlag(SF);

			if(int16_t(tmp) == 0)
				setFlag(ZF);

			tmp |= sign;
			global_regs.general_regs[dst] = int16_t(tmp);

			break;

		case DIV_REG : 			// DIV REG
			unsigned tmp;

			op1 = global_regs.general_regs[alu_regs.operand1]
			op2 = global_regs.general_regs[dst];
			tmp = op2 / op1;
			global_regs.general_regs[dst] = tmp;

			if (uint16_t(tmp) == 0)
				setFlag(zF);
			break;

		case IDIV_REG :			// IDIV_REG	
			unsigned tmp;

			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN); 

			op1 &= INT16_BITS;
			op2 &= INT16_BITS;

			tmp = op2 / op1;

			if (uint16_t(tmp) > UINT16_MAX)
				setFlag(OF);

			if (sign)
				setFlag(SF);

			if(int16_t(tmp) == 0)
				setFlag(ZF)

			tmp |= sign;
			global_regs.general_regs[dst] = int16_t(tmp);

			break;
		
		case AND_REG : 			// AND REG
			uint16_t tmp;

			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst] 
			tmp = op1 & op2;
			global_regs.general_regs[dst] = tmp;
			sign = tmp & INT16_SGN;

			if(sign)
				setFlag(SF);

			if(tmp == 0)
				setFlag(ZF);

			break;
		
		case OR_REG : 			// OR REG
			unsigned tmp;
			op1 = global_regs.general_regs[alu_regs.operand1]
			op2 = global_regs.general_regs[dst];
			tmp = op1 | op2;
			global_regs.general_regs[dst] = tmp;
			sign = tmp & INT16_SGN;

			if (sign)
				setFlag(SF);

			if(tmp == 0)
				setFlag(ZF);

			break;
		
		case SHL_REG : 			// SHL REG
			unsigned tmp, c_out;

			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op2 << op1;
			sign = tmp & INT16_SGN;
			c_out = tmp & BIT_17TH;
			global_regs.general_regs[dst] = tmp;

			if (sign)
				setFlag(SF);

			if (int16_t(tmp) == 0)
				setFlag(ZF);

			if(c_out == BIT_17TH)
				setFlag(CF);

			break;
		
		case SAL_REG : 			// SAL REG
			unsigned tmp, c_out;

			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op2 << op1;
			sign = tmp & INT16_SGN;
			c_out = tmp & BIT_17TH;
			global_regs.general_regs[dst] = tmp;

			if (sign)
				setFlag(SF);

			if (int16_t(tmp) == 0)
				setFlag(ZF);

			if(c_out == BIT_17TH)
				setFlag(CF);
			break;
		
		case SHR_REG : 			// SHR REG
			uint16_t c_out;
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			
			while(op1) {
				c_out = op2 & 1;
				op2 >>= 1;
				op1--;
			}
			sign = op2 & INT16_SGN;
			global_regs.general_regs[dst] = tmp;

			if(sign)
				setFlag(SF);

			if(op2 == 0)
				setFlag(ZF);

			if(c_out)
				setFlag(CF);

			break;
		
		case SAR_REG : 			// SAR REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			sign = op1 & INT16_SGN;
			
			while (op1) {
				c_out = op2 & 1;
				op2 >>= 1;
				op2 |= sign;
				op1--;
			}

			if(sign)
				setFlag(SF);

			if(op2 == 0)
				setFlag(ZF);

			if (c_out)
				setFlag(CF);
			
			break;
	}	
}
