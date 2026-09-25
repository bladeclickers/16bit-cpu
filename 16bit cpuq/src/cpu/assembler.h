#pragma once

#include "cpu.h"
#include "ram.h"

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

inline int16_t conv(instructions inst) noexcept {
	return static_cast<int16_t>(inst);
}

inline int16_t parse_assembly_word(std::string_view token) {
	if (token.empty()) {
		throw std::invalid_argument("empty instruction or operand");
	}

	std::string_view digits = token;
	if (digits.front() == '+') {
		digits.remove_prefix(1);
	}

	if (digits.empty()) {
		throw std::invalid_argument("invalid instruction or operand: " + std::string(token));
	}

	int32_t value = 0;
	const auto result = std::from_chars(digits.data(), digits.data() + digits.size(), value, 10);
	if (result.ec == std::errc::result_out_of_range) {
		throw std::out_of_range("value is outside the 16-bit word range: " + std::string(token));
	}
	if (result.ec != std::errc{} || result.ptr != digits.data() + digits.size()) {
		throw std::invalid_argument("invalid instruction or operand: " + std::string(token));
	}

	constexpr int32_t min_word = std::numeric_limits<int16_t>::min();
	constexpr int32_t max_word = std::numeric_limits<uint16_t>::max();
	if (value < min_word || value > max_word) {
		throw std::out_of_range("value is outside the 16-bit word range: " + std::string(token));
	}

	const uint16_t bits = static_cast<uint16_t>(value);
	int16_t word = 0;
	static_assert(sizeof(word) == sizeof(bits));
	std::memcpy(&word, &bits, sizeof(word));
	return word;
}

inline int16_t str_to_inst(std::string_view inst) {
	if (inst == "NOP") return conv(instructions::NOP);
	if (inst == "LDA") return conv(instructions::LDA);
	if (inst == "LDI") return conv(instructions::LDI);
	if (inst == "STA") return conv(instructions::STA);
	if (inst == "ADD") return conv(instructions::ADD);
	if (inst == "SUB") return conv(instructions::SUB);
	if (inst == "MUL") return conv(instructions::MUL);
	if (inst == "DIV") return conv(instructions::DIV);
	if (inst == "JMP") return conv(instructions::JMP);
	if (inst == "JZ") return conv(instructions::JZ);
	if (inst == "JNZ") return conv(instructions::JNZ);
	if (inst == "JC") return conv(instructions::JC);
	if (inst == "JNC") return conv(instructions::JNC);
	if (inst == "PUTC") return conv(instructions::PUTC);
	if (inst == "GETC") return conv(instructions::GETC);
	if (inst == "PUTS") return conv(instructions::PUTS);
	if (inst == "GETS") return conv(instructions::GETS);
	if (inst == "TOSTR") return conv(instructions::TOSTR);
	if (inst == "TOINT") return conv(instructions::TOINT);
	if (inst == "CMP") return conv(instructions::CMP);
	if (inst == "HLT") return conv(instructions::HLT);
	if (inst == "RET") return conv(instructions::RET);

	return parse_assembly_word(inst);
}

inline std::vector<std::string> parseln(std::string_view in) {
	std::vector<std::string> out;
	std::istringstream input{std::string(in)};
	std::string word;

	while (input >> word) {
		out.push_back(word);
	}

	return out;
}

inline std::vector<std::string> parse(std::string_view in) {
	std::istringstream input{std::string(in)};
	std::vector<std::string> lines;
	std::string line;

	while (std::getline(input, line)) {
		lines.push_back(line);
	}

	return lines;
}

inline std::vector<int16_t> assemble(std::string_view in) {
	std::vector<int16_t> program;
	std::size_t line_number = 0;

	for (std::string line : parse(in)) {
		line_number++;

		const auto comment_start = line.find('#');
		if (comment_start != std::string::npos) {
			line.resize(comment_start);
		}

		for (const std::string& token : parseln(line)) {
			int16_t word = 0;
			try {
				word = str_to_inst(token);
			}
			catch (const std::exception& error) {
				throw std::invalid_argument("line " + std::to_string(line_number) + ": " + error.what());
			}
			program.push_back(word);
		}
	}

	return program;
}
