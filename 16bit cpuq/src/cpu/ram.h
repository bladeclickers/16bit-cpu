#pragma once

#include <cstdint>

// 64kb ram header

struct ram_s {
	int16_t memory[0x10000];
	uint16_t mar;
	int16_t mdr;

	void insert(int16_t data, int loc) {
		if (loc < 0 || loc >= 0x10000) return;
		memory[loc] = data;
	}
} ram;