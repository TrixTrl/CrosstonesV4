#include "DCUtils.h"
#include "../Globals/Piece.h"

#include <Windows.h>
#include <WinUser.h>
#include <bitset>


void DCUtils::print(std::string str, bool newLine = false) {
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void DCUtils::print(char* chars, bool newLine = false) {
	std::string str(chars);
	print(str, newLine);
}

void DCUtils::print(float f, bool newLine = false) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void DCUtils::print(int f, bool newLine = false) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void DCUtils::print(size_t f, bool newLine = false) {
	std::string str = std::to_string(f);
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}

void DCUtils::print(BasicGenerator::xMove f, bool newLine = false) {
	std::string str = std::to_string(f.i);
	str += " | ";
	str += std::to_string(f.j);
	str += " | ";
	std::bitset<8> x(f.delta);
	str += x.to_string();
	if (newLine) str += "\n";
	std::wstring temp = std::wstring(str.begin(), str.end());
	LPCWSTR wideString = temp.c_str();
	OutputDebugString(wideString);
}