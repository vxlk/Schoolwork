/*
Tyler Small
CPSC 3300
Smotherman
Fall 2018

COMPILE:
g++ -std=c++14 mips.cpp
*/


///TODO:
//bne not working

//PC STILL ISNT COUNTING CORRECTLY...
//increment pc BEFORE calling pipeline code

//-------in3-----------------
//figure out his numbering scheme X
//you recorded an extra alu op 
//----------------------------


//-------in4-----------------
//endless loop in X
//loop is in jr
//--------------------------

//-------in5----------------
//registers untaken, but printing wrong
//short one alu op???
//--------------------------

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>
#include <type_traits>

//https://en.wikibooks.org/wiki/MIPS_Assembly/Instruction_Formats

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define SIZE_OF_MEM 20000/*16384*/
#define RETURN_ADDR 31
#define NUM_INSTRUCTIONS 32
#define NUM_REGISTERS 32
#define NOT_IN_USE -1
#define REGISTERS_IN_AN_OP 3

/*
Decide Which Outputs
*/
bool output1 = false;
bool output2 = false;
bool output3 = true;

enum type
{
	Control,
	LoadStore,
	Multiply,
	ALUops,
	NONE
};

struct ICstats {
	int ops;
	int loads;
	int jumps;
};

struct memAccess {
	int fetches;
	int loads;
	int stores;
};

struct xferControl {
	int jumps;
	int jumpsLinks;
	int takenBranches;
	int untakenBranches;
};

struct iPair {
	int issueCycles = 0;
	int doubleIssues = 0;
	int controlStops = 0;
	int structuralStops = 0;
	int dataDepStops = 0;
};

unsigned int hits = 0;
unsigned int misses = 0;
unsigned int writebacks = 0;

int memory[SIZE_OF_MEM];
std::vector<int> instructionList;
int branch = 0;
bool halt = false;
int usedWordNumber = 0;
int wordNumber = 0;
int rNumber = 0;
int word[NUM_REGISTERS];
int r[NUM_REGISTERS];
int pc = 0;
iPair ipair;
ICstats ic;
memAccess mem;
xferControl xfer;
int numInstructions = 0;
int wouldAlso = 0;

#define LINES_PER_BANK 32
unsigned int plru_state[LINES_PER_BANK];  /* current state for each set    */
unsigned int valid[4][LINES_PER_BANK];    /* valid bit for each line       */
unsigned int tag[4][LINES_PER_BANK];      /* tag bits for each line        */
							 /* line contents are not tracked */
unsigned int plru_bank[8] /* table for bank replacement choice based on state */ = { 0, 0, 1, 1, 2, 3, 2, 3 };
unsigned int next_state[32] /* table for next state based on state and bank ref */
				   /* index by 5-bit (4*state)+bank [=(state<<2)|bank] */
						/*  bank ref  */
					   /* 0  1  2  3 */
	/*         0 */ =	{ 6, 4, 1, 0,
	/*         1 */       7, 5, 1, 0,
	/*         2 */       6, 4, 3, 2,
	/* current 3 */       7, 5, 3, 2,
	/*  state  4 */       6, 4, 1, 0,
	/*         5 */       7, 5, 1, 0,
	/*         6 */       6, 4, 3, 2,
	/*         7 */       7, 5, 3, 2 };
bool dirty[4][LINES_PER_BANK];
void cache_init(void) {
	int i;
	for (i = 0; i<LINES_PER_BANK; i++) {
		plru_state[i] = 0;
		valid[0][i] = tag[0][i] = 0;
		valid[1][i] = tag[1][i] = 0;
		valid[2][i] = tag[2][i] = 0;
		valid[3][i] = tag[3][i] = 0;
		dirty[0][i] = false;
		dirty[1][i] = false;
		dirty[2][i] = false;
		dirty[3][i] = false;
	}
}

void checkCache(unsigned int address, bool isWrite)
{
	address = address * 4;
	/*
	 are all 4 lines valid?
 *                             /       \
 *                           yes        no, use an invalid line
 *                            |
 *                       bit 0 == 0?
 *                        /       \
 *                       y         n
 *                      /           \
 *               bit 1 == 0?    bit 2 == 0?
 *                 /    \          /    \
 *                y      n        y      n
 *               /        \      /        \
 *   replace:  line 0  line 1  line 2  line 3
 *
	*/

	/*
	//get line
	int lineToUse = -1;
	for (int i = 0; i < 4; ++i)
		if (!valid[i])
		{
			lineToUse = i;
			break;
		}

	if (lineToUse == -1)
	{
		if (tag[0] == 0)
		{
			if (tag[1] == 0)
				lineToUse = 0;
			else lineToUse = 1;
		}

		else
		{
			if (tag[2] == 0)
				lineToUse = 2;
			else lineToUse = 3;
		}
	}
	*/

	unsigned int

		addr_tag,    /* tag bits of address     */
		addr_index,  /* index bits of address   */
		bank;        /* bank that hit, or bank chosen for replacement */

	addr_index = (address >> 5) & 0x1f;
	addr_tag = address >> 10;

	//if (!isWrite && dirty[addr_index]) writebacks++;

	//any write is dirty
	//if (isWrite) dirty[addr_index] = true;

	/* check bank 0 hit */

	if (valid[0][addr_index] && (addr_tag == tag[0][addr_index])) {
		hits++;
		bank = 0;
		//if (isWrite && dirty[addr_index]) writebacks++;
		/* check bank 1 hit */

	}
	else if (valid[1][addr_index] && (addr_tag == tag[1][addr_index])) {
		hits++;
		bank = 1;
		//if (isWrite && dirty[addr_index]) writebacks++;
		/* check bank 2 hit */

	}
	else if (valid[2][addr_index] && (addr_tag == tag[2][addr_index])) {
		hits++;
		bank = 2;
		//if (isWrite && dirty[addr_index]) writebacks++;
		/* check bank 3 hit */

	}
	else if (valid[3][addr_index] && (addr_tag == tag[3][addr_index])) {
		hits++;
		bank = 3;
		//if (!isWrite && dirty[addr_index]) writebacks++;
		/* miss - choose replacement bank */

	}

	else {
		misses++;

		//else if (isWrite && dirty[addr_index]) writebacks++;

		if (!valid[0][addr_index]) bank = 0;
		else if (!valid[1][addr_index]) bank = 1;
		else if (!valid[2][addr_index]) bank = 2;
		else if (!valid[3][addr_index]) bank = 3;
		else bank = plru_bank[plru_state[addr_index]];

		if (isWrite && dirty[bank][addr_index]) writebacks++;
		//clear dirty bit
		if (!isWrite)
		{
			if (dirty[bank][addr_index]) writebacks++;
			dirty[bank][addr_index] = false;

		}
		valid[bank][addr_index] = 1;
		tag[bank][addr_index] = addr_tag;
	}

	/* update replacement state for this set (i.e., index value) */

	plru_state[addr_index] = next_state[(plru_state[addr_index] << 2) | bank];
	//any write is dirty
	if (isWrite) dirty[bank][addr_index] = true;
}

struct pipelineContents
{
	pipelineContents() : intr(NONE), rs(NOT_IN_USE), rt(NOT_IN_USE), rd(NOT_IN_USE), name("") {}
	pipelineContents(std::string n, type i, int _rs, int _rt, int _rd, int _pc_)
	{
		this->name = n;
		this->intr = i;
		this->rs = _rs;
		this->rd = _rd;
		this->rt = _rt;
		this->_pc = _pc_;

	}
	//ghetto copy constructor, more obvious what it is doing with this name
	void replace(pipelineContents toBeLoaded)
	{
		this->name = toBeLoaded.name;
		this->intr = toBeLoaded.intr;
		this->rd = toBeLoaded.rd;
		this->rs = toBeLoaded.rs;
		this->rt = toBeLoaded.rt;
		this->_pc = toBeLoaded._pc;
	}
	void clear()
	{
		this->name = "";
		this->intr = NONE;
		this->rd = NOT_IN_USE;
		this->rs = NOT_IN_USE;
		this->rt = NOT_IN_USE;
		this->_pc = 0;
	}
	std::string name;
	type intr;
	int rs;
	int rt;
	int rd;
	int _pc;
};

class Pipeline
{
public:
	void loadIntoPipeline(pipelineContents toBeLoaded)
	{
		if (content1.intr != NONE)  loadIntoSecond(toBeLoaded);
		else content1.replace(toBeLoaded);
	}

private:
	//agner.org optimizing in c++
	void printPipeline(std::string action)
	{
		ipair.issueCycles++;
		//print pc
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::hex << std::setfill('0') << std::setw(3) << content1._pc << ": ";

		std::cout << content1.name << " ";
		if (content2.intr != NONE)
			std::cout << std::hex << std::setfill('0') << std::setw(3) << content1._pc + 1 << ": " << content2.name;
		std::cout << "        " + action + "\n";

		//clear both after printing
		content1.clear();
		content2.clear();
	}

	bool dataDep(pipelineContents toBeLoaded)
	{
		//god i should have made these arrays to start with..

		int d1[REGISTERS_IN_AN_OP];
		int d2[REGISTERS_IN_AN_OP];

		d1[0] = content1.rd;
		d1[1] = content1.rt;
		d1[2] = content1.rs;

		d2[0] = toBeLoaded.rd;    ///dont do this in a loop, just compare
		d2[1] = toBeLoaded.rt;
		d2[2] = toBeLoaded.rs;

		//make it ok for rts to be the same
		//check all the registers, return true if any of them are equal
		for (int i = 0; i < REGISTERS_IN_AN_OP; ++i)
		{
			if (d1[i] == NOT_IN_USE) continue;
			for (int j = 0; j < REGISTERS_IN_AN_OP; ++j)
			{
				if (content1.name == "sw") continue; //skip the target registers, they dont matter
				if (d2[j] == NOT_IN_USE) continue;
				if (content1.name == "lw" && d1[1] == d2[j]) return true;
				else if (d1[0] == d2[j]) return true;
			}
		}
		return false;

	}
	void loadIntoSecond(pipelineContents toBeLoaded)
	{
		//come here to try and load the second instruction, or throw a hazard and replace the first content

		//check instruction, check registers
		/*

		Control hazard: A control instruction is picked for issue slot 1.
		Structural hazard:
		A load/store instruction would be placed in issue slot 2, but there is already a load/store instruction in issue slot 1.
		A multiply instruction would be placed in issue slot 2, but there is already a multiply instruction in issue slot 1.
		Data hazard: The instruction that would be placed in issue slot 2 has a RAW or WAW register data dependency with the instruction that is already in issue slot 1.

		*/

		//control
		if (content1.intr == Control)
		{
			ipair.controlStops++;
			std::string action = "// control stop";
			if (dataDep(toBeLoaded)) action += "(also data dep.)";
			printPipeline(action);
			this->loadIntoPipeline(toBeLoaded);
		}

		//structural
		else if (content1.intr == LoadStore && toBeLoaded.intr == LoadStore
			|| content1.intr == Multiply && toBeLoaded.intr == Multiply)
		{
			ipair.structuralStops++;
			std::string action = "// structural stop";
			if (dataDep(toBeLoaded))
			{
				action += "(also data dep.)";
				wouldAlso++;
			}
			printPipeline(action);
			this->loadIntoPipeline(toBeLoaded);
		}

		//data hazard
		else if (dataDep(toBeLoaded))
		{
			ipair.dataDepStops++;
			printPipeline("// data dependency stop");
			this->loadIntoPipeline(toBeLoaded);
		}

		//double issue
		//its okay to load into 2nd space
		else
		{
			ipair.doubleIssues++;
			//ipair.issueCycles--;
			content2.replace(toBeLoaded);
			printPipeline("// --double issue--");
		}

	}

	pipelineContents content1;
	pipelineContents content2;
};

Pipeline pipeline;

void instructionPair(int pc_, std::string instrName, type itype, int rs = NOT_IN_USE, int rt = NOT_IN_USE, int rd = NOT_IN_USE)
{

	//ipair.issueCycles++;
	//make a new request type
	pipelineContents newData(instrName, itype, rs, rt, rd, pc_);
	//throw it into the pipe
	pipeline.loadIntoPipeline(newData);
}

//i
void addiu(const unsigned int passedValue) {
	ic.ops++;
	int maskimm = 65535;
	uint16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;


	r[rt] = r[rs] + imm;

	if (output1) {
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "addiu	- register r[" << rt << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rt] << "\n";
	}
	if (output2) {
		instructionPair(pc, "addiu", ALUops, rs, rt);
	}
	pc++;
}

void sw(const unsigned int passedValue) {
	mem.stores++;
	ic.loads++;

	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	if (output3)
	{
		memory[r[rs] + imm] = r[rt];
		checkCache(r[rs] + imm, true);
	}
	if (output1) {
		instructionList[r[rs] + imm] = r[rt];
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		std::cout << "sw	- register r[" << rt << "] value stored in memory\n";
	}
	if (output2) {
		instructionList[r[rs] + imm] = r[rt];
		//std::cout << "sw";
		instructionPair(pc, "sw", LoadStore, rs, rt);
	}
	pc++;
}

void beq(const unsigned int passedValue) {
	//test if taken
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	if (output1) {
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		pc++;
		ic.jumps++;

		//if (output1) {
		std::cout << "beq	- ";
		if (r[rs] == r[rt]) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
			std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << imm << "\n";
		}
		else {
			xfer.untakenBranches++;
			std::cout << std::setw(8) << "branch untaken\n";
		}
	}

	if (output2) {
		instructionPair(pc, "beq", Control, rs, rt);
		pc++;
		ic.jumps++;

		if (r[rs] == r[rt]) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
		}
		//instructionPair(pc, "beq", Control, rs, rt);
	}

	if (output3)
	{
		pc++;
		ic.jumps++;

		if (r[rs] == r[rt]) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
		}
	}
}

void bgtz(const unsigned int passedValue) {
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	if (output1) {
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		pc++;
		ic.jumps++;

		//if (output1) {
		std::cout << "bgtz	- ";
		if (r[rs] > 0) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
			std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << pc << "\n";
		}
		else {
			xfer.untakenBranches++;
			std::cout << std::setw(8) << "branch untaken\n";
		}
	}

	if (output2) {
		instructionPair(pc, "bgtz", Control, rs, rt);
		pc++;
		ic.jumps++;

		if (r[rs] > 0) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
		}
	}

	if (output3)
	{
		pc++;
		ic.jumps++;

		if (r[rs] > 0) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
		}
	}
}
//i
void blez(const unsigned int passedValue) {
	//test if taken
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	if (output1) {
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		pc++;
		ic.jumps++;

		//if (output1) {
		std::cout << "blez	- ";
		if (r[rs] <= 0) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
			std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << pc << "\n";
		}
		else {
			xfer.untakenBranches++;
			std::cout << std::setw(8) << "branch untaken\n";
		}
	}

	if (output2) {
		//instructionPair(pc, "blez", Control, rs, rt);
		pc++;
		instructionPair(pc, "blez", Control, rs, rt);
		ic.jumps++;
		if (r[rs] <= 0) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
			//std::cout << std::setw(8) << "branch untaken\n";
		}

	}

	if (output3)
	{
		pc++;
		ic.jumps++;

		if (r[rs] <= 0) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
		}
	}
}

void bne(const unsigned int passedValue) {
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	if (output1) {
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		pc++;
		ic.jumps++;
		//std::cout << "bne	- ";


		std::cout << "bne	- ";
		if (r[rs] != r[rt]) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
			std::cout << std::setw(8) << "branch taken to " << std::setfill('0') << std::setw(8) << imm << "\n";
		}
		else {
			xfer.untakenBranches++;
			std::cout << std::setw(8) << "branch untaken\n";
		}
	}

	if (output2) {
		instructionPair(pc, "bne", Control, rs, rt);
		pc++;
		ic.jumps++;
		if (r[rs] != r[rt]) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
		}
		//instructionPair(pc, "bne", Control, rs, rt);
	}

	if (output3)
	{
		pc++;
		ic.jumps++;
		
		if (r[rs] != r[rt]) {
			pc += imm;
			branch = pc - 1;
			xfer.takenBranches++;
		}
		else {
			xfer.untakenBranches++;
		}
	}
}
//j
void j(const unsigned int passedValue) {
	xfer.jumps++;
	ic.jumps++;
	int jumpVal = passedValue;
	//get rid of opcode
	jumpVal = jumpVal << 6;
	jumpVal = jumpVal >> 6;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";


		std::cout << std::setw(8) << "j	- jump to " << std::setfill('0') << std::hex << std::setw(8) << jumpVal << "\n";
	}

	if (output2) {
		//	std::cout << "j";
		instructionPair(pc, "j", Control);
	}
	pc = jumpVal;
	branch = jumpVal - 1; //was -1
}

void jal(const unsigned int passedValue) {
	xfer.jumpsLinks++;
	//xfer.jumps++;
	ic.jumps++;

	int jumpVal = passedValue;
	//get rid of opcode
	jumpVal = jumpVal << 6;
	jumpVal = jumpVal >> 6;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";


		std::cout << std::setw(8) << "jal	- jump to " << std::setfill('0') << std::hex << std::setw(8) << jumpVal << "\n";
	}
	if (output2) {
		//std::cout << "jal";
		instructionPair(pc, "jal", Control);
	}
	//pc = jumpVal;
	//r[RETURN_ADDR] = pc; //was instructionList[branch]
	pc++;
	branch = jumpVal - 1; //was 1
	r[RETURN_ADDR] = pc; //was instructionList[branch]
}

void lui(const unsigned int passedValue) {
	ic.ops++;
	int maskimm = 65535;
	int imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	//r[rt]<-immed << 16

	r[rt] = imm << 16;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "lui	- register r[" << rt << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rt] << "\n";
	}
	if (output2) {
		instructionPair(pc, "lui", ALUops, rs, rt);
	}
	pc++;
}

void lw(const unsigned int passedValue) {
	ic.loads++;
	mem.loads++;
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	if (output1 || output2)
		r[rt] = instructionList[r[rs] + imm];

	else
	{
		r[rt] = memory[r[rs] + imm];
		checkCache(r[rs] + imm, false);
	}

	//pc++;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "lw	- register r[" << rt << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rt] << "\n";
	}
	if (output2) {
		//std::cout << "lw";
		instructionPair(pc, "lw", LoadStore, rs, rt);
	}
	if (!rt)
	{
		std::cout << "***** - register r[0] not allowed to change; reset to 0\n";
		r[rt] = 0;
	};
	pc++;
}

void mul(const unsigned int passedValue) {
	ic.ops++;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] * r[rt];

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "mul	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//std::cout << "mul";
		instructionPair(pc, "mul", Multiply, rs, rt, rd);
	}
	pc++;
}

void slti(const unsigned int passedValue) {
	ic.ops++;
	int maskimm = 65535;
	int16_t imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;

	r[rt] = (signed(r[rs]) < (imm)) ? 1 : 0;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		std::cout << "slti	- register r[" << rt << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rt] << "\n";
	}
	if (output2) {
		//std::cout << "slti";
		instructionPair(pc, "slti", ALUops, rs, rt);
	}
	pc++;
}

void xori(const unsigned int passedValue) {
	ic.ops++;
	int maskimm = 65535;
	int imm = passedValue & maskimm;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;


	r[rt] = r[rs] ^ imm; //zero_ext means unsigned?

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";


		std::cout << "xori	- register r[" << rt << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rt] << "\n";
	}
	if (output2) {
		//std::cout << "xor";
		instructionPair(pc, "xori", ALUops, rs, rt);
	}
	pc++;
}

/* ALU FUNCTIONS */
void addu(const unsigned int passedValue) {

	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] + r[rt];

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "addu	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//std::cout << "addu";
		instructionPair(pc, "addu", ALUops, rs, rt, rd);
	}
	pc++;

}

void and_(const unsigned int passedValue) {
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] & r[rt];

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "and	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//	std::cout << "and";
		instructionPair(pc, "and", ALUops, rs, rt, rd);
	}
	pc++;
}

void jalr(const unsigned int passedValue) {
	xfer.jumpsLinks++;
	//xfer.jumps++;
	ic.jumps++;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		//r[rd]<-updated_pc; pc<-r[rs]
		std::cout << std::setw(8) << "jalr	- jump to " << std::setfill('0') << std::hex << std::setw(8) << r[rs] << "\n";
	}
	if (output2) {
		//	std::cout << "jalr";
		instructionPair(pc, "jalr", Control, rs, rt, rd);
	}
	pc++;
	//r[rd] = r[rs];
	//pc = r[rs];
	//r[RETURN_ADDR] = r[rs];
	r[rd] = pc;
	pc = r[rs];
	branch = pc - 1;
	//r[rd] = pc++;
}

void jr(const unsigned int passedValue) {
	//xfer.jumpsLinks++;
	xfer.jumps++;
	ic.jumps++;
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";

		//r[rd]<-updated_pc; pc<-r[rs]
		std::cout << std::setw(8) << "jr	- jump to " << std::setfill('0') << std::hex << std::setw(8) << r[rs] << "\n";
	}
	if (output2) {
		//std::cout << "jr";
		instructionPair(pc, "jr", ALUops, rs, rt, rd);
	}
	//Spc++;
	//r[rd] = pc++;
	//pc++;
	pc = r[rs];
	branch = pc - 1;

}

void nor(const unsigned int passedValue) {
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = ~(r[rs] | r[rt]);

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "nor	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//std::cout << "nor";
		instructionPair(pc, "nor", ALUops, rs, rt, rd);
	}
	pc++;
}

void or_(const unsigned int passedValue) {
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] | r[rt];

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "or	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}

	if (output2) {
		//std::cout << "or";
		instructionPair(pc, "or", ALUops, rs, rt, rd);
	}
	pc++;
}

//rshamt
void sll(const unsigned int passedValue) {
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;
	int shamt = passedValue >> 6;
	shamt = shamt & maskr;

	r[rd] = r[rt] << shamt;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "sll	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//std::cout << "sll";
		instructionPair(pc, "sll", ALUops, rs, rt, rd);
	}
	pc++;
}

void sra(const unsigned int passedValue) {
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;
	int shamt = passedValue >> 6;
	shamt = shamt & maskr;

	r[rd] = r[rt] >> shamt; //(sign bit duplicated) ... pls dont let me down gcc

							//safe check
	if (r[rt] < 0 && r[rd] > 0) r[rd] = (~(r[rd])) + 1;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "sra	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//std::cout << "sra";
		instructionPair(pc, "sra", ALUops, rs, rt, rd);
	}
	pc++;

}

void srl(const unsigned int passedValue) {
	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;
	int shamt = passedValue >> 6;
	shamt = shamt & maskr;

	r[rd] = r[rt] >> shamt; //with zero fill //(sign bit duplicated)

							//safe check .. if it preserved the negative, flip it and make it positive
	if (r[rt] < 0 && r[rd] < 0) r[rd] = (~(r[rd])) + 1;

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "srl	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//std::cout << "srl";
		instructionPair(pc, "srl", ALUops, rs, rt, rd);
	}
	pc++;

}

//r
void subu(const unsigned int passedValue) {

	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] - r[rt];

	if (output1) {
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "subu - register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		//std::cout << "subu";
		instructionPair(pc, "subu", ALUops, rs, rt, rd);
	}
	pc++;
}

void xor_(const unsigned int passedValue) {

	int maskr = 31;
	int rs = passedValue >> 21;
	rs = rs & maskr;
	int rt = passedValue >> 16;
	rt = rt & maskr;
	int rd = passedValue >> 11;
	rd = rd & maskr;

	r[rd] = r[rs] ^ r[rt];

	if (output1)
	{
		std::cout << std::setfill('0')
			<< std::dec
			<< std::showbase
			<< std::internal;
		std::cout << std::setfill('0') << std::setw(3) << pc << ": ";
		std::cout << "xor	- register r[" << rd << "] now contains " << std::hex << std::setfill('0') << std::setw(8) << r[rd] << "\n";
	}
	if (output2) {
		instructionPair(pc, "xor", ALUops, rs, rt, rd);
	}
	pc++;
}

/****************************************************/

void ALUfunction(const unsigned int passedValue) {
	//decide if op or constant
	//deal with constants later
	int test = passedValue;
	test = test >> 6;
	if (test == 0) {
		word[wordNumber] = passedValue;
		wordNumber++;
		return;
	}

	unsigned int mask = 63;
	unsigned int funct = passedValue & mask;
	if (funct != 0x09 && funct != 0x08)
		ic.ops++;
	switch (funct) {
	case 0x21: addu(passedValue); break;
	case 0x24: and_(passedValue); break;
	case 0x09: jalr(passedValue); break;
	case 0x08: jr(passedValue); break;
	case 0x27: nor(passedValue); break;
	case 0x25: or_(passedValue); break;
	case 0x00: sll(passedValue); break;
	case 0x03: sra(passedValue); break;
	case 0x02: srl(passedValue); break;
	case 0x23: subu(passedValue); break;
	case 0x26: xor_(passedValue); break;
	default: break;
	}

}

void hlt() {
	if (output1)
		std::cout << std::setfill('0') << std::setw(3) << pc << ": hlt\n";
	if (output2)
		instructionPair(pc, "hlt", Control);
	halt = true;
}

void functionCall(const unsigned int passedValue) {
	//figure out what kind of function it is
	//get the function opcode
	//send the int to the function's function
	if (passedValue == 0) {
		hlt();
		return;
	}
	unsigned int mask = 63;
	mask = mask << 26;

	unsigned int opcode = passedValue & mask;
	opcode = opcode >> 26;
	mem.fetches++;
	switch (opcode) {
	case 0: ALUfunction(passedValue); break;
	case 0x09: addiu(passedValue); break;
	case 0x04: beq(passedValue); break;
	case 0x07: bgtz(passedValue); break;
	case 0x06: blez(passedValue); break;
	case 0x05: bne(passedValue); break;
	case 0x02: j(passedValue); break;
	case 0x03: jal(passedValue); break;
	case 0x0f: lui(passedValue); break;
	case 0x23: lw(passedValue); break;
	case 0x1c: mul(passedValue); break;
	case 0x0a: slti(passedValue); break;
	case 0x2b: sw(passedValue); break;
	case 0x0e: xori(passedValue); break;
	default: break;
	}
}

int main(void) {

	std::string inputCommand;
	int i = 0;
	unsigned int input;

#ifdef _MSC_VER
	//FILE* inputFile;
	std::ifstream inputFile;
	//inputFile = fopen("input.txt", "r");
	inputFile.open("in6.txt");
	//while (fscanf(inputFile, "%s", &inputCommand) != EOF)
	while (inputFile >> std::hex >> input) {
		std::string::size_type sz;
		instructionList.push_back(input);

	}

#else
	//school computers
	while (std::cin >> std::hex >> input)
		instructionList.push_back(input);
#endif

	if (output1)
	{
		//before
		std::cout << "contents of memory\n";
		std::cout << "addr value\n";
		for (int i = 0; i < instructionList.size(); ++i) {
			//printf("%x\n", instructionList[i]);
			std::cout << std::setfill('0')
				<< std::dec
				<< std::showbase
				<< std::internal;
			std::cout << std::setfill('0') << std::setw(3) << i << ": ";
			std::cout << std::hex << std::setfill('0') << std::setw(8) << instructionList[i] << "\n";
		}

		//go through list of instructions ... perform each action
		std::cout << "behavioral simulation of simple MIPS-like machine\n";
		std::cout << "(all values are shown in hexadecimal)\n";
		std::cout << "pc   result of instruction at that location\n";
		for (int i = 0; i < instructionList.size(); i++) {
			functionCall(instructionList[i]);
			if (halt)
				break;
			if (branch) {
				i = branch;
				branch = 0;
			}
		}

		//after
		std::cout << "contents of memory\n";
		std::cout << "addr value\n";
		for (int i = 0; i < instructionList.size(); i++) {
			//printf("%x\n", instructionList[i]);
			std::cout << std::setfill('0')
				<< std::dec
				<< std::showbase
				<< std::internal;
			std::cout << std::setfill('0') << std::setw(3) << i << ": ";
			std::cout << std::hex << std::setfill('0') << std::setw(8) << instructionList[i] << "\n";
		}


		/* Report Results */
		std::cout << std::dec << "instruction class counts(omits hlt instruction)\n";
		std::cout << "ops " << ic.ops << "\n";
		std::cout << "loads / stores " << ic.loads << "\n";
		std::cout << "jumps/ branches " << ic.jumps << "\n";
		std::cout << "total " << ic.ops + ic.loads + ic.jumps << "\n";

		std::cout << "memory access counts(omits hlt instruction)\n";
		std::cout << "inst.fetches " << mem.fetches << "\n";
		std::cout << "loads " << mem.loads << "\n";
		std::cout << "stores " << mem.stores << "\n";;
		std::cout << "total " << mem.fetches + mem.loads + mem.stores << "\n";

		std::cout << "transfer of control counts\n";
		std::cout << "jumps " << xfer.jumps << "\n";
		std::cout << "jump - and-links " << xfer.jumpsLinks << "\n";;
		std::cout << "taken branches " << xfer.takenBranches << "\n";;
		std::cout << "untaken branches " << xfer.untakenBranches << "\n";;
		std::cout << "total " << xfer.jumps + xfer.jumpsLinks + xfer.takenBranches + xfer.untakenBranches << "\n";
	}

	else if (output2)
	{

		//before
		std::cout << "contents of memory\n";
		std::cout << "addr value\n";
		for (int i = 0; i < instructionList.size(); ++i) {
			//printf("%x\n", instructionList[i]);
			std::cout << std::setfill('0')
				<< std::dec
				<< std::showbase
				<< std::internal;
			std::cout << std::setfill('0') << std::setw(3) << i << ": ";
			std::cout << std::hex << std::setfill('0') << std::setw(8) << instructionList[i] << "\n";
		}

		std::cout << "simple MIPS-like machine with instruction pairing\n";
		std::cout << "(all values are shown in hexadecimal)\n";

		std::cout << "instruction pairing analysis\n";
		for (int i = 0; i < instructionList.size(); i++) {
			functionCall(instructionList[i]);
			if (halt)
			{
				//clear the hlt out and print it
				instructionPair(pc, "", NONE);
				break;
			}
			if (branch) {
				i = branch;
				branch = 0;
			}
		}

		/* Report Results */
		std::cout << std::dec << "instruction class counts(omits hlt instruction)\n";
		std::cout << "ops " << ic.ops << "\n";
		std::cout << "loads / stores " << ic.loads << "\n";
		std::cout << "jumps/ branches " << ic.jumps << "\n";
		std::cout << "total " << ic.ops + ic.loads + ic.jumps << "\n";

		std::cout << "memory access counts(omits hlt instruction)\n";
		std::cout << "inst.fetches " << mem.fetches << "\n";
		std::cout << "loads " << mem.loads << "\n";
		std::cout << "stores " << mem.stores << "\n";;
		std::cout << "total " << mem.fetches + mem.loads + mem.stores << "\n";

		std::cout << "transfer of control counts\n";
		std::cout << "jumps " << xfer.jumps << "\n";
		std::cout << "jump - and-links " << xfer.jumpsLinks << "\n";;
		std::cout << "taken branches " << xfer.takenBranches << "\n";;
		std::cout << "untaken branches " << xfer.untakenBranches << "\n";;
		std::cout << "total " << xfer.jumps + xfer.jumpsLinks + xfer.takenBranches + xfer.untakenBranches << "\n";

		/* Report Results */
		std::cout << std::dec << "Instruction pairing counts (inclues hlt instruction\n";
		std::cout << "issue cycles " << ipair.issueCycles << "\n";
		std::cout << "double issues " << ipair.doubleIssues << " ( " << std::setprecision(3) << ipair.doubleIssues / (float)ipair.issueCycles * 100 << " percent of issue cycles)";
		std::cout << "control stops " << ipair.controlStops << "\n";
		std::cout << "structural stops " << ipair.structuralStops << " (" << wouldAlso << " of which would also stop on a data dep.)" << "\n";
		std::cout << "data dep. stops " << ipair.dataDepStops << "\n";
	}

	if (output3)
	{
		cache_init();
		for (int i = 0; i < instructionList.size(); i++)
			memory[i] = instructionList[i];
		//go through list of instructions ... perform each action
		for (int i = 0; i < SIZE_OF_MEM; i++) {
			functionCall(memory[i]);
			if (halt)
				break;
			if (branch) {
				i = branch;
				branch = 0;
			}
		}

		//after
		std::cout << "contents of memory\n";
		std::cout << "addr value\n";
		for (int i = 0; i < instructionList.size(); i++) {
			//printf("%x\n", instructionList[i]);
			std::cout << std::setfill('0')
				<< std::dec
				<< std::showbase
				<< std::internal;
			std::cout << std::setfill('0') << std::setw(3) << i << ": ";
			std::cout << std::hex << std::setfill('0') << std::setw(8) << instructionList[i] << "\n";
		}

		/* Report Results */
		std::cout << std::dec << "instruction class counts(omits hlt instruction)\n";
		std::cout << "ops " << ic.ops << "\n";
		std::cout << "loads / stores " << ic.loads << "\n";
		std::cout << "jumps/ branches " << ic.jumps << "\n";
		std::cout << "total " << ic.ops + ic.loads + ic.jumps << "\n";

		std::cout << "memory access counts(omits hlt instruction)\n";
		std::cout << "inst.fetches " << mem.fetches << "\n";
		std::cout << "loads " << mem.loads << "\n";
		std::cout << "stores " << mem.stores << "\n";;
		std::cout << "total " << mem.fetches + mem.loads + mem.stores << "\n";

		std::cout << "transfer of control counts\n";
		std::cout << "jumps " << xfer.jumps << "\n";
		std::cout << "jump - and-links " << xfer.jumpsLinks << "\n";;
		std::cout << "taken branches " << xfer.takenBranches << "\n";;
		std::cout << "untaken branches " << xfer.untakenBranches << "\n";;
		std::cout << "total " << xfer.jumps + xfer.jumpsLinks + xfer.takenBranches + xfer.untakenBranches << "\n";

		/* Report Results */
		std::cout << std::dec << "data cache counts\n";
		std::cout << "hits " << hits << "\n";
		std::cout << "misses " << misses << "\n";
		std::cout << "write backs " << writebacks << "\n";
	}

#ifdef _MSC_VER
	int x;
	std::cin >> x;
#endif

	return 0;
}