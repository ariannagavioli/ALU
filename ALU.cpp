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
		
		//Send some test mess
		ages
		if(m->timestamp < 50) {
			if(getName() ==
			 Alu){

			myMessage->valid =
			 1;
			myMessage->timestamp = getTime();
				strcpy(myMessage->source, getName().c_str());
				strcpy(myMessage->dest, "Fetch");

				/*** OUR CODE ***/

				global_regs.flag &= !(1 << CF); // carry flag
				global_regs.flag &= !(1 << ZF);	// zero flag
				global_regs.flag &= !(1 << SF);	// sign flag
			
				global_regs.flag &= !(1 << OF);	// Overflow flag

				switch(alu_regs.opcode){
					case 0b01010010 :			// INC
						/*According to the book, the flags after this operation aren't modify */
						uint8_t src = alu_regs.operand1;
						global_regs.general_regs[src]++;
						break;

					case 0b01010011 : 			// DEC
						/*According to the book, the flags after this operation aren't modify */
						uint8_t src = alu_regs.operand1;
						global_regs.general_regs[src]--;
						break;

					case 0b01010100 :			// NEG: two complement
						/* Flags not specified on the book, not even operation, review operation */
						unsigned tmp;
						uint8_t src = alu_regs.operand1;

						tmp = global_regs.general_regs[src];
						tmp = 0 - tmp;
						global_regs.general_regs[src] = tmp;
						break;

					case 0b01010101 :			// NOT: negate bitwise
						unsigned tmp;
						uint8_t src = alu_regs.operand1;

						tmp = global_regs.general_regs[src];
						tmp ~= tmp;
						global_regs.general_regs[src] = tmp;
						break;

					case 0b01100001 :			//ADD
						int tmp;
						uint16_t op1, op2;
						uint8_t dst = alu_regs.operand2;

						op1 = alu_regs.operand1;
						op2 = global_regs.general_regs[dst];
						tmp = op1 + op2;
						global_regs.general_regs[dst] = tmp;

						if(int16_t(tmp) == 0) 
							setFlag(ZF);

						if(int16_t(tmp < 0))
							setFlag(SF);

						if(unsigned(tmp) >= pow(2,16))
							setFlag(OF);
						break;

					case 0b01100010 :			//SUB
						int tmp;
						uint16_t op1, op2;
						uint8_t dst = alu_regs.operand2;

						op1 = alu_regs.operand1;
						op2 = global_regs.general_regs[dst];
						tmp = op2 - op1;
						global_regs.general_regs[dst] = tmp;

						if(int16_t(tmp) == 0)
							setFlag(ZF);

						if(int16_t(tmp < 0))
							setFlag(SF);

						if(unsigned(tmp) >= pow(2,16))
							setFlag(OF);
						break;

					case 0b01100011 :			//CMP
						int tmp;
						uint8_t dst = alu_regs.operand2;
						uint16_t op1, op2;

						op1 = alu_regs.operand1;
						op2 = global_regs.general_regs[dst];
						tmp = op2 - op1;

						if(int16_t(tmp) == 0)
							setFlag(ZF);

						if(int16_t(tmp < 0))
							setFlag(SF);

						if(unsigned(tmp) >= pow(2, 16))
							setFlag(OF);
						break;

					case 0b01100100 :			// MUL
						unsigned tmp;
						uint16_t op1, op2;
						uint8_t dst = alu_regs.operand2;

						op1 = alu_regs.operand1;
						op2 = global_regs.general_regs[dst];
						tmp = op1 * op2;
						global_regs.general_regs[dst] = tmp;
						
						if(int16_t(tmp) == 0)		// According to x86 ISA, this is undefined 
							setFlag(ZF);
						
						if(unsigned(tmp) >= pow(2,16))
							setFlag(OF);
						break;

					case 0b01100101 :			//IMUL
						unsigned tmp;
						uint8_t dst = alu_regs.operand2;
						uint16_t op1, op2;

						op1 = alu_regs.operand1 & 0x7FFF;	// I am putting to zero the first bit
						op2 = global_regs.general_regs[dst] & 0x7FFF;
						tmp = op1 * op2;

						if(uint16(tmp) >= pow(2,15))			// The 16th bit is for the sign!
							setFlag(OF);

						uint16_t sign1 = op1 & 0x8000;			// I am now only taking the bit sign
						uint16_t sign2 = op2 & 0x8000;
						uint16_t sign = sign1 ^ sign2;			// Final sign is result of xor of the operands signs

						if(sign != 0)				
							setFlag(SF);

						if(int16_t(tmp) == 0)
							setFlag(ZF);

						tmp = tmp | sign;

						global_regs.general_regs[dst] = int16_t(tmp);

						break;

					case 0b01100110 :			//DIV
						unsigned tmp;
						uint16_t op1, op2;
						uint8_t dst = alu_regs.operand2;
						
						op1 = alu_regs.operand1;
						op2 = global_regs.general_regs[dst];
						tmp = op2 / op1;
						global_regs.general_regs[dst] = tmp;
						
						/* x86 ISA Ref: flags are undefined  */
						if(uint16_t(tmp) == 0)
							setFlag(ZF);

						// x86, this is undefined (every flag)
						// if(uint16(tmp) >= pow(2,16))
						// 	setFlag(OF);
						/************************************/

						break;

					case 0b01100111 :			//IDIV
						unsigned tmp;
						uint16_t op1, op2;
						uint8_t dst = alu_regs.operand2;

						op1 = alu_regs.operand1 & 0x7FFF;	// I am putting to zero the first bit
						op2 = global_regs.general_regs[dst] & 0x7FFF;
						tmp = op2 / op1;

						if(uint16(tmp) >= pow(2,15))		// The 16th bit is for the sign!
							setFlag(OF);

						uint16_t sign1 = op1 & 0x8000;			// I am now only taking the bit sign
						uint16_t sign2 = op2 & 0x8000;
						uint16_t sign = sign1 ^ sign2;			// Final sign is result of xor of the operands signs

						/* x86 ISA Ref: flags are undefined  */
						if(sign != 0)				
							setFlag(SF);

						if(int16(tmp) == 0)
							setFlag(ZF);
						/**************************************/

						tmp = tmp | sign;
						global_regs.general_regs[dst] = int16_t(tmp);

						break;

					case 0b01101000 :			//AND
						unsigned tmp;
						uint8_t dst = alu_regs.operand2;
						uint16_t op1 = alu_regs.operand1;
						uint16_t op2 = global_regs.general_regs[dst];

						tmp = op1 & op2;
						global_regs.general_regs[dst] = tmp;
						sign = tmp & 0x8000;
						
						if(sign != 0)
							setFlag(SF);

						if(int16_t(tmp) == 0)
							setFlag(ZF);
						break;

					case 0b01101001 :			//OR
						uint16_t tmp, sign;
						uint8_t dst = alu_regs.operand2;
						uint16_t op1 = alu_regs.operand1;
						uint16_t op2 = alu_regs.general_regs[dst];

						tmp = op1 | op2;
						global_regs.general_regs[dst] = tmp;
						sign = tmp & 0x8000;

						if(sign != 0)
							setFlag(SF);

						if (int16_t(tmp) == 0) 
							setFlag(ZF);
						break;

					case 0b01101010 :			//SHL
						unsigned tmp, c_out;
						uint16_t sign; 
						uint8_t dst = alu_regs.operand2;
						uint16_t op1 = alu_regs.operand1;
						uint16_t op2 = alu_regs.general_regs[dst];

						tmp = op2 << op1;
						sign = tmp & 0x8000;
						c_out = tmp & 0x10000;
						global_regs.general_regs[dst] = tmp;

						if (sign != 0)
							setFlag(SF);

						if (int16_t(tmp) == 0)
							setFlag(ZF);

						if (c_out == 0x10000)
							setFlag(CF);
						break;

					case 0b01101011 :			//SAL
						unsigned tmp, c_out;
						uint16_t sign; 
						uint8_t dst = alu_regs.operand2;
						uint16_t op1 = alu_regs.operand1;
						uint16_t op2 = alu_regs.general_regs[dst];

						tmp = op2 << op1;
						sign = tmp & 0x8000;
						c_out = tmp & 0x10000;
						global_regs.general_regs[dst] = uint16_t(tmp);

						if (sign != 0)
							setFlag(SF);

						if (int16_t(tmp) == 0)
							setFlag(ZF);

						if (c_out == 0x10000)
							setFlag(CF);
						break;

					case 0b01101100 :			//SHR
						uint16_t c_out, sign;
						uint8_t dst = alu_regs.operand2;
						uint16_t op1 = alu_regs.operand1;
						uint16_t op2 = global_regs.general_regs[dst];

						while (op1) {
							c_out = op2 & 0x1;
							op2 >>= 1;
							op1--;
						}
						
						sign = op2 & 0x8000;
						
						
						break;
					case 0b01101101 :			//SAR
						int tmp2 = alu_regs.operand1 & 0x8000;
						tmp = ((alu_regs.operand1 & 0xFFFF) >> 1);
						tmp |= tmp2;
						break;

					/*Register - Register*/
					case 0b10000001 :			// ADD REG
						tmp = global_regs.general_regs[alu_regs.operand1] + global_regs.general_regs[alu_regs.operand2];
						
						if (tmp > 0xFFFF)
							setFlag(OF);

						break;

					case 0b10000010 :			// SUB REG
						tmp = global_regs.general_regs[alu_regs.operand1] - global_regs.general_regs[alu_regs.operand2];
						
						if (tmp > 0xFFFF)
							setFlag(OF);

						break;

					case 0b10000011 : 			// CMP REG
						int tmpx = global_regs.general_regs[alu_regs.operand1] - global_regs.general_regs[alu_regs.operand2];
						
						if (abs(tmp) > pow(2, 16))
							setFlag(OF);

						break;

					case 0b10000100 : 			// MUL REG
						tmp = global_regs.general_regs[alu_regs.operand1] * global_regs.general_regs[alu_regs.operand2];
						if (tmp > 0xFFFF)
							setFlag(OF);
							
						break;

					case 0b10000101 :			// IMUL REG
						unsigned tmp2 = global_regs.general_regs[alu_regs.operand1] & 0x7FFF;
						unsigned tmp3 = global_regs.general_regs[alu_regs.operand2] & 0x7FFF;
						unsigned sign = (global_regs.general_regs[alu_regs.operand1] & 0x8000) ^ (global_regs.general_regs[alu_regs.operand1] & 0x8000); 

						tmp = tmp2 * tmp3;

						if (tmp > 0xFFFF)
							setFlag(OF);

						tmp |= sign;

						if (sign == 0x8000)
							setFlag(SF);

						break;

					case 0b10000110 : 			// DIV REG
						tmp = global_regs.general_regs[alu_regs.operand1] / global_regs.general_regs[alu_regs.operand2];
						if (tmp > 0xFFFF)
							setFlag(OF);

						break;
					case 0b10000111 :			
						unsigned tmp2 = global_regs.general_regs[alu_regs.operand1] & 0x7FFF;
						unsigned tmp3 = global_regs.general_regs[alu_regs.operand2] & 0x7FFF;
						unsigned sign = (global_regs.general_regs[alu_regs.operand1] & 0x8000) ^ (global_regs.general_regs[alu_regs.operand1] & 0x8000); 

						tmp = tmp2 / tmp3;

						if (tmp > 0xFFFF)
							setFlag(OF);

						tmp |= sign;
						if (sign == 0x8000)
							setFlag(SF);

						break;
					case 0b10001000 : 			// AND REG
						tmp = global_regs.general_regs[alu_regs.operand1] & global_regs.general_regs[alu_regs.operand2];
						break;
					case 0b10001001 : 			// OR REG
						tmp = global_regs.general_regs[alu_regs.operand1] | global_regs.general_regs[alu_regs.operand2];
						break;
					case 0b10001010 : 			// SHL REG

						tmp = global_regs.general_regs[alu_regs.operand1] << global_regs.general_regs[alu_regs.operand2];
						if (tmp > 0xFFFF)
							global_regs.flag |= (1 << OF);

						break;
					case 0b10001011 : 			// SAL REG
						tmp = global_regs.general_regs[alu_regs.operand1] << global_regs.general_regs[alu_regs.operand2];
						if (tmp > 0xFFFF)
							global_regs.flag |= (1 << OF);
						break;
					case 0b10001100 : 			// SHR REG
						// alu_regs.operand1 &= 0xFF; 
						tmp = global_regs.general_regs[alu_regs.operand1] >> global_regs.general_regs[alu_regs.operand2];
						
						if (tmp == 0)
							global_regs.flag |= (1 << ZF);

						break;
					case 0b10001101 : 			// SAR REG
						unsigned sign = global_regs.general_regs[alu_regs.operand1] & 0x8000;
						uint16_t tmp2 = global_regs.general_regs[alu_regs.operand2];
						tmp = global_regs.general_regs[alu_regs.operand1];
						
						while (tmp2) {
							tmp >>= 1;
							tmp |= sign;
							--tmp2;
						}
								
						if (tmp > 0xFFFF)
							global_regs.flag |= (1 << OF);		

						if (tmp == 0)
							global_regs.flag |= (1 << ZF);

						break;
				}

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