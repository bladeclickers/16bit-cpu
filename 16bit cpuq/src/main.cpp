#include <iostream>
#include "cpu/cpu.h"

int16_t calculator[] = {
	static_cast<int16_t>(instructions::PUTS), 93,
	static_cast<int16_t>(instructions::LDI), 19,
	static_cast<int16_t>(instructions::GETS), 400,
	static_cast<int16_t>(instructions::TOINT), 400,
	static_cast<int16_t>(instructions::STA), 450,

	static_cast<int16_t>(instructions::PUTS), 114,
	static_cast<int16_t>(instructions::LDI), 19,
	static_cast<int16_t>(instructions::GETS), 420,
	static_cast<int16_t>(instructions::TOINT), 420,
	static_cast<int16_t>(instructions::STA), 451,

	static_cast<int16_t>(instructions::PUTS), 136,
	static_cast<int16_t>(instructions::GETC), 440,

	static_cast<int16_t>(instructions::LDA), 440,
	static_cast<int16_t>(instructions::CMP), 170,
	static_cast<int16_t>(instructions::JZ), 49,

	static_cast<int16_t>(instructions::LDA), 440,
	static_cast<int16_t>(instructions::CMP), 171,
	static_cast<int16_t>(instructions::JZ), 60,

	static_cast<int16_t>(instructions::LDA), 440,
	static_cast<int16_t>(instructions::CMP), 172,
	static_cast<int16_t>(instructions::JZ), 71,

	static_cast<int16_t>(instructions::LDA), 440,
	static_cast<int16_t>(instructions::CMP), 173,
	static_cast<int16_t>(instructions::JZ), 82,

	static_cast<int16_t>(instructions::HLT),

	static_cast<int16_t>(instructions::LDA), 450,
	static_cast<int16_t>(instructions::ADD), 451,
	static_cast<int16_t>(instructions::TOSTR), 460,
	static_cast<int16_t>(instructions::PUTS), 161,
	static_cast<int16_t>(instructions::PUTS), 460,
	static_cast<int16_t>(instructions::HLT),

	static_cast<int16_t>(instructions::LDA), 450,
	static_cast<int16_t>(instructions::SUB), 451,
	static_cast<int16_t>(instructions::TOSTR), 460,
	static_cast<int16_t>(instructions::PUTS), 161,
	static_cast<int16_t>(instructions::PUTS), 460,
	static_cast<int16_t>(instructions::HLT),

	static_cast<int16_t>(instructions::LDA), 450,
	static_cast<int16_t>(instructions::MUL), 451,
	static_cast<int16_t>(instructions::TOSTR), 460,
	static_cast<int16_t>(instructions::PUTS), 161,
	static_cast<int16_t>(instructions::PUTS), 460,
	static_cast<int16_t>(instructions::HLT),

	static_cast<int16_t>(instructions::LDA), 450,
	static_cast<int16_t>(instructions::DIV), 451,
	static_cast<int16_t>(instructions::TOSTR), 460,
	static_cast<int16_t>(instructions::PUTS), 161,
	static_cast<int16_t>(instructions::PUTS), 460,
	static_cast<int16_t>(instructions::HLT),

	'E', 'n', 't', 'e', 'r', ' ', 'f', 'i', 'r', 's', 't',
	' ', 'n', 'u', 'm', 'b', 'e', 'r', ':', ' ', 0,

	'E', 'n', 't', 'e', 'r', ' ', 's', 'e', 'c', 'o', 'n',
	'd', ' ', 'n', 'u', 'm', 'b', 'e', 'r', ':', ' ', 0,

	'O', 'p', 'e', 'r', 'a', 't', 'i', 'o', 'n', ' ', '(',
	'+', ',', ' ', '-', ',', ' ', '*', ',', ' ', '/', ')',
	':', ' ', 0,

	'R', 'e', 's', 'u', 'l', 't', ':', ' ', 0,

	'+', '-', '*', '/'
};

void run_calculator() {
	reset();
	load_program(calculator, sizeof(calculator) / sizeof(calculator[0]));
	run_program(false);
}

int main()
{
	std::cout << std::dec;

	run_calculator();

	return 0;
}