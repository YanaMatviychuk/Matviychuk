#include "stdafx.h"
#include "galua_field.h"

uint64_t inflate(uint64_t number, half h)
{
    if (h == half::left)
		number = number >> 32;
	uint64_t result{ number & 1ull };
	for (int i = 1; i <= 31; i++)
	{
		result += (((number >> i) & 1ull) << (2 * i));
	}
	return result;
}
// size m
uLI bToULi(const char* given, unsigned int bitstoprocess)
{
	uLI res{};
	//int fulldigs = m / 64;
	//int whatsleft = m % 64;
	for (; bitstoprocess > 64; bitstoprocess -= 64)
	{
		char current_group[65]{};
		memcpy(&(current_group[0]), &(given[bitstoprocess - 64u]), 64);
		current_group[64] = '\0';	// "0101110001010011"
		uint64_t got = std::bitset<64>(current_group).to_ullong();
		res.push_back(got);
	}
	char* last_group = new char[bitstoprocess + 1ull];
	memcpy(&(last_group[0]), &(given[0]), bitstoprocess);
	last_group[bitstoprocess] = '\0';
	uint64_t got = std::bitset<64>(last_group).to_ullong();
	res.push_back(got);
	res.purify();
	delete[] last_group;
	return res;
}

pGF::pGF(const uLI* _base, const std::vector<unsigned int>& elem) : base(_base), number(0ull)
{
	
    for (auto e : elem)
	{
		number = number + uLI(1ull).bitmove(-(int64_t)e);
	}
}
pGF& pGF::operator=(const pGF& given)
{
	if (&given != this)
	{
		base = given.base;
		number = given.number;
	}

	return *this;
}
void pGF::set(char given[m])
{
	number = bToULi(given, m);
}
string pGF::to_string() const
{
	// 101 10001001
	string result{};
	uLI cop{ number };
	int coplen = cop.length();
	int diff = (m + 63) / 64 - coplen;
	while (diff > 0)
	{
		cop.push_back();
		diff--;
	}
	int bignum = m / 64;
	result += std::bitset<m % 64>(cop[bignum]).to_string();
	bignum--;
	for (; bignum >= 0; bignum--)
	{
		result += std::bitset<64>(cop[bignum]).to_string();
	}
	return result;
}
pGF pGF::operator+(const pGF& other) const
{
	if (base == other.base)
	{
		int dislen = number.BitLength();
		int givlen = other.number.BitLength();
		if (dislen < givlen)
		{
			return (other + *this);
        }
		else
		{

			// givlen >= dislen

			pGF result{ other };

			int disDigits = (dislen + 63) / 64;
			int givDigits = (givlen + 63) / 64;

			int diff = disDigits - givDigits;

			for (int i = 0; i < diff; i++)
			{
				result.number.push_back(0ull);
			}

			for (int i = 0; i < disDigits; i++)
			{
				result.number.number[i] = result.number.number[i] ^ number.number[i];
			}

			return result;
		}

	}
	else
		throw std::exception("Different bases!");

}
pGF pGF::operator-(const pGF& other) const
{
	if (base == other.base)
	{
		return operator+(other);
	}
	else
		throw std::exception("Different bases!");
}
pGF pGF::operator*(const pGF& other) const
{
	if (base == other.base)
	{
		if (number == 0ull)
			return{ base, uLI(0ull) };
		else if (other.number == 0ull)
			return{ base, uLI(0ull) };
		else if (number == 1ull)
			return{ other };
		else if (other.number == 1ull)
			return{ *this };
		else
		{
			int disbitlen = number.BitLength();
			int givbitlen = other.number.BitLength();

			if (givbitlen > disbitlen)
				return other * (*this);
			else
			{
				pGF result{ base, uLI(0ull) };
				pGF temp{ *this };
				//uLI buf = other.number;
				int givlen = (givbitlen) / 64;
				for (int i = 0; i < givlen; i++)
				{
					uint64_t buf = other.number[i];
					for (int j = 0; j < 64; j++)
					{
						if (buf & 1ull)
							result = result + temp;
						temp.number.bitmove(-1);
						buf = buf >> 1;
					}
				}
				uint64_t buf = other.number[givlen];
				for (; buf > 0ull; buf = buf >> 1)
				{
					if (buf & 1ull)
						result = result + temp;
					temp.number.bitmove(-1);
				}
				//result.number = result.number % *base;
				result.reduce();

				return result;

				}
		}
		//return { base, (number * other.number) % *base };
	}
	else
		throw std::exception("Different bases!");
}
pGF pGF::operator/(const pGF& other) const
{
	if (base == other.base)
	{
		return operator*(other.inverse());
	}
	else
		throw std::exception("Different bases!");
}
void pGF::reduce()
{
	pGF R = *this;
	R.number.purify();
	int k = base->BitLength();
	while (R.number >= *base)
	{
		int t = R.number.BitLength();
		pGF C{ base, *base };
		C.number.bitmove(-(t - k));
		if (R.number < C.number)
		{
			//t--;
			C.number.bitmove(1);
		}
		R = R - C;
		R.number.purify();
	}

	number = R.number;
}
pGF pGF::trace() const
{
	pGF current{ *this };
	pGF result{ *this };
	for (int i = 1; i < m; i++)
	{
		current = current.sqr();
		result = result + current;
	}
	return result;
}
pGF pGF::sqr() const
{
	uLI RES;
	int givlen = number.length();
	for (int i = 0; i < givlen; i++)
	{
		RES.push_back(inflate(number[i], half::right));
		RES.push_back(inflate(number[i], half::left));
	}
	pGF result{ base, RES };
	result.number.purify();
	result.reduce();
	//return { base, result % *base };
	return result;
}
pGF pGF::pow(const uLI& given) const
{
	if (number == 0ull)
		return{ base, uLI(0ull) };
	else if (number == 1ull)
		return{ base, uLI(1ull) };
	else if (given == 0ull)
		return{ base, uLI(1ull) };
	else if (given == 1ull)
		return *this;
	else if (given > (uLI(1ull).bitmove(-m) - uLI(1ull)))
		throw std::overflow_error("Length is more than maximal!");
	else
	{
		pGF result{ base, 1ull };
		pGF temp{ *this };
		uLI buf = given;
		int givlen = given.length();
		for (int i = 0; i < givlen - 1; i++)
		{
			uint64_t buf = given[i];
			for (int j = 0; j < 64; buf = buf >> 1)
			{	// те саме логарифмічне зменшення без необхідності зсувати все число кожен раз
				if (buf & 1ull)
					result = result * temp;
				temp = temp.sqr();
				j++;
			}
		}
		uint64_t last = given[givlen - 1];
		for (; last > 0ull; last = last >> 1)
		{	// те саме логарифмічне зменшення без необхідності зсувати все число кожен раз
			if (last & 1ull)
				result = result * temp;
			temp = temp.sqr();
		}
		//result.reduce();
		//result.number = result.number % *base;
		return result;
	}


	//return {base, number.pow(power) % *base };
}
pGF pGF::inverse() const
{
	if (number == 0ull)
		throw std::exception("Division by zero!");
	else
		return pow(uLI(1ull).bitmove(-m) - uLI(2ull));
	//return {base, LI::inverseMod(number, *base)};
}

Ma::Ma()
{
	cout << "please wait, the Lambda is being constructed...\n";
	cout << "it will look like this:\n";
	for (int i = 0; i < 293; i++)
		cout << '=';
	cout << '\n';
	// ініціалізуємо алгебру Монтгомері для швидного підрахунку залишків від ділення на p
	MN bass{ p };
	int allsum = 0;
	concurrency::parallel_for(int(0), m, [&](int i)
		{
		int localsum = 0;
	    std::bitset<m> diset;
		for (int j = 0; j < m; j++)
		{
			bool re;
			//if (i > j)
			//	re = Lambda[j][i]; else
			if (LI::MontRedPlus(uLI(1ull).bitmove(-i), uLI(1ull).bitmove(-j), bass) == uLI(1ull))
				re = true;
			else if (LI::MontRedMinus(uLI(1ull).bitmove(-i), uLI(1ull).bitmove(-j), bass) == uLI(1ull))
				re = true;
			else if (LI::MontRedMinus(uLI(1ull).bitmove(-j), uLI(1ull).bitmove(-i), bass) == uLI(1ull))
				re = true;
			else if (LI::MontRedMinus(uLI(0ull), LI::MontRedPlus(uLI(1ull).bitmove(-i), uLI(1ull).bitmove(-j), bass), bass) == uLI(1ull))
				re = true;
			else
				re = false;
			diset.set(j, re);
			if (re) localsum++;
		}

		{
			cout << '=';
			Lambda[i] = diset;
			allsum += localsum;
		}
		//LambdaLock.unlock();
	}
	);
	cout << "\nLambda was constructed!\ncontrol sum is " << allsum << " (must be 2m-1=" << 2 * m - 1 << ")\n";
}

const Ma LaM = Ma();

nGF& nGF::operator=(const nGF& given)
{
	if (&given != this)
	{
		base = given.base;
		number = given.number;
	}
	return *this;
}
nGF nGF::zero(const uLI* _base)
{
	std::bitset<m> zeros;
	zeros.reset();	// встановлює всі біти в 0
	nGF result(_base, zeros.to_string());
	return result;
}
nGF nGF::one(const uLI* _base)
{
	std::bitset<m> ones;
	ones.set();	// встановлює всі біти в 1
	nGF result(_base, ones.to_string());
	return result;
}
void nGF::set(char given[m])
{
	number = bToULi(given, m);
}
string nGF::to_string() const
{
	string result{};
	uLI cop{ number };
	int coplen = cop.length();
	int diff = (m + 63) / 64 - coplen;
	while (diff > 0)
	{
		cop.push_back();
		diff--;
	}
	int bignum = m / 64;
	result += std::bitset<m % 64>(cop[bignum]).to_string();
	bignum--;
	for (; bignum >= 0; bignum--)
	{
		result += std::bitset<64>(cop[bignum]).to_string();
	}
	return result;
}
nGF nGF::operator+(const nGF& other) const
{
	if (base == other.base)
	{
		int dislen = number.BitLength();
		int givlen = other.number.BitLength();
		if (dislen < givlen)
		{
		}
		else
		{
			// givlen >= dislen

			nGF result{ other };

			int disDigits = (dislen + 63) / 64;
			int givDigits = (givlen + 63) / 64;

			int diff = disDigits - givDigits;
			for (int i = 0; i < diff; i++)
			{
				result.number.push_back(0ull);
			}
			for (int i = 0; i < disDigits; i++)
			{
				result.number.number[i] = result.number.number[i] ^ number.number[i];
			}
			return result;
		}
	}
	else
		throw std::exception("Different bases!");
}
nGF nGF::operator-(const nGF& other) const
{
	if (base == other.base)
	{
		return operator+(other);
	}
	else
		throw std::exception("Different bases!");
}
nGF nGF::operator*(const nGF& other) const
{
	const matrix& Lambda = LaM.Lambda;

	string u_str = to_string();
	reverse(u_str.begin(), u_str.end());	// необхідно для зручної адресації (бітсети адресуються справа наліво)
	row u_(u_str);

	string v_str = other.to_string();
	reverse(v_str.begin(), v_str.end());
	row v_(v_str);

	row res;
	for (int k = 0; k < m; k++)
	{
		row uL_k;
		for (int i = 0; i < m; i++)
		{
			bool bitt = false;
			const row Lam_i = Lambda[i];
			for (int j = 0; j < m; j++)
			{
				bitt ^= (Lam_i[j] && u_[j]);
			}
			uL_k[i] = bitt;
		}
		bool bitk = false;
		for (int t = 0; t < m; t++)
		{
			bitk ^= v_[t] && uL_k[t];
		}
		res[k] = bitk;

		bool u_last = u_[0];
		u_ = u_ >> 1;
		u_[m - 1] = u_last;
		bool v_last = v_[0];
		v_ = v_ >> 1;
		v_[m - 1] = v_last;
	}

	// переводимо результат в звичайну форму
	string res_tostr = res.to_string();
	reverse(res_tostr.begin(), res_tostr.end());
	nGF result(base, res_tostr);

	return result;
}
nGF nGF::trace() const
{
	nGF result{ base };
	bool tr = false;
	int bitlen = number.BitLength();
	for (int i = 0; i < bitlen; i++)
		tr = tr || ((number[i / 64] >> (i % 64)) & 1ull);
	if (tr)
		result = one(base);	
	return result;
}
nGF nGF::sqr() const
{
	// квадрат – це циклічний зсув вправо
	nGF result{ *this };
	uint64_t lastbit = result.number[0] & 1ull;
	result.number.bitmove(1);
	if (lastbit)
	{
		for (int numlen = result.number.length(); numlen < md; numlen++)
			result.number.push_back();
		result.number.number[md - 1] = result.number[md - 1] + (1ull << (m % 64 - 1));
	}
	return result;
}
nGF nGF::sqrt() const
{
	// корінь – обернена операція до квадрату, тому це циклічний зсув вліво
	nGF result{ *this };
	for (int i = result.number.length(); i < md; i++)
		result.number.push_back();
	uint64_t firstbit = (result.number[md - 1] >> (m % 64)) & 1ull;
	result.number.bitmove(-1);
	result.number.number[0] += firstbit;
	return result;
}
nGF nGF::pow(const uLI& given) const
{
	if (number == nGF::zero(base).number)
		return nGF::zero(base);
	else if (number == nGF::one(base).number)
		return nGF::one(base);
	else if (given == 0ull)
		return nGF::one(base);
	else if (given == 1ull)
		return *this;
	else if (given > (uLI(1ull).bitmove(-m) - uLI(1ull)))
		throw std::overflow_error("Length is more than maximal!");
	else
	{
		nGF result = nGF::one(base);
		nGF temp{ *this };
		uLI buf = given;
		int givlen = given.length();
		for (int i = 0; i < givlen - 1; i++)
		{
			uint64_t buf = given[i];
			for (int j = 0; j < 64; buf = buf >> 1)
			{	
				if (buf & 1ull)
					result = result * temp;
				temp = temp.sqr();
				j++;
			}
		}
		uint64_t last = given[givlen - 1];
		for (; last > 0ull; last = last >> 1)
		{	
			if (last & 1ull)
				result = result * temp;
			temp = temp.sqr();
		}
		return result;
	}
}
nGF nGF::inverse() const
{
	if (number == 0ull)
		throw std::exception("Division by zero!");
	else
		return pow(uLI(1ull).bitmove(-m) - uLI(2ull));
}

