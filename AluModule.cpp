#include <cstring>
#include <iostream>
#include <bitset>

#include "AluModule.h"
#include "decode_registers.h"

using namespace std;

struct decode_registers alu_regs;
struct global_registers global_regs;

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
		
		
		// cout << "Message " << m->id << ": Sent at " << m->timestamp << ", current timestamp " << getTime() << ". My name is " << getName() << "." << endl;
		
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
				
				sendWithDelay(myMessage, delay);
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


void AluModule::setFlag(unsigned flag) {
	global_regs.flag |= (1 << flag);
}

void AluModule::operate() {

	sign = 0;
	c_outMSB = 0;
	c_outLSB = 0;
	global_regs.flag &= !(1 << CF); // carry flag
	global_regs.flag &= !(1 << ZF);	// zero flag
	global_regs.flag &= !(1 << SF);	// sign flag
	global_regs.flag &= !(1 << OF);	// Overflow flag
	dst = alu_regs.operand2;

	switch(alu_regs.opcode){
		case INC_OPC :			// INC
			/*According to the book, the flags after this operation aren't modify */
			src = alu_regs.operand1;
			global_regs.general_regs[src]++;
			tmp16s = global_regs.general_regs[src];

			if(tmp16s == 0)
				setFlag(ZF);
			if(tmp16s < 0)
				setFlag(SF);

			delay = INC_DELAY;
			break;

		case DEC_OPC : 			// DEC
			/*According to the book, the flags after this operation aren't modify */
			src = alu_regs.operand1;
			global_regs.general_regs[src]--;
			tmp16s = global_regs.general_regs[src];

			if(tmp16s == 0)
				setFlag(ZF);
			if(tmp16s < 0)
				setFlag(SF);
			delay = DEC_DELAY;
			break;

		case NEG_OPC :			// NEG: two complement
			/*Flags not specified on the book, not even operation, review operation */
			
			src = alu_regs.operand1;			
			tmp16 = global_regs.general_regs[src];

			if(tmp16 == INT16_SGN) {
				setFlag(OF);
				break;
			}

			if(tmp16 == 0)
				setFlag(CF);

			tmp16 = -tmp16;						
			global_regs.general_regs[src] = tmp16;
			tmp16s = tmp16;

			if(tmp16 == 0)
				setFlag(ZF);
			if(tmp16s < 0)
				setFlag(SF);

			delay = NEG_DELAY;
			break;

		case NOT_OPC :			// NOT: negate bitwise
			src = alu_regs.operand1;

			tmp16 = global_regs.general_regs[src];
			tmp16 = ~tmp16;
			global_regs.general_regs[src] = tmp16;
			delay = NOT_DELAY;
			break;

		case ADD_OPC :			//ADD
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op1 + op2;
			global_regs.general_regs[dst] = tmp;

			if(int16_t(tmp) == 0) 
				setFlag(ZF);

			if(int16_t(tmp < 0))
				setFlag(SF);

			if((op1 & INT16_SGN) == (op2 & INT16_SGN))		// operands have the same sing
				if(op1 & INT16_SGN != tmp & INT16_SGN)	// result has different sign
					setFlag(OF);

			if(unsigned(tmp) < op1 || unsigned(tmp) < op2)
				setFlag(CF);
			delay = ADD_DELAY;
			break;

		case SUB_OPC :			//SUB
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;
			global_regs.general_regs[dst] = tmp;

			if(int16_t(tmp) == 0)
				setFlag(ZF);

			if(int16_t(tmp < 0))
				setFlag(SF);

			if((op1 & INT16_SGN) != (op2 & INT16_SGN))		// operands have different sign
				if((tmp & INT16_SGN) != (op1 & INT16_SGN))	// result has different sign than op1
					setFlag(OF);

			if(unsigned(tmp) > op1)
				setFlag(CF);
			delay = SUB_DELAY;
			break;

		case CMP_OPC :			//CMP
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;

			if(int16_t(tmp) == 0)
				setFlag(ZF);

			if(int16_t(tmp < 0))
				setFlag(SF);

			if((op1 & INT16_SGN) != (op2 & INT16_SGN))		// operands have different sign
				if((tmp & INT16_SGN) != (op1 & INT16_SGN))	// result has different sign than op1
					setFlag(OF);

			if(uint16_t(tmp) > op1)
				setFlag(CF);
			delay = CMP_DELAY;
			break;

		case MUL_OPC :			// MUL
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			utmp = op1 * op2;
			global_regs.general_regs[dst] = utmp;
			
			if(utmp > UINT16_MAX){
				setFlag(OF);
				setFlag(CF);
			}
			delay = MUL_DELAY;
			break;

		case IMUL_OPC :			//IMUL
			op1 = alu_regs.operand1;						// Cast to unsigned 15 bits 
			op2 = global_regs.general_regs[dst];
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN);	// Final sign is result of xor of the operands signs
			op1 = abs(int16_t(op1));						// in absolute value, the product of two numbers are
			op2 = abs(int16_t(op2));						// the same
			
			utmp = op1 * op2;
			
			if(utmp > INT16_BITS){
				setFlag(OF);
				setFlag(CF);
			}

			if(sign)
				utmp = -utmp;

			global_regs.general_regs[dst] = int16_t(utmp);
			delay = IMUL_DELAY;
			break;

		case DIV_OPC :			//DIV
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			utmp = op2 / op1;
			global_regs.general_regs[dst] = utmp;
			delay = DIV_DELAY;
			break;

		case IDIV_OPC :			//IDIV
			op1 = alu_regs.operand1;						// I am putting to zero the first bit
			op2 = global_regs.general_regs[dst];
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN);	// Final sign is result of xor of the operands signs

			op1 = abs(int16_t(op1));
			op2 = abs(int16_t(op2));

			utmp = op2 / op1;

			if(sign)
				utmp = -utmp;

			global_regs.general_regs[dst] = int16_t(utmp);

			delay = IDIV_DELAY;
			break;

		case AND_OPC :			//AND
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp16 = op1 & op2;
			global_regs.general_regs[dst] = tmp16;
			sign = tmp16 & INT16_SGN;
			
			if(sign)
				setFlag(SF);

			if(tmp16 == 0)
				setFlag(ZF);
			delay = AND_DELAY;
			break;


		case OR_OPC :			//OR
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			tmp16 = op1 | op2;
			global_regs.general_regs[dst] = tmp16;
			sign = tmp16 & INT16_SGN;

			if(sign)
				setFlag(SF);

			if (tmp16 == 0) 
				setFlag(ZF);
			delay = OR_DELAY;
			break;

		case SHL_OPC :			//SHL
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			utmp = op2 << op1;
			sign = utmp & INT16_SGN;
			c_outMSB = utmp & BIT_17TH;
			global_regs.general_regs[dst] = utmp;

			if (sign)
				setFlag(SF);

			if (int16_t(utmp) == 0)
				setFlag(ZF);

			if (c_outMSB == BIT_17TH)
				setFlag(CF);
			delay = SHL_DELAY * op1;;
			break;

		case SAL_OPC :			//SAL
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			utmp = op2 << op1;
			sign = utmp & INT16_SGN;
			c_outMSB = utmp & BIT_17TH;
			global_regs.general_regs[dst] = uint16_t(utmp);

			if (sign)
				setFlag(SF);

			if (int16_t(utmp) == 0)
				setFlag(ZF);

			if (c_outMSB)
				setFlag(CF);
			delay = SAL_DELAY * op1;
			break;

		case SHR_OPC :			//SHR
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			sign = op2 & INT16_SGN;
			delay = SHR_DELAY * op1;
			
			while (op1) {
				c_outLSB = op2 & 1;
				op2 >>= 1;
				op1--;
			}
			
			sign = op2 & INT16_SGN;
			global_regs.general_regs[dst] = op2;

			if (sign) 
				setFlag(SF);

			if (op2 == 0)
				setFlag(ZF);

			if (c_outLSB)
				setFlag(CF);

			break;
		
		case SAR_OPC :			//SAR
			op1 = alu_regs.operand1;
			op2 = global_regs.general_regs[dst];
			delay = SHR_DELAY * op1;

			while(op1) {
				c_outLSB = op2 & 1;
				op2 >> 1;
				op2 |= sign;
				op1--;
			}
			sign = op2 & INT16_SGN;
			global_regs.general_regs[dst] = op2;
			
			if(sign)
				setFlag(SF);
			
			if(op2 == 0)
				setFlag(ZF);
			
			if(c_outLSB)
				setFlag(CF);

			delay = SAR_DELAY;
			break;

		/*Register - Register*/
		case ADD_REG_OPC :			// ADD REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op1 + op2;
			global_regs.general_regs[dst] = tmp;

			if (int16_t(tmp) == 0)
				setFlag(ZF);

			if(int16_t(tmp) < 0)
				setFlag(SF);

			if((op1 & INT16_SGN) == (op2 & INT16_SGN))
				if((op1 & INT16_SGN) != (tmp & INT16_SGN))
					setFlag(OF);
			
			if((unsigned(tmp) < op1) || (unsigned(tmp) < op2))
				setFlag(CF);				

			delay = ADD_DELAY;
			break;

		case SUB_REG_OPC :			// SUB REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;
			global_regs.general_regs[dst] = tmp;

			if(uint16_t(tmp) == 0)
				setFlag(ZF);
			
			if(int16_t(tmp) < 0)
				setFlag(SF);

			if((op1 & INT16_SGN) != (op2 & INT16_SGN))
				if((tmp & INT16_SGN) != (op1 & INT16_SGN))
					setFlag(OF);

			delay = SUB_DELAY;
			break;

		case CMP_REG_OPC : 			// CMP REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp = op2 - op1;
			
			if (uint16_t(tmp) == 0)
				setFlag(ZF);
			
			if (int16_t(tmp) < 0)
				setFlag(SF);

			if ((op1 & INT16_SGN) != (op2 & INT16_SGN))
				if((tmp & INT16_SGN) != (op1 & INT16_SGN))
					setFlag(OF);

			if(uint16_t(tmp) > op1)
				setFlag(CF);

			delay = CMP_DELAY;
			break;

		case MUL_REG_OPC : 			// MUL REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			utmp = op1 * op2;
			global_regs.general_regs[dst] = utmp;

			if (utmp > UINT16_MAX){
				setFlag(OF);		
				setFlag(CF);
			}

			delay = MUL_DELAY;
			break;

		case IMUL_REG_OPC :			// IMUL REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN); 

			op1 = abs(int16_t(op1));
			op2 = abs(int16_t(op2));

			utmp = op1 * op2;

			if (utmp > UINT16_MAX) {
				setFlag(OF);
				setFlag(CF);
			}

			if (sign)
				utmp = -utmp;

			global_regs.general_regs[dst] = int16_t(utmp);
			delay = IMUL_DELAY;
			break;

		case DIV_REG_OPC : 			// DIV REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			utmp = op2 / op1;
			global_regs.general_regs[dst] = utmp;
			delay = DIV_DELAY;
			break;

		case IDIV_REG_OPC :			// IDIV_REG	
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			sign = (op1 & INT16_SGN) ^ (op2 & INT16_SGN); 

			op1 = abs(int16_t(op1));
			op2 = abs(int16_t(op2));

			utmp = op2 / op1;

			if (sign)
				utmp = -utmp;

			global_regs.general_regs[dst] = int16_t(utmp);
			delay = IDIV_DELAY;
			break;
		
		case AND_REG_OPC : 			// AND REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst]; 
			tmp16 = op1 & op2;
			global_regs.general_regs[dst] = tmp16;
			sign = tmp16 & INT16_SGN;

			if(sign)
				setFlag(SF);

			if(tmp16 == 0)
				setFlag(ZF);
			delay = AND_DELAY;
			break;
		
		case OR_REG_OPC : 			// OR REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			tmp16 = op1 | op2;
			global_regs.general_regs[dst] = tmp16;
			sign = tmp16 & INT16_SGN;

			if (sign)
				setFlag(SF);

			if(tmp16 == 0)
				setFlag(ZF);
			delay = OR_DELAY;
			break;
		
		case SHL_REG_OPC : 			// SHL REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			utmp = op2 << op1;
			sign = utmp & INT16_SGN;
			c_outMSB = utmp & BIT_17TH;
			global_regs.general_regs[dst] = utmp;

			if (sign)
				setFlag(SF);

			if (int16_t(utmp) == 0)
				setFlag(ZF);

			if(c_outMSB)
				setFlag(CF);
			delay = SHL_DELAY * op1;
			break;
		
		case SAL_REG_OPC : 			// SAL REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			utmp = op2 << op1;
			sign = utmp & INT16_SGN;
			c_outMSB = utmp & BIT_17TH;
			global_regs.general_regs[dst] = utmp;

			if (sign)
				setFlag(SF);

			if (int16_t(utmp) == 0)
				setFlag(ZF);

			if(c_outMSB)
				setFlag(CF);
			delay = SAL_DELAY * op1;
			break;
		
		case SHR_REG_OPC : 			// SHR REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			delay = SHR_DELAY * op1;
			while(op1) {
				c_outLSB = op2 & 1;
				op2 >>= 1;
				op1--;
			}
			sign = op2 & INT16_SGN;
			global_regs.general_regs[dst] = op2;

			if(sign)
				setFlag(SF);

			if(op2 == 0)
				setFlag(ZF);

			if(c_outLSB)
				setFlag(CF);
			delay = SHR_DELAY;
			break;
		
		case SAR_REG_OPC : 			// SAR REG
			op1 = global_regs.general_regs[alu_regs.operand1];
			op2 = global_regs.general_regs[dst];
			sign = op2 & INT16_SGN;
			delay = SHR_DELAY * op1;
			
			while (op1) {
				c_outLSB = op2 & 1;
				op2 >>= 1;
				op2 |= sign;
				op1--;
			}

			sign = op2 & INT16_SGN;
			global_regs.general_regs[dst] = op2;
			
			if(sign)
				setFlag(SF);

			if(op2 == 0)
				setFlag(ZF);

			if (c_outLSB)
				setFlag(CF);
			delay = SAR_DELAY;
			break;
	}	
	cout << "Result DEC\t" << int16(global_regs.general_regs[dst]) << endl;
	cout << "Result U DEC\t" << uint16_t(global_regs.general_regs[dst]) << endl;
	cout << "Result BIN\t" << bitset<16>(global_regs.general_regs[dst]) << endl;
	cout << "Flags\t\t" << bitset<16>(global_regs.flag) << endl;
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
}
