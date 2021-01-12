#include "longint.h"

uLongInt::uLongInt()
{
	number = vector<uint64_t>();
}
uLongInt::uLongInt(uint64_t _given)
{
	number = vector<uint64_t>(1, _given);
}
uLongInt::uLongInt(string _given)	
{
	if (_given[0] == '0')
	{
		if (_given[1] == 'x')	// шістнадцятковий ввід
		{
			// кожне uint64_t складається з 8 байтів
			// кожен байт записується двома шістнадцятковими символами
			unsigned int chars_to_process = _given.length() - 2u;
			unsigned int bytes = (chars_to_process + 1u) / 2u;
			//  довжина масиву задається кількістю байтів
			unsigned int numbers = (bytes + 7u) / 8u;
			number = vector<uint64_t>(numbers, 0u);
			
			unsigned int current = 0u;
			for (; chars_to_process > 16u; chars_to_process -= 16u)
			{
				char current_group[19];
				memcpy(&(current_group[0]), &(_given[0]), 2u);	
				memcpy(&(current_group[2]), &(_given[2u + chars_to_process - 16u]), 16u);
				current_group[18] = '\0';	
				number[current] = std::stoull(current_group, 0, 16);
				current++;
			}
			char* last_group = new char[chars_to_process+2u+1u];
			memcpy(&(last_group[0]), &(_given[0]), 2);
			memcpy(&(last_group[2]), &(_given[2]), chars_to_process);
			last_group[chars_to_process + 2] = '\0';
			number[current] = std::stoull(last_group, 0, 16);
			delete[] last_group;
		}
	}
	else if (_given == string("random"))
	{
		std::random_device rd;
		std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
		uint64_t arr[default_len];
		for (int n = 0; n < default_len; ++n)
			arr[n] = dist(rd);
		number = vector<uint64_t>{ arr, arr + default_len };
		purify();
	}
}
uLongInt::uLongInt(const uLongInt& other)
{
	number = other.number;
}
uLongInt& uLongInt::operator=(const uLongInt& other)
{
	if (&other != this)
	{
		number.clear();
		number = other.number;
	}
	return *this;
}
uLongInt::~uLongInt()
{
	number.clear();
}

void uLongInt::randomize(int digits = default_len)
{
	if (digits > 0 and digits <= MAX_SIZE)
	{
		number.clear();	
		std::random_device rd;	
		std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX); 
		uint64_t* arr = new uint64_t[digits];	
		for (int n = 0; n < digits; ++n)
			arr[n] = dist(rd);	
		number = vector<uint64_t>{ arr, arr + digits };	
		purify();
		delete[] arr;
	}
}


uLongInt uLongInt::operator+(const uLongInt& given) const
{
	uLongInt result{};
	uint64_t carry = 0;
	uint64_t sum = 0;
	unsigned int i = 0;
	if (length() >= given.length())	
	{
		for (; i < given.length(); i++)
		{
			sum = number[i] + given[i] + carry;
			carry = (sum < number[i] or sum < given[i]);	
			result.push_back(sum);
		}
		for (; i < length(); i++)	
		{
			sum = number[i] + carry;
			carry = (sum < number[i]);
			result.push_back(sum);
		}
	}
	else
	{
		result = (given + *this); 
	}
	if (carry)	
		result.push_back(carry);
	if (result.length() > MAX_SIZE)	
		throw std::overflow_error("Length is more than maximal!");

	return result;
}
uLongInt uLongInt::operator-(const uLongInt& given) const
{
	if (operator<(given))
		return uLongInt();	
	else if (operator==(given))
		return uLongInt(0ull);	
	else
	{	// по алгоритму з методички
		uint64_t borrow = 0;
		uLongInt result;
		int i = 0;
		for (; i < given.length(); i++)
		{
			uint64_t temp = number[i] - given[i]- borrow;
			if (temp > number[i])
			{
				result.push_back(temp /*+  UINT64_MAX + 1*/);
				borrow = 1;
			}
			else
			{
				result.push_back(temp);
				borrow = 0;
			}
		}
		for (; i < length(); i++)
		{
			uint64_t temp = number[i] - borrow;
			if ((temp > number[i]))
			{
				result.push_back(temp /*+  UINT64_MAX + 1*/);
				borrow = 1;
			}
			else
			{
				result.push_back(temp);
				borrow = 0;
			}
		}
		result.purify();	 
		return result;
	}
}
uLongInt uLongInt::operator*(const uint64_t& given) const
{
	uLongInt result{};
	if (given == 0ull)	
		result = 0ull;
	else if ((given & (given - 1)) == 0)	
	{
		result = *this;
		int power = 0;
		while ((given >> power) != 1ull)
		{
			power++;
		}
		result.bitmove(-power);
	}
	else
	{
		int len = number.size();
		for (int i = len - 1; i > 0; i--)
		{
			result = result + multiply(given, number[i]);
			result.move_arr(1);
		}
		result = result + multiply(given, number[0]);
		result.purify();
	}
	return result;
}
uLongInt uLongInt::operator*(const uLongInt& given) const
{
	uLongInt result{};

	int givlen = given.length();
	if (givlen > number.size())
		result = given * *this;	
	else
	{
		if (givlen > 0)
		{
			if (given == uLongInt(0ull))	
				result = uLongInt(0ull);
			else if (operator==(uLongInt(0ull)))	
				result = uLongInt(0ull);
			else if (given.isPowerOfTwo())	
			{
				result = *this;
				int power = 0;
				while ((given[givlen - 1] >> power) != 1ull)	
				{
					power++;
				}
				if (givlen > 1)
					power += (givlen - 1) * 64;		
				result.bitmove(-power);		
			}
			else if (isPowerOfTwo())	
			{
				result = given;
				int len = length();
				int power = 0;
				while ((operator[](len - 1) >> power) != 1ull)	
				{
					power++;
				}
				if (len > 1)
					power += (len - 1) * 64;		
				result.bitmove(-power);		
			}
			else
			{
				for (int i = givlen - 1; i >= 0; i--)
				{
					uint64_t thisdigit = given[i];
					result = result + (operator*(thisdigit)).move_arr(i);
				}
			}
		}
	}
	return result;
}
uLongInt uLongInt::operator/(const uLongInt& given) const
{
	uLongInt result;
	int givlen = given.length();
	
        if (length() != 0 and givlen != 0)
	{
		if (operator==(uLongInt(0ull)))
			result = *this;
		else if (given == uLongInt(0ull))
			throw std::exception("Division by zero!");
		else if (given == uLongInt(1ull))
			result = *this;
		else if (operator<(given))
			result = uLongInt(0ull);
		else if (given.isPowerOfTwo())
		{
			result = *this;
			int power = 0;
			while ((given[givlen-1] >> power) != 1)
				power++;
			if (givlen > 1)
				power += (givlen - 1) * 64;
			result.bitmove(power);
		}
		else
		{
			// за методичкою
			int k = given.BitLength();
			uLongInt R{ *this };
			uLongInt Q{ 0ull };
			while (R >= given)
			{
				int t = R.BitLength();
				uLongInt C{ given };
				C.bitmove(-(t - k));
				if (R < C)
				{
					t--;
					C.bitmove(1);
				}
				R = R - C;
				Q = Q + (uLongInt(1ull)).bitmove(-(t - k));
			}
			result = Q;
		}
	}
	return result;
}
uLongInt uLongInt::operator%(const uLongInt& given) const
{
	uLongInt result;
	int givlen = given.length();

	if (length() != 0 and givlen != 0)
	{
		if (operator==(uLongInt(0ull)))
			result = *this;
		else if (given == uLongInt(0ull))
			throw std::exception("Division by zero!");
		 else if (given == uLongInt(1ull))
			result = 0ull;
		else if (operator<(given))
			result = *this;
		else if (given.isPowerOfTwo())
		{
			int power = 0;
			while ((given[givlen - 1] >> power) != 1)
				power++;
			if (givlen > 1)
				power += (givlen - 1) * 64;
			int j;
			for (j = 0; j < power / 64; j++)
			{
				result.push_back(number[j]);	
			}
			result.push_back(number[j] & ((1ull << (power % 64)) - 1));	
			result.purify();
		}
		else
		{
			int k = given.BitLength();
			uLongInt R{ *this };
			while (R >= given)
			{
				int t = R.BitLength();
				uLongInt C{ given };
				C.bitmove(-(t - k));
				if (R < C)
				{
					//t--;
					C.bitmove(1);
				}
				R = R - C;
			}
			result = R;
		}
	}
	return result;
}
uLongInt uLongInt::sqr() const
{
	return (*this) * (*this);
}
uLongInt uLongInt::pow(const uint64_t& given) const
{
	if (length() == 0)
		return uLongInt();
	else if (*this == 0ull)
		return 0ull;
	else if (*this == 1ull)
		return 1ull;
	else if (given == 0ull)
		return 1ull;
	else if (given == 1ull)
		return *this;
	else if (given > (MAX_SIZE * 64 - 1))
		throw std::overflow_error("Length is more than maximal!");
	else if (isPowerOfTwo())
	{
		int len = length();
		int power = 0;
		while ((operator[](len - 1) >> power) != 1ull)	
		{
			power++;
		}
		if (len > 1)
			power += (len - 1) * 64;		
		return uLongInt(1ull).bitmove(-(int)given * power);
	}
	else
	{
		uLongInt result{ 1ull };
		uLongInt temp{ *this };
		uint64_t buf = given;
		for (; buf > 0ull; buf = buf >> 1)
		{	
			if (buf & 1ull)
				result = result * temp;	
			temp = temp.sqr();
		}
		return result;
	}
	
}
uLongInt uLongInt::pow(const uLongInt& given) const
{
	if ((length() == 0) or (given.length() == 0))
		return uLongInt();
	else if (*this == 0ull)
		return 0ull;
	else if (*this == 1ull)
		return 1ull;
	else if (given == 0ull)
		return 1ull;
	else if (given == 1ull)
		return *this;
	else if (given > (MAX_SIZE*64-1))
		throw std::overflow_error("Length is more than maximal!");
	else if (isPowerOfTwo())
	{
		int len = length();
		int power = 0;
		while ((operator[](len - 1) >> power) != 1ull)	
		{
			power++;
		}
		if (len > 1)
			power += (len - 1) * 64;		
		return uLongInt(1ull).bitmove(- (int)given[0] * power);
	}
	else
	{
		uLongInt result{ 1ull };
		uLongInt temp{ *this };
		int givlen = given.length();
		for (int i = 0; i < givlen; i++)
		{
			uint64_t buf = given[i];
			for (; buf > 0ull; buf = buf >> 1)
			{	
				if (buf & 1ull)
					result = result * temp;
				temp = temp.sqr();
			}
		}
		return result;
	}
	
}

int uLongInt::compare_to(const uLongInt& other) const
{
	if (length() > other.length())
		return 1;	
	else if (length() < other.length())
		return -1;	
	else
	{
		if (number == other.number)
			return 0;	
		else
		{
			int i;
			for (i = length() - 1; i >= 0; i--)	
			{
				if (number[i] > other[i])
					return 1;
				else if (number[i] < other[i])
					return -1;
			}
			return 0;	
		}
	}
}

uLongInt LongInt::inverseMod(const uLongInt& num, const uLongInt& m)
{
	//
	LongInt x{ false, 1ull };
	LongInt y{ false, 1ull };
	uLongInt result;
	uLongInt g = GreatCommDivExt(num, m, x, y);
	if (g != 1ull)
		throw std::exception("No inverse to given x in given modulo: not coprimes");
	if (x.isNegative)
		result = m - x.value % m;
	else
		result = x.value % m;
	return result;
}

MontNum::MontNum(const uLongInt& mod)
{
	n = mod;
	if ((n[0] & 1ull) == 0)
		throw std::exception("Montgomery is only defined for odd modulo!");
	R = uLongInt(1ull).move_arr(n.length());	
	R_inv = LongInt::inverseMod(R, n);			
	k = ((R * R_inv) - 1ull) / n;	
}

uLongInt LongInt::mont(const uLongInt& number, const MontNum& base)
{
	return (number * base.R) % base.n;
}
uLongInt LongInt::redc(const uLongInt& x, const MontNum& base)
{
	uLongInt u = (x + ((x * base.k) % base.R) * base.n) / base.R;
	if (u >= base.n)
		u = u - base.n;
	return u;
}
uLongInt LongInt::MontRedPlus(const uLongInt& A, const uLongInt& B, const MontNum& base)
{
	return redc(mont(A, base) + mont(B, base), base);
}
uLongInt LongInt::MontRedMinus(const uLongInt& A, const uLongInt& B, const MontNum& base)
{
	uLongInt a_ = mont(A, base);
	uLongInt b_ = mont(B, base);
	if (a_ > b_)
	{
		return redc(a_ - b_, base);
	}
	else
	{
		return redc((a_ + base.n) - b_, base);
	}
}
uLongInt LongInt::MontRedTimes(const uLongInt& A, const uLongInt& B, const MontNum& base)
{
	
	uLongInt a_ = mont(A, base);	
	uLongInt b_ = mont(B, base);	
	uLongInt x = a_ * b_;		
	uLongInt u = redc(x, base);
	return redc(u, base);
}
uLongInt LongInt::MontRedSquare(const uLongInt& A, const MontNum& base)
{
	return MontRedTimes(A, A, base);
}
uLongInt LongInt::MontRedPower(const uLongInt& number, const uLongInt& power, const MontNum& base)
{
	uLongInt a_ = mont(number, base);	
	uLongInt c_ = mont(1ull, base);	
	
	int givlen = power.length();
	for (int i = 0; i < givlen; i++)
	{
		uint64_t buf = power[i];
		for (; buf > 0ull; buf = buf >> 1)
		{	
			if (buf & 1ull)
				c_ = redc(c_ * a_, base);
			a_ = redc(a_*a_, base);
		}
	}

	return redc(c_, base);
}

uLongInt LongInt::GreatCommDiv( uLongInt a, uLongInt b)
{
	if (a == 0ull)
		return b;
	else
		return GreatCommDiv(b % a, a);
}

uLongInt LongInt::GreatCommDivExt(uLongInt a, uLongInt b, LongInt& x, LongInt& y)
{
	if (a == 0ull)
	{
		x.value = 0ull;
		y.value = 1ull;
		return b;
	}
	LongInt x_{ false, 1ull };
	LongInt y_{ false, 1ull };

	uLongInt gcd = GreatCommDivExt(b % a, a, x_, y_);
	LongInt diff{ x_.isNegative, (b / a) * x_.value };

	if (y_.isNegative == diff.isNegative)
	{
		if (y_.value > diff.value)
			x = {  y_.isNegative, y_.value - diff.value };
		else
			x = { !y_.isNegative, diff.value - y_.value };
	}
	else
		x = { y_.isNegative, y_.value + diff.value };
	y = x_;
	return gcd;
}
uLongInt LongInt::LeastCommMult(uLongInt first, uLongInt second)
{
	return (first * second)/GreatCommDiv(first, second);
}

// оператор перевірки рівності
bool uLongInt::operator==(const uLongInt& other) const
{
	return (number == other.number);	
}
// оператор перевірки нерівності
bool uLongInt::operator!=(const uLongInt& other) const
{
	return (number != other.number);	
}
bool uLongInt::operator>(const uLongInt& other) const
{
	return (compare_to(other) > 0);		
}
bool uLongInt::operator<(const uLongInt& other) const
{
	return (compare_to(other) < 0);
}
bool uLongInt::operator>=(const uLongInt& other) const
{
	return (compare_to(other) >= 0);
}
bool uLongInt::operator<=(const uLongInt& other) const
{
	return (compare_to(other) <= 0);
}

// конвертування в шістнадцяткову строку у форматі big endian
string uLongInt::toHex() const
{
	int i = number.size() - 1; 
	if (i == -1)
		return "null";	
	else
	{
		const char* digits = "0123456789ABCDEF";
		string result = string((number.size() * 8) * 3 - 1, '\0');
		int j = 0;
		for (; i > 0; i--)	
		{
			int k = 60;
			while (k > 8)
			{
				result[j] = digits[(number[i] >> k) & 0b1111]; j++; k -= 4;
				result[j] = digits[(number[i] >> k) & 0b1111]; j++; k -= 4;
				result[j] = ' '; j++;
			}
			result[j] = digits[(number[i] >> k) & 0b1111]; j++; k -= 4;
			result[j] = digits[(number[i] >> k) & 0b1111]; j++; k -= 4;
			result[j] = '|'; j++;	// кожен розряд відділяємо від наступного
		}
		int k = 60;
		while (k > 8)
		{
			result[j] = digits[(number[i] >> k) & 0b1111]; j++; k -= 4;
			result[j] = digits[(number[i] >> k) & 0b1111]; j++; k -= 4;
			result[j] = ' '; j++;
		}
		result[j] = digits[(number[i] >> k) & 0b1111]; j++; k -= 4;
		result[j] = digits[(number[i] >> k) & 0b1111];
		return result;
	}
}

// множення на 2^32 (зсув на 32 біти)
uLongInt uLongInt::multiply32() const
{
	int len = length();
	uLongInt result{ *this };
	result.push_back();
	result.number[len] = (result.number[len - 1] >> 32) & UINT32_MAX;
	for (int i = len - 1; i > 0; i--)
	{
		result.number[i] = ((result.number[i] & UINT32_MAX) << 32) + ((result.number[i - 1] >> 32) & UINT32_MAX);
	}
	result.number[0] = ((result.number[0] & UINT32_MAX) << 32);
	return result;
}
// множення на 2^64 (зсув на 64 біти)
uLongInt uLongInt::multiply64() const
{
	uLongInt result{ *this };
	return result.move_arr(1);
}

uLongInt uLongInt::operator<<(const int64_t& i) const
{
	uLongInt result{ *this };
	return result.bitmove(-i);
}

uLongInt uLongInt::operator>>(const int64_t& i) const
{
	uLongInt result{ *this };
	return result.bitmove(i);
}
