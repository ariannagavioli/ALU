#include "ExampleModule.h"
#include "System.h"
#include "AluModule.h"

#include <iostream>
#include <bitset>
#include "decode_registers.h"
using namespace std;

int main() {
	System system;
	
	//Initialize the system here by adding modules to the system object
	// system.addModule(new ExampleModule("Alice"));
	// system.addModule(new ExampleModule("Bob", 10));//The second parameter is the priority. It's useful only when you need to intercept someone's else message
	uint8_t dst = 4;
	
	global_regs.general_regs[dst] = -4000;
	cout << "Initial binary " << bitset<16>(global_regs.general_regs[dst]) << endl;

	alu_regs.opcode = IMUL_OPC;
	alu_regs.operand1 = 2;

	global_regs.general_regs[alu_regs.operand1] = 5;
	
	alu_regs.operand2 = dst;
	alu_regs.destination_reg = dst;

	cout << "OPCODE: " << int(alu_regs.opcode) << 
	" / OPE1: " << alu_regs.operand1 <<
	" / OPE2: " << alu_regs.operand2 << 
	" / DST: " << int(alu_regs.destination_reg) << 
	" / GENR: " << global_regs.general_regs[dst] << endl; 

	system.addModule(new AluModule("Alu"));
	system.addModule(new ExampleModule("Fetch"));
	//Call run() to start the simulation
	system.run();
}
