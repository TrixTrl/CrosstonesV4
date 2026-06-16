#include "Utility.h"
#include "../Globals/Piece.h"

#include <Windows.h>
#include <WinUser.h>
#include <bitset>

using namespace dc;

void Utility::print(std::string str, bool newLine = false) {
	if (newLine) str += "\n";
	std::string temp = std::string(str.begin(), str.end());
	LPCSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utility::print(char* chars, bool newLine = false) {
	std::string str(chars);
	print(str, newLine);
}

void Utility::print(float f, bool newLine = false) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::string temp = std::string(str.begin(), str.end());
	LPCSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utility::print(int f, bool newLine = false) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::string temp = std::string(str.begin(), str.end());
	LPCSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utility::print(size_t f, bool newLine = false) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::string temp = std::string(str.begin(), str.end());
	LPCSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void Utility::print(BasicGenerator::xMove f, bool newLine = false) {
	std::string str = std::to_string(f.i);
	str += " | ";
	str += std::to_string(f.j);
	str += " | ";
	std::bitset<8> x(f.delta);
	str += x.to_string();
	if (newLine) str += "\n";
	std::string temp = std::string(str.begin(), str.end());
	LPCSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}