#pragma once
#include <cstdint>

constexpr int POOL_ALLOCATOR_MAX_ALLOCATIONS = 16384;

class c_pool_allocator
{
	void* pool = nullptr;
	bool free_list[POOL_ALLOCATOR_MAX_ALLOCATIONS] = { false };
    uint64_t offset = 0;
public:


};
