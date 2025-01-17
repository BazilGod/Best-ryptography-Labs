// GOST 28147-89.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <streambuf>

using namespace std;
constexpr uint64_t synchroChange = 0x7643;
constexpr uint64_t mask = 255;
constexpr const char* defaultPath = "D:\\6sem\\Криптографические методы защиты информации\\Лабораторная_7\\GOST 28147-89\\";

constexpr size_t ROWS = 8;
constexpr size_t COLS = 16;

constexpr uint32_t synchroCode = 0xABC4EDB1;

// таблица замен
constexpr uint8_t changeTable[ROWS][COLS] =
{
	10, 4, 5, 6, 8, 1, 3, 7, 13, 12, 14, 0, 9, 2, 11, 15,
	5, 15, 4, 0, 2, 13, 11, 9, 1, 7, 6, 3, 12, 14, 10, 8,
	7, 15, 12, 14, 9, 4, 1, 0, 3, 11, 5, 2, 6, 10, 8, 13,
	4, 10, 7, 12, 0, 15, 2, 8, 14, 1, 6, 5, 13, 11, 9, 3,
	7, 6, 4, 11, 9, 12, 2, 10, 1, 8, 0, 14, 15, 13, 3, 5,
	7, 6, 2, 4, 13, 9, 15, 0, 10, 1, 5, 11, 8, 14, 12, 3,
	13, 14, 4, 1, 7, 0, 5, 10, 3, 12, 8, 15, 6, 2, 9, 11,
	1, 3, 10, 9, 5, 11, 4, 15, 8, 6, 7, 14, 13, 0, 2, 12
};

enum class EncryptionMode { DECRYPT, ENCRYPT };

std::vector<uint64_t> getSubKey()
{
	std::vector<uint64_t> res;
	for (size_t rows = 0; rows < ROWS; ++rows)
	{
		uint64_t tmp = 0;
		for (size_t cols = 0; cols < COLS; ++cols)
			tmp |= static_cast<uint64_t> (changeTable[rows][cols]) << 4 * cols;

		res.emplace_back(tmp);
	}
	return res;
}

static std::vector<uint64_t> changeVect = getSubKey();

// ключик
uint32_t KEYS[8] =
{
	0x01234567,
	0x01234567,
	0x01234567,
	0x01234567,
	0x01234567,
	0x01234567,
	0x01234567,
	0x76543210
};

template <class T = uint64_t>
auto getSymbol(T it, T mask, size_t shift)
{
	return ((static_cast<T>(it) & (mask << shift)) >> shift);
}


vector<uint64_t> strToVect(const std::string& str)
{
	vector<uint64_t> res;

	uint64_t buf = 0;
	int count = 0;
	for (auto& it : str)
	{
		buf |= (static_cast<uint64_t>(it) << count);
		count += 8;
		if (count >= 64)
		{
			res.emplace_back(buf);
			buf = 0;
			count = 0;
		}
	}

	if (count != 0)
	{
		res.emplace_back(buf);
	}

	return res;
}


string vectToStr(const vector<uint64_t>& uint_vect)
{
	string res;
	for (auto& it : uint_vect)
	{
		for (size_t i = 0; i < 8; ++i)
		{
			uint64_t tmp = getSymbol(it, mask, i * 8);
			res += tmp;
		}
	}

	return res;
}


string readFromFile(const std::string fileName,
	const std::string path = defaultPath)
{
	std::string result;
	std::ifstream ifs(path + fileName);

	//  allocates all the memory up front (rather than relying on the string class's automatic reallocation)
	try
	{
		ifs.seekg(0, std::ios::end);
		result.reserve(ifs.tellg());
		ifs.seekg(0, std::ios::beg);

		result.assign((std::istreambuf_iterator<char>(ifs)),
			std::istreambuf_iterator<char>());
	}
	catch (...)
	{
		std::cerr << "Unable to read file: path " << path + fileName << std::endl;
	}


	return result;
}

size_t getIndForKey(const size_t index, const EncryptionMode mode)
{
	if (mode == EncryptionMode::ENCRYPT)
		return (index < 24) ? (index % (ROWS - 2)) : (ROWS - 1);
	else
		return (index < 8) ? (ROWS - 1) : (index % (ROWS - 2));

}

uint32_t getSubstitution(const uint32_t ind)
{
	uint32_t result = 0;

	for (size_t i = 0; i < 8; ++i)
	{
		uint8_t temp = (ind >> 4 * i) & 0x0f;
		temp = changeTable[i][temp];
		result |= temp << 4 * i;
	}
	return result;
}

uint64_t operateOnBlock(uint64_t incomeBlock, const EncryptionMode mode)
{
	uint32_t left = (0xffffffff & (incomeBlock >> 32));
	uint32_t right = incomeBlock & 0xffffffff;

	for (size_t i = 0; i< 32; ++i)
	{
		const size_t keyInd = getIndForKey(i, mode);
		const uint32_t substitutionInd = (static_cast<uint64_t>(left) + KEYS[keyInd]) % UINT32_MAX;
		uint32_t substitution = getSubstitution(substitutionInd);
		substitution = (substitution << 11);
		substitution ^= right;

		if (i < 31)
		{
			right = left;
			left = substitution;
		}
		else
		{
			right = substitution;
		}

	}

	return (static_cast<uint64_t>(left) << 32) | right;
}


const uint32_t firstEncryptedBlock = operateOnBlock(synchroCode, EncryptionMode::ENCRYPT);


std::vector<uint64_t> gammaCipher(const std::vector<uint64_t>& dataBlocks, const EncryptionMode mode)
{
	std::vector<uint64_t> result;
	uint64_t prevEncryption = operateOnBlock(firstEncryptedBlock, EncryptionMode::ENCRYPT);

	for (auto& dataBlock : dataBlocks)
	{
		const uint64_t xoredDataBlock = dataBlock ^ prevEncryption;

		result.emplace_back(xoredDataBlock);
		prevEncryption = (mode == EncryptionMode::ENCRYPT) ? (xoredDataBlock) : (dataBlock);
	}

	return result;
}


int main()
{

	std::string save_str = "D:\\6sem\\Криптографические методы защиты информации\\Лабораторная_7\\GOST 28147-89\\encrypted_data.txt";

	string massage = readFromFile("text1.txt");

	auto blockedSequense = strToVect(massage);

	cout << "\n" << vectToStr(blockedSequense) << '\n';

	// 1101110011111001000000110111110010010101001000110110101101000

	auto res = operateOnBlock(static_cast<uint64_t>(0x1B9F206F92A46D68), EncryptionMode::ENCRYPT);
	cout << "input = " << res << endl;
	cout << "output = " << operateOnBlock(res, EncryptionMode::DECRYPT) << endl;

	auto gammaEncrypted = gammaCipher(blockedSequense, EncryptionMode::ENCRYPT);
	auto gammaDecrypted = gammaCipher(gammaEncrypted, EncryptionMode::DECRYPT);

	std::string gammaDecryptedStr = vectToStr(gammaDecrypted);


	std::cout << " gammaEncryptedStr = " << vectToStr(gammaEncrypted) << std::endl;
	std::cout << " gammaDecryptedStr = " << gammaDecryptedStr << std::endl;

	std::cout << "blockedSequense[0] = " << blockedSequense[0] << std::endl;
	std::cout << "gammaDecrypted [0] = " << gammaDecrypted[0] << std::endl;

	return 0;

}
