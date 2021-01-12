#pragma once

#include "longint.h"
#include <bitset>
#include <array>
#include <ppl.h>	
#include <mutex>	
// реалізуємо як масив uint64 у форматі little endian

constexpr int m = 293;
const int md = (m + 63) / 64;
const int p = 2 * m + 1;

typedef uLongInt uLI;
typedef MontNum MN;
typedef LongInt LI;
typedef std::bitset<m> row;
typedef std::bitset<m + 1> generow;
typedef std::array<row, m> matrix;

enum class half { right, left };

uint64_t inflate(uint64_t number, half h);
uLI bToULi(const char* given, unsigned int bitstoprocess);

class pGF
{
public:
	//const MN* base;
	const uLI* base;
	uLI number;
	pGF(const uLI* _base) : base(_base), number(0ull) {}
	pGF(const uLI* _base, const uLI& given) : base(_base), number(given) {}
	pGF(const uLI* _base, string given) : base(_base), number(bToULi(given.c_str(), given.size())) {}
	pGF(const uLI* _base, const std::vector<unsigned int>& elem);
	pGF(const pGF& given) : base(given.base), number(given.number) {}
	pGF& operator=(const pGF& given);
	~pGF() {}

	void set(char given[m]);
	string to_string() const;

	pGF operator+(const pGF& other) const;
	pGF operator-(const pGF& other) const;
	pGF operator*(const pGF& other) const;
	pGF operator/(const pGF& other) const;
	void reduce();
	pGF trace() const;
	pGF sqr() const;
	pGF pow(const uLI& given) const;
	pGF inverse() const;
};

class Ma
{
public:
	matrix Lambda;
	Ma();
	~Ma() {}
};


class nGF
{
public:
	const uLI* base;
	uLI number;
	nGF(const uLI* _base) : base(_base), number(0ull) {}
	nGF(const uLI* _base, const uLI& given) : base(_base), number(given) {}
	nGF(const uLI* _base, string given) : base(_base), number(bToULi(given.c_str(), given.size())) {}
	nGF(const nGF& given) : base(given.base), number(given.number) {}
	nGF& operator=(const nGF& given);
	~nGF() {}

	static nGF zero(const uLI* _base);
	static nGF one(const uLI* _base);

	void set(char given[m]);
	string to_string() const;
	nGF operator+(const nGF& other) const;
	nGF operator-(const nGF& other) const;
	nGF operator*(const nGF& other) const;
	nGF trace() const;
	nGF sqr() const;
	nGF sqrt() const;
	nGF pow(const uLI& given) const;
	nGF inverse() const;
};



