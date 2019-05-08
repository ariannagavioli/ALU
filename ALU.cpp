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

void Alu::setFlag(int flag){
	global_regs.flag |= (1 << flag);
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

				global_regs.flag &= !(1 << CF); // carry flag
				global_regs.flag &= !(1 << ZF);	// zero flag
				global_regs.flag &= !(1 << SF);	// sign flag
				global_regs.flag &= !(1 << OF);	// Overflow flag
				int tmp = 0;

				switch(alu_regs.opcode){
					case 0b01010010 :			// INC
						uint8 src = alu_regs.operand1;
						tmp = global_regs.general_regs[src];
						tmp++;
						global_regs.general_regs[src] = tmp;
						break;

					case 0b01010011 : 			// DEC
						uint8 src = alu_regs.operand1;
						tmp = global_regs.general_regs[src];
						tmp--;
						global_regs.general_regs[src] = tmp;
						break;

					case 0b01010100 :			// NEG: two complement
						uint8 src = alu_regs.operand1;
						tmp = global_regs.general_regs[src];
						tmp = 0 - tmp;
						global_regs.general_regs[src] = tmp;
						break;

					case 0b01010101 :			// NOT: negate bitwise
						uint8 src = alu_regs.operand1;
						tmp = global_regs.general_regs[src];
						tmp ~= tmp;
						global_regs.general_regs[src];
						break;

					case 0b01100001 :			//ADD
						uint8 dst = alu_regs.operand2;
						uint16 op1, op2;
						op1 = alu_regs.operand1;
						op2 = alu_regs.general_regs[dst];
						tmp = op1 + op2;
						alu_regs.general_regs[dst] = tmp;
						if(uint16(tmp) == 0)
							setFlag(ZF);
						if(int16(tmp < 0))
							setFlag(SF);
						if(uint(tmp) >= pow(2,16))
							setFlag(OF);
						break;

					case 0b01100010 :			//SUB
						uint8 dst = alu_regs.operand2;
						uint16 op1, op2;
						op1 = alu_regs.operand1;
						op2 = alu_regs.general_regs[dst];
						tmp = op2 - op1;
						alu_regs.general_regs[dst] = tmp;
						if(int16(tmp) == 0)
							setFlag(ZF);
						if(int16(tmp < 0))
							setFlag(SF);
						if(uint(tmp) >= pow(2,16))
							setFlag(OF);
						break;

					case 0b01100011 :			//CMP
						uint8 dst = alu_regs.operand2;
						uint16 op1, op2;
						op1 = alu_regs.operand1;
						op2 = alu_regs.general_regs[dst];
						tmp = op2 - op1;
						if(int16(tmp) == 0)
							setFlag(ZF);
						if(int16(tmp < 0))
							setFlag(SF);
						if(uint16(tmp) >= pow(2,16))
							setFlag(OF);
						break;


					case 0b01100100 :			//MUL
						uint8 dst = alu_regs.operand2;
						uint16 op1, op2;
						op1 = alu_regs.operand1;
						op2 = alu_regs.general_regs[dst];
						tmp = op1 * op2;
						alu_regs.general_regs[dst] = tmp;
						if(int16(tmp) == 0)
							setFlag(ZF);
						if(int16(tmp < 0))
							setFlag(SF);
						if(uint16(tmp) >= pow(2,16))
							setFlag(OF);
						break;

					case 0b01100101 :			//IMUL
						uint8 dst = alu_regs.operand2;
						uint16 op1, op2;
						op1 = ( alu_regs.operand1 & 0x7FFF );	// I am putting to zero the first bit
						op2 = ( alu_regs.general_regs[dst] & 0x7FFF);
						tmp = op1 * op2;

						if(uint16(tmp) >= pow(2,15))		// The 16th bit is for the sign!
							setFlag(OF);

						uint16 sign1 = op1 & 0x8000;			// I am now only taking the bit sign
						uint16 sign2 = op2 & 0x8000;
						uint16 sign = sign1 ^ sign2;			// Final sign is result of xor of the operands signs

						if((sign) != 0)				
							setFlag(SF);

						if(int16(tmp) == 0)
							setFlag(ZF);

						tmp = tmp | sign;
						break;

					case 0b01100110 :			//DIV
						uint8 dst = alu_regs.operand2;
						uint16 op1, op2;
						op1 = alu_regs.operand1;
						op2 = alu_regs.general_regs[dst];
						tmp = op2 / op1;
						alu_regs.general_regs[dst] = tmp;
						if(int16(tmp) == 0)
							setFlag(ZF);
						if(int16(tmp < 0))
							setFlag(SF);
						if(uint16(tmp) >= pow(2,16))
							setFlag(OF);
						break;

					case 0b01100111 :			//IDIV
						uint8 dst = alu_regs.operand2;
						uint16 op1, op2;
						op1 = ( alu_regs.operand1 & 0x7FFF );	// I am putting to zero the first bit
						op2 = ( alu_regs.general_regs[dst] & 0x7FFF);
						tmp = op2 / op1;

						if(uint16(tmp) >= pow(2,15))		// The 16th bit is for the sign!
							setFlag(OF);

						uint16 sign1 = op1 & 0x8000;			// I am now only taking the bit sign
						uint16 sign2 = op2 & 0x8000;
						uint16 sign = sign1 ^ sign2;			// Final sign is result of xor of the operands signs

						if((sign) != 0)				
							setFlag(SF);

						if(int16(tmp) == 0)
							setFlag(ZF);

						tmp = tmp | sign;		
						break;

					case 0b01101000 :			//AND
						uint8 dst = alu_regs.operand2;
						uint16 op1 = alu_regs.operand1;
						uint16 op2 = alu_regs.general_regs(dst);

						tmp = op1 & op2;

						sign = tmp & 0x8000;
						if(sign != 0)
							setFlag(SF);
						break;

					case 0b01101001 :			//OR
						uint8 dst = alu_regs.operand2;
						uint16 op1 = alu_regs.operand1;
						uint16 op2 = alu_regs.general_regs(dst);

						tmp = op1 | op2;

						sign = tmp & 0x8000;
						if(sign != 0)
							setFlag(SF);
						break;

					case 0b01101010 :			//SHL
						tmp = (alu_regs.operand1 << 1);
						break;
					case 0b01101011 :			//SAL
						tmp = alu_regs.operand1 << 1;
						break;
					case 0b01101100 :			//SHR
						tmp = alu_regs.operand1 >> 1;
						break;
					case 0b01101101 :			//SAR
						int tmp2 = alu_regs.operand1 & 0b1000000000000000;
						tmp = ((alu_regs.operand1 & 0b01111111111111111) >> 1);
						tmp |= tmp2;
						break;

					/*Register - Register*/
					case 0b10000001 :			// ADD
						tmp = alu_regs.operand1; 
						break;
					case 0b10000010 :			// SUB
						tmp = alu_regs.operand1;
						break;
					case 0b10000011 : 			// CMP

						break;
					case 0b10000100 : 			// MUL

						break;
					case 0b10000101 :			// IMUL

						break;
					case 0b10000110 : 			// DIV

						break;
					case 0b10000111 :			// IDIV

						break;
					case 0b10001000 : 			// AND

						break;
					case 0b10001001 : 			// OR

						break;
					case 0b10001010 : 			// SHL

						break;
					case 0b10001011 : 			// SAL

						break;
					case 0b10001100 : 			// SHR

						break;
					case 0b10001101 : 			// SAR

						break;
				}

				// Checking overflow
				if (abs(tmp) > pow(2, 16))
					global_regs.flag |= (1 << OF);
				
				// Checking sign
				if (tmp < 0)
					global_regs.flag |= (1 << SF);
				
				// Checking zero
				else if (tmp == 0)
					global_regs.flag |= (1 << ZF);

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