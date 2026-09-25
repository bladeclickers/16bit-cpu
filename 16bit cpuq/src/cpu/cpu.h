#pragma once

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include "ram.h"

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
	LDI = 0x02,
	STA = 0x03,
	ADD = 0x04,
	SUB = 0x05,
	MUL = 0x06,
	DIV = 0x07,
	JMP = 0x08,
	JZ = 0x09,
	JNZ = 0x10,
	JC = 0x11,
	JNC = 0x12,
	PUTC = 0x13,
	GETC = 0x14,
	PUTS = 0x15,
	GETS = 0x16,
	TOSTR = 0x17,
	TOINT = 0x18,
	CMP = 0x19,
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
	case instructions::LDI:
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
	case instructions::PUTC:
	case instructions::GETC:
	case instructions::PUTS:
	case instructions::GETS:
	case instructions::TOINT:
	case instructions::TOSTR:
	case instructions::CMP:
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
	case instructions::LDI:  data.acr = ram.mar; break;
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
	case instructions::PUTC: {
		putchar(static_cast<unsigned char>(ram.memory[ram.mar] & 0xFF));
		data.flags = 0;
    	break;
	}
	case instructions::GETC: {
		int c = getchar();
		if (c != EOF) ram.memory[ram.mar] = c;
		data.flags = 0;
		break;
	}
	case instructions::PUTS: {
		int16_t a = ram.mar;

		while (ram.memory[a] != 0) {
			putchar(ram.memory[a] & 0xFF);
			a++;
		}

		data.flags = 0;
		break;
	}
	case instructions::GETS: {
		int16_t a = data.acr + 1;
		int16_t b = ram.mar;

		std::vector<char> in(a);
		std::cin.getline(in.data(), a);

		for (int16_t i = 0; i < std::cin.gcount(); i++) {
			ram.memory[b + i] = in[i];
		}

		ram.memory[b + std::cin.gcount()] = 0;

		data.flags = 0;
		break;
	}
	case instructions::TOSTR: {
		int16_t value = data.acr;
		int16_t address = ram.mar;

		if (value == 0) {
			ram.memory[address] = '0';
			ram.memory[address + 1] = 0;
			data.flags = 0;
			break;
		}

		bool negative = value < 0;

		uint16_t magnitude;
		if (negative) {
			magnitude = static_cast<uint16_t>(-(int32_t)value);
		} else {
			magnitude = static_cast<uint16_t>(value);
		}

		int16_t end = address;

		while (magnitude > 0) {
			ram.memory[end++] = '0' + (magnitude % 10);
			magnitude /= 10;
		}

		if (negative) {
			ram.memory[end++] = '-';
		}

		ram.memory[end] = 0;

		int16_t left = address;
		int16_t right = end - 1;

		while (left < right) {
			int16_t temp = ram.memory[left];
			ram.memory[left] = ram.memory[right];
			ram.memory[right] = temp;

			left++;
			right--;
		}

		data.flags = 0;
		break;
	}
	case instructions::TOINT: {
		int16_t address = ram.mar;
		int16_t result = 0;
		bool negative = false;

		if (ram.memory[address] == '-') {
			negative = true;
			address++;
		}

		while (ram.memory[address] != 0) {
			char c = static_cast<char>(ram.memory[address]);

			if (c < '0' || c > '9')
				break;

			result = result * 10 + (c - '0');
			address++;
		}

		data.acr = negative ? -result : result;
		data.flags = 0;
		break;
	}
	case instructions::CMP: {
		int16_t a = data.acr;
		int16_t b = ram.memory[ram.mar];

		data.acr = a - b;
		data.flags = (static_cast<uint16_t>(a) < static_cast<uint16_t>(b) ? 0x01 : 0) | (((a >= 0 && b < 0 && data.acr < 0) || (a < 0 && b > 0 && data.acr > 0)) ? 0x02 : 0);
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
	bool overflow = data.flags & 0x02;
	bool parity = __builtin_popcount(data.acr) % 2 == 0; // msvc uses __popcnt
	
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