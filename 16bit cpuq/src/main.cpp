#include <iostream>
#include <cpu/cpu.h>

constexpr uint16_t CELL_X = 0x1000;

int16_t add(const int16_t* numbers, size_t count) {
	if (count == 0) return 0;
	reset();

	size_t p = 0;
	ram.insert(static_cast<int16_t>(instructions::LDA), p++);
	ram.insert(static_cast<int16_t>(CELL_X), p++);

	for (size_t i = 1; i < count; ++i) {
		ram.insert(static_cast<int16_t>(instructions::ADD), p++);
		ram.insert(static_cast<int16_t>(CELL_X + i), p++);
	}

	ram.insert(static_cast<int16_t>(instructions::HLT), p++);

	for (size_t i = 0; i < count; ++i) {
		ram.insert(static_cast<int16_t>(numbers[i]), CELL_X + i);
	}

	run_program(false);
	return data.acr;
}

int16_t sub(const int16_t* numbers, size_t count) {
	if (count == 0) return 0;
	reset();

	size_t p = 0;
	ram.insert(static_cast<int16_t>(instructions::LDA), p++);
	ram.insert(static_cast<int16_t>(CELL_X), p++);

	for (size_t i = 1; i < count; ++i) {
		ram.insert(static_cast<int16_t>(instructions::SUB), p++);
		ram.insert(static_cast<int16_t>(CELL_X + i), p++);
	}

	ram.insert(static_cast<int16_t>(instructions::HLT), p++);

	for (size_t i = 0; i < count; ++i) {
		ram.insert(static_cast<int16_t>(numbers[i]), CELL_X + i);
	}

	run_program(false);
	return data.acr;
}

int16_t mul(const int16_t* numbers, size_t count) {
	if (count == 0) return 0;
	reset();

	size_t p = 0;
	ram.insert(static_cast<int16_t>(instructions::LDA), p++);
	ram.insert(static_cast<int16_t>(CELL_X), p++);

	for (size_t i = 1; i < count; ++i) {
		ram.insert(static_cast<int16_t>(instructions::MUL), p++);
		ram.insert(static_cast<int16_t>(CELL_X + i), p++);
	}

	ram.insert(static_cast<int16_t>(instructions::HLT), p++);

	for (size_t i = 0; i < count; ++i) {
		ram.insert(static_cast<int16_t>(numbers[i]), CELL_X + i);
	}

	run_program(false);
	return data.acr;
}

int16_t divide(const int16_t* numbers, size_t count) {
	if (count == 0) return 0;
	for (size_t i = 1; i < count; ++i) {
		if (numbers[i] == 0) return 0;
	}
	reset();

	size_t p = 0;
	ram.insert(static_cast<int16_t>(instructions::LDA), p++);
	ram.insert(static_cast<int16_t>(CELL_X), p++);

	for (size_t i = 1; i < count; ++i) {
		ram.insert(static_cast<int16_t>(instructions::DIV), p++);
		ram.insert(static_cast<int16_t>(CELL_X + i), p++);
	}

	ram.insert(static_cast<int16_t>(instructions::HLT), p++);

	for (size_t i = 0; i < count; ++i) {
		ram.insert(static_cast<int16_t>(numbers[i]), CELL_X + i);
	}

	run_program(false);
	return data.acr;
}

int main()
{
	std::cout << std::dec;

	int16_t input[] = { 20, 6 };

	std::cout << "add: " << add(input, sizeof(input) / sizeof(input[0])) << "\n";
	std::cout << "sub: " << sub(input, sizeof(input) / sizeof(input[0])) << "\n";
	std::cout << "mul: " << mul(input, sizeof(input) / sizeof(input[0])) << "\n";
	std::cout << "div: " << divide(input, sizeof(input) / sizeof(input[0])) << "\n";
	std::cin.get();
	return 0;
}