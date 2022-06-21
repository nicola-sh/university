#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <Windows.h>

std::wstring convertToWideString(const std::string& str);

struct ThreadArgs {
	size_t number_of_clients;
	std::string filename;

	const std::vector<HANDLE>& read_events;
	const std::vector<HANDLE>& modify_events;

	std::vector<size_t> read_section_counter;

	const std::vector<HANDLE>& mutexes;

	ThreadArgs(size_t num_of_clients, std::string file,const std::vector<HANDLE>& r_events, const std::vector<HANDLE>& m_events, const std::vector<HANDLE>& muts);
};