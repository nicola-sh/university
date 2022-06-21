#include "utils.h"

std::wstring convertToWideString(const std::string& str) {
    const char* c_str = str.c_str();
    wchar_t* wstr = new wchar_t[str.length() + 1];
    mbstowcs(wstr, c_str, str.size() + 1);
    std::wstring wide(wstr);
    delete[] wstr;
    return wide;
}

ThreadArgs::ThreadArgs(size_t num_of_clients, std::string file, const std::vector<HANDLE>& r_events, const std::vector<HANDLE>& m_events, const std::vector<HANDLE>& muts) :
    number_of_clients{ num_of_clients }, filename{ file }, read_events{ r_events }, modify_events{ m_events }, read_section_counter(r_events.size(), 0), mutexes{ muts } {}
