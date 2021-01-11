#pragma once
#include <vector>   // для внутрішнього представлення
#include <string>   // для обробки тексту
#include <iostream> // для вводу-виводу через консоль
#include <random>   // для випадкової генерації числа
#define and &&
#define or ||
#define xor ^
using std::vector;
using std::string;

using std::cin;
using std::cout;
using std::endl;

// реалізуємо як масив uint64 у форматі little endian

class uLongInt
{
public:
	// максимальна кількість розрядів обмежує використовувану пам'ять
	const static unsigned int MAX_SIZE{ 32 };       // 2048 біт
													// довжина випадково згенерованого числа за замовчуванням
	const static unsigned int default_len{ 4 };
	// внутрішнє представлення числа – вектор 64-бітних розрядів від молодшого до старшого
	vector<uint64_t> number;
	uLongInt();
	// конструктор за числом
	uLongInt(uint64_t);
	// конструктор за строкою в коректному форматі
	uLongInt(string);

	// конструктор копіювання
	uLongInt(const uLongInt&);
	uLongInt& operator=(const uLongInt&);

	// деструктор
	~uLongInt();
	void randomize(int digits);
	// оператор додавання
	uLongInt operator+(const uLongInt&) const;
	// оператор віднімання
	uLongInt operator-(const uLongInt&) const;
	// оператор множення на однорозрядне число
	uLongInt operator*(const uint64_t&) const;
	// оператор множення 
	uLongInt operator*(const uLongInt&) const;
	// оператор цілочисельного ділення
	uLongInt operator/(const uLongInt&) const;
	// оператор остачі від ділення
	uLongInt operator%(const uLongInt&) const;
	// множення двох розрядів
	static uLongInt multiply(uint64_t _left, uint64_t _right)
	{
		// (10a+b)*(10c+d) = bd + 10*(ad + bc) + 100*ac 
		uint64_t first[2]{ _left & UINT32_MAX, (_left >> 32) & UINT32_MAX };
		uint64_t second[2]{ _right & UINT32_MAX, (_right >> 32) & UINT32_MAX };
		uLongInt carry{ first[1] * second[1] };
		carry.move_arr(1);
		carry = carry + uLongInt(first[0] * second[0]);
		carry = carry + (uLongInt(first[0] * second[1]) + uLongInt(first[1] * second[0])).multiply32();
		carry.purify();
		return carry;
	}
	// множення на 2^32 (зсув на 32 біти)
	uLongInt multiply32() const;
	// швидке множення на 2^64 (зсув на 64 біти)
	uLongInt multiply64() const;
	// бітовий зсув всього числа на i бітів вліво (до більшого)
	uLongInt operator<<(const int64_t&) const;
	// бітовий зсув всього числа на i бітів вправо (до меншого) 
	uLongInt operator >> (const int64_t&) const;


	// піднесення до однорозрядного степеня
	uLongInt pow(const uint64_t&) const;
	// піднесення до степеня
	uLongInt pow(const uLongInt&) const;
	// квадрат даного числа
	uLongInt sqr() const;

	// оператор перевірки рівності
	bool operator==(const uLongInt&) const;
	// оператор перевірки нерівності
	bool operator!=(const uLongInt&) const;

	// оператори порівняння

	bool operator>(const uLongInt&) const;
	bool operator<(const uLongInt&) const;
	bool operator>=(const uLongInt&) const;
	bool operator<=(const uLongInt&) const;
	// функція порівняння (+1 для більше, 0 для рівності, -1 для менше) 
	int compare_to(const uLongInt&) const;

	// конвертування в шістнадцяткову строку
	string toHex() const;
	// довжина числа (кількість розрядів)
	size_t length() const
	{
		return number.size();
	}
	// дописує справа один старший розряд
	void push_back(uint64_t given = 0ull)
	{
		number.emplace_back(given);
	}
	const uint64_t& operator[](size_t index) const
	{
		return number[index];
	}
	uLongInt& move_arr(int i)
	{
		int s = number.size();

		if (i > 0)  
		{
			if (i + s > MAX_SIZE)
				throw std::overflow_error("Length is more than maximal!");
			for (int k = 0; k < i; k++)
				number.emplace_back();
			memmove(&(number[i]), &(number[0]), s * 8);    
														  
			for (int k = 0; k < i; k++)
				number[k] = 0;
		}
		else if (i < 0) 
		{
			i = -i;
			if (i >= s)
			{
				number.clear();
				number.emplace_back(0ull);
			}
			else
			{
				memmove(&(number[0]), &(number[i]), (s - i) * 8); 
																  
				for (int k = 0; k < i; k++)
					number.pop_back();
			}
		}
		return *this;
	}
	uLongInt& bitmove(int bits_to_move)
	{
		if (bits_to_move > 0)   
		{
			if (bits_to_move >= BitLength())
			{
				number.clear();
				number.emplace_back(0ull);
			}
			else if (bits_to_move >= 64) 
			{
				move_arr(-(bits_to_move / 64)); 
				bitmove(bits_to_move % 64);     
			}
			else
			{

				uint64_t toprev_mask = (1ull << bits_to_move) - 1ull;   
				uint64_t tokeep_mask = toprev_mask xor UINT64_MAX;  

			    int len = number.size();
				for (int j = 0; j < len - 1; j++) 
		                  number[j] = ((number[j] & tokeep_mask) >> bits_to_move) + ((number[j + 1] & toprev_mask) << (64 - bits_to_move));
				number[len - 1] = (number[len - 1] & tokeep_mask) >> bits_to_move;
				if (number[len - 1] == 0ull)    
					number.pop_back();
			}
		}
		else if (bits_to_move < 0)  
		{
			bits_to_move = -bits_to_move;
			if (bits_to_move + BitLength() > MAX_SIZE * 64)
				throw std::overflow_error("Length is more than maximal!");
			else if (bits_to_move >= 64)
			{
				move_arr(bits_to_move / 64); 
				bitmove(-(bits_to_move % 64));  
			}
			else
			{
				uint64_t tokeep_mask = (1ull << (64 - bits_to_move)) - 1ull;    
				uint64_t tonext_mask = tokeep_mask xor UINT64_MAX;  
																	
				if (((number[number.size() - 1]) & tonext_mask) != 0ull)
					number.emplace_back(0ull);
				int len = number.size();
				for (int j = len - 1; j > 0; j--)   
					number[j] = ((number[j] & tokeep_mask) << bits_to_move) + ((number[j - 1] & tonext_mask) >> (64 - bits_to_move));
				number[0] = (number[0] & tokeep_mask) << bits_to_move;
			}
		}
		return *this;
	}

	void purify()
	{
		int len = number.size();
		if (len > 1)
		{
			len--;  
			while (number[len] == 0)    
			{
				number.pop_back();  
				len--;  
				if (len <= 1) break;    
			}
		}
	}
	// показати число у hex вигляді у форматі big endian
	void showBig()
	{
		cout << "0x " << toHex() << '\n';
	}
	// перевірка, що число - степінь двійки
	bool isPowerOfTwo() const
	{
		bool result;
		if (number.size() == 0) 
			result = false;
		else if (*this == uLongInt(0ull))   
			result = false;
		else
		{
			int len = number.size();
			uint64_t BigDigit = number[len - 1];    
			bool BigDigitIs = ((BigDigit & (BigDigit - 1)) == 0);   
			if (BigDigitIs)     
			{
				bool AllOthersAreZero = true;
				for (int i = len - 2; (i >= 0) and AllOthersAreZero; i--)
				{
					uint64_t ThisDigit = number[i];     
					AllOthersAreZero = AllOthersAreZero and (ThisDigit == 0ull); 
				}
				result = BigDigitIs and AllOthersAreZero;    
			}
			else  
				result = false;
		}
		return result;
	}
	// бітова довжина (номер старшого біта)
	int BitLength() const
	{
		int result = 0;
		int len = number.size();
		uint64_t BigDigit = number[len - 1];
		int i;
		for (i = 0; i < 64; i++)
			if ((1ull << i) > BigDigit) break;
		result += i;
		result += 64 * (len - 1);
		return result;
     }
};

// основні числа методу Монтгомері
class MontNum
{
public:
	// основа системи, має бути непарна
	uLongInt n;
	// число – степінь двійки, має бути більшим за n
	uLongInt R;
	// число, обернене до R за модулем n: 
	uLongInt R_inv;
	// число, що доповнює основу:
	uLongInt k;
	// конструює всі необхідні числа для даного модуля
	MontNum(const uLongInt& mod);
	~MontNum() {};
};

// містить операції по модулю
class LongInt
{
public:
	bool isNegative;
	uLongInt value;

	// знаходить число, зворотнє до даного: 
    static uLongInt inverseMod(const uLongInt& num, const uLongInt& m);
	// знаходить лишок Монтгомері в даному наборі параметрів
	static uLongInt mont(const uLongInt& number, const MontNum& base);
	// знаходить редукцію Монтгомері в даному наборі параметрів
	static uLongInt redc(const uLongInt& number, const MontNum& base);
	// швидка сума за модулем base.n
	static uLongInt MontRedPlus(const uLongInt& A, const uLongInt& B, const MontNum& base);
	// швидка різниця за модулем base.n
	static uLongInt MontRedMinus(const uLongInt& A, const uLongInt& B, const MontNum& base);
	// швидкий добуток за модулем base.n
	static uLongInt MontRedTimes(const uLongInt& A, const uLongInt& B, const MontNum& base);
	// швидкий квадрат за модулем base.n
	static uLongInt MontRedSquare(const uLongInt& A, const MontNum& base);
	// швидкий степінь за модулем base.n
	static uLongInt MontRedPower(const uLongInt& number, const uLongInt& power, const MontNum& base);


	// найбільший спільний дільник
	static uLongInt GreatCommDiv(uLongInt, uLongInt);
	// найбільший спільний дільник за розширеним алгоритмом Евкліда
	static uLongInt GreatCommDivExt(uLongInt, uLongInt, LongInt&, LongInt&);
	// найменше спільне кратне
	static uLongInt LeastCommMult(uLongInt, uLongInt);
};
