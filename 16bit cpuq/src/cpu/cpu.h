#pragma once

#include <cstdint>
#include <iostream>
#include <cpu/ram.h>

struct data_s {
	int16_t acr;
	uint16_t pcr;
	int16_t spr;
	int16_t instr;
	int16_t flags;
} data;

enum class instructions : uint8_t {
	NOP = 0x00,
	LDA = 0x01,
	STA = 0x02,
	ADD = 0x03,
	SUB = 0x04,
	MUL = 0x05,
	DIV = 0x06,
	JMP = 0x07,
	JZ = 0x08,
	JNZ = 0x09,
	JC = 0x10,
	JNC = 0x11,
	HLT = 0xFF,
	RET = 0xFE,
};

void reset() {
	data.acr = 0;
	data.pcr = 0;
	data.spr = 0;
	data.instr = 0;
	data.flags = 0;
	ram.mar = 0;
	ram.mdr = 0;
}

void load_program(const int16_t* program, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		ram.insert(program[i], i);
	}
}

// this function isn't very readable, i don't care enough to fix it
void execute_instruction() {
	data.instr = ram.memory[data.pcr];
	data.pcr++;

	bool has_operand = false;
	switch (static_cast<instructions>(data.instr)) {
	case instructions::LDA:
	case instructions::STA:
	case instructions::ADD:
	case instructions::SUB:
	case instructions::MUL:
	case instructions::DIV:
	case instructions::JMP:
	case instructions::JZ:
	case instructions::JNZ:
	case instructions::JC:
	case instructions::JNC:
		has_operand = true;
		break;
	default:
		break;
	}
	if (has_operand) {
		ram.mar = ram.memory[data.pcr];
		data.pcr++;
	}

	switch (static_cast<instructions>(data.instr)) {
	case instructions::LDA:  data.acr = ram.memory[ram.mar]; break;
	case instructions::STA:  ram.memory[ram.mar] = data.acr; break;
	case instructions::JMP:  data.pcr = ram.mar; break;
	case instructions::JZ:   if (data.acr == 0) data.pcr = ram.mar; break;
	case instructions::JNZ:  if (data.acr != 0) data.pcr = ram.mar; break;
	case instructions::JC:   if (data.flags & 0x01) data.pcr = ram.mar; break;
	case instructions::JNC:  if (!(data.flags & 0x01)) data.pcr = ram.mar; break;
	case instructions::ADD: {
		int16_t a = data.acr, b = ram.memory[ram.mar];
		data.acr = a + b;
		data.flags = (static_cast<unsigned>(static_cast<uint16_t>(a)) + static_cast<unsigned>(static_cast<uint16_t>(b)) >> 16 ? 0x01 : 0) | (((a > 0 && b > 0 && data.acr < 0) || (a < 0 && b < 0 && data.acr > 0)) ? 0x02 : 0);
		break;
	}
	case instructions::SUB: {
		int16_t a = data.acr, b = ram.memory[ram.mar];
		data.acr = a - b;
		data.flags = (static_cast<uint16_t>(a) < static_cast<uint16_t>(b) ? 0x01 : 0) | (((a >= 0 && b < 0 && data.acr < 0) || (a < 0 && b > 0 && data.acr > 0)) ? 0x02 : 0);
		break;
	}
	case instructions::MUL: {
		int16_t a = data.acr, b = ram.memory[ram.mar];
		data.acr = a * b;
		data.flags = (((a > 0 && b > 0 && data.acr < 0) || (a < 0 && b < 0 && data.acr < 0) || (a > 0 && b < 0 && data.acr > 0) || (a < 0 && b > 0 && data.acr > 0)) ? 0x02 : 0);
		break;
	}
	case instructions::DIV: {
		int16_t a = data.acr, b = ram.memory[ram.mar];
		data.acr = a / b;
		data.flags = (b == 0 ? 0x01 : 0) | (((a >= 0 && b < 0 && data.acr < 0) || (a < 0 && b > 0 && data.acr > 0)) ? 0x02 : 0);
		break;
	}
	case instructions::HLT:
	case instructions::RET:
		break;
	}
}

void get_result() {
	int output = data.acr;
	bool carry = data.flags & 0x01;
	bool zero = data.acr == 0;
	bool negative = data.acr < 0;
	bool overflow = data.flags & 0x0;
	bool parity = __popcnt(data.acr) % 2 == 0; // msvc specific, for other compilers use __builtin_popcount
	
	std::cout << (carry ? "carry: true\n" : "");
	std::cout << (zero ? "zero: true\n" : "");
	std::cout << (negative ? "negative: true\n" : "");
	std::cout << (overflow ? "overflow: true\n" : "");
	std::cout << (parity ? "parity: true\n" : "");

	if (output < 0) {
		std::cout << "output: -" << std::hex << -output << "\n";
	}
	else {
		std::cout << "output: " << std::hex << output << "\n";
	}

	return;
}

void run_program(bool print_result = true) {
	while (true) {
		execute_instruction();
		if (data.instr == static_cast<int16_t>(instructions::HLT)) {
			break;
		}
	}
	if (print_result) get_result();
}