
#include "longint.h"
#include <chrono>
#include <functional>
#include <Windows.h>
using namespace std::chrono;
void pure_measure(std::function<void()> fun, const unsigned int iter = 1 << 10)
{
	auto begin = steady_clock::now();
	for (int i = 0; i < iter; i++)
	{
		fun();
	}
	auto end = steady_clock::now();
	cout << "Average time for " << iter << " iterations: " << (duration_cast<nanoseconds>(end - begin).count() / iter) << " nanoseconds\n";

}
void random_measure(std::function<void(const uLongInt&)> fun, const unsigned int left_len = uLongInt::default_len, unsigned const int iter = 1 << 10)
{
	uLongInt first;
	auto begin = steady_clock::now();
	for (int i = 0; i < iter; i++)
	{
		first.randomize(left_len);
		fun(first);
	}
	auto end = steady_clock::now();
	cout << "Average time for " << iter << " iterations: " << (duration_cast<nanoseconds>(end - begin).count() / iter) << " nanoseconds\n";
}
void random_measure(std::function<void(const uLongInt&, const uLongInt&)> fun, const unsigned int left_len = uLongInt::default_len, const unsigned int right_len = uLongInt::default_len, unsigned const int iter = 1 << 10)
{
	uLongInt first;
	uLongInt second;
	auto begin = steady_clock::now();
	for (int i = 0; i < iter; i++)
	{
		first.randomize(left_len);
		second.randomize(right_len);
		fun(first, second);
	}
	auto end = steady_clock::now();
	cout << "Average time for " << iter << " iterations: " << (duration_cast<nanoseconds>(end - begin).count() / iter) << " nanoseconds\n";
}
void random_mont_measure(std::function<void(const uLongInt&, const MontNum&)> fun, const MontNum& base, const unsigned int left_len = uLongInt::default_len, unsigned const int iter = 1 << 10)
{
	uLongInt first;
	auto begin = steady_clock::now();
	for (int i = 0; i < iter; i++)
	{
		first.randomize(left_len);
		fun(first, base);
	}
	auto end = steady_clock::now();
	cout << "Average time for " << iter << " Mont iterations: " << (duration_cast<nanoseconds>(end - begin).count() / iter) << " nanoseconds\n";
}
void random_mont_measure(std::function<void(const uLongInt&, const uLongInt&, const MontNum&)> fun, const MontNum& base, const unsigned int left_len = uLongInt::default_len, const unsigned int right_len = uLongInt::default_len, unsigned const int iter = 1 << 10)
{
	uLongInt first;
	uLongInt second;
	auto begin = steady_clock::now();
	for (int i = 0; i < iter; i++)
	{
		first.randomize(left_len);
		second.randomize(right_len);
		fun(first, second, base);
	}
	auto end = steady_clock::now();
	cout << "Average time for " << iter << " Mont iterations: " << (duration_cast<nanoseconds>(end - begin).count() / iter) << " nanoseconds\n";
}

void Lab1_demo()
{
	// частина А
	uLongInt A = uLongInt{ "0x456" };
	uLongInt B = uLongInt{ "0x24" };
	cout << "let A:   ";
	A.showBig();
	cout << "let B:   ";
	B.showBig();

	// 1) переведення констант
	uLongInt zero{ 0ull };
	uLongInt one{ 1ull };
	cout << "zero:    ";
	zero.showBig();
	cout << "one:     ";
	one.showBig();
	// 2) додавання чисел
	cout << "B + A:   ";
	(B + A).showBig();
	// 3) віднімання чисел
	cout << "B - A:   ";
	(B - A).showBig();
	cout << "A - B:   ";
	(A - B).showBig();
	// 4) множення чисел, квадрат
	cout << "A * B:   ";
	(A * B).showBig();
	cout << "A^2:     ";
	(A.sqr()).showBig();
	// 5) ділення, остача від ділення
	cout << "A / B:   ";
	(A / B).showBig();
	cout << "A % B:   ";
	(A % B).showBig();
	cout << "B / A:   ";
	(B / A).showBig();
	cout << "B % A:   ";
	(B % A).showBig();
	// 6) піднесення до степеня
	cout << "A^B:     ";
	(A.pow(B)).showBig();
	// 7) конвертування в строку та зі строки в число
	uLongInt D{ "0x100000FFFFFFFFFFFF0" };
	cout << "D = 0x " << D.toHex() << '\n';

	cout << "D big bit number = " << D.BitLength() << '\n';
	cout << "D moved 4 bits left = 0x" << D.bitmove(-4).toHex() << '\n';

	// частина Б
	cout << "(A+B)*D= ";
	((A + B) * D).showBig();
	cout << "A*D+B*D= ";
	(A * D + B * D).showBig();

	for (int i = 0; i < 123; i++)
		zero = zero + B;
	cout << "123 B++B=";
	zero.showBig();
	cout << "123 * B =";
	(B * 123ull).showBig();

	//перевірка ділення:
	cout << "(A / B) * B + (A % B) = A:\n";
	((A / B) * B + (A % B)).showBig();
	cout << "(B / A) * A + (B % A) = B:\n";
	(A * (B / A) + (B % A)).showBig();
	cout << "B * 16 = ";
	(B * 16ull).showBig();
	(B * (uLongInt(256ull).move_arr(1))).showBig();

	// вимірювання

	cout << "1000 \n";
	random_measure([](const uLongInt&, const uLongInt&) {});
	cout << "given  A + B: \n";
	pure_measure([&A, &B]() {A + B; });
	cout << "random A + B: \n";
	random_measure([](const uLongInt& a, const uLongInt& b) {a + b; }, A.length(), B.length());
	cout << "given  A - B: \n";
	pure_measure([&A, &B]() {A - B; });
	cout << "random A - B: \n";
	random_measure([](const uLongInt& a, const uLongInt& b) {a - b; }, A.length(), B.length());
	cout << "given  A * B: \n";
	pure_measure([&A, &B]() {A * B; });
	cout << "random A * B: \n";
	random_measure([](const uLongInt& a, const uLongInt& b) {a * b; }, A.length(), B.length());
	cout << "given  A / B: \n";
	pure_measure([&A, &B]() {A / B; });
	cout << "random A / B: \n";
	random_measure([](const uLongInt& a, const uLongInt& b) {a / b; }, A.length(), B.length());
	cout << "given  A % B: \n";
	pure_measure([&A, &B]() {A % B; });
	cout << "random A % B: \n";
	random_measure([](const uLongInt& a, const uLongInt& b) {a % b; }, A.length(), B.length());
	cout << "given  A ^ 2: \n";
	pure_measure([&A]() {A.pow(2); });
	cout << "random A ^ 2: \n";
	random_measure([](const uLongInt& a) {a.pow(2); }, A.length());
	cout << "given  A ^ B: \n";
	pure_measure([&A]() {A.pow(3); });
	cout << "random A ^ 3: \n";
	random_measure([](const uLongInt& a) {a.pow(3); }, A.length());
}

void Lab2_demo()
{
	uLongInt A = uLongInt{ "0x94EDE1A444B9738ADF06CDB40DCAFA87B25A8BECA2D2262A53D8431A119405F0CBEFB83D2AD547CCE3AE74A8EC74A313C8BED20D4349D9EFBA356FE6E8AD89E2" };
	uLongInt B = uLongInt{ "0x5BCC0B222EE17877C9EB60FA91632BC7A6E29D80F02CD3FE16B5C2A2231B43DB2B2D12F21B293AAF49FE1165CB7A21D12D6ACEC225285544B36BABD3F8B4DD8D" };
	cout << "let A:   ";
	A.showBig();
	cout << "let B:   ";
	B.showBig();

	uLongInt mod = uLongInt{ "0x8E06E4DFFB37B57A66ECC52CF2D7D888C49C2794E6FB944C4183A128203932FEBEA4B6E62B2EBDAD4FF0B80DBEDC8439D31280D13E7E523596D92861F6A89E81" };
	MontNum base{ mod };
	// частина А
	// 1) НСК та НСД:
	LongInt x{ false, 1ull };
	LongInt y{ false, 1ull };
	cout << "GCD(A,B):\n";
	(LongInt::GreatCommDiv(A, B)).showBig();
	cout << "LCM(A,B):";
	(LongInt::LeastCommMult(A, B)).showBig();
	// 2) додавання за модулем
	cout << "(A+B)%n:\n";
	((A + B) % base.n).showBig();
	// 3) віднімання за модулем
	cout << "(A-B)%n:\n";
	((A - B) % base.n).showBig();
	cout << "(B-A)%n:\n";
	((B - A) % base.n).showBig();
	// 4) множення, квадрат
	cout << "(A*B)%n:\n";
	((A * B) % base.n).showBig();
	cout << "(A^2)%n:\n";
	(A.sqr() % base.n).showBig();
	// 5) степінь
	//cout << "(A^B)%n:\n";
	//(A.pow(B) % base.n).showBig();

	// частина Б
	// 1) дистрибутивність
	uLongInt D;
	D.randomize(2);
	cout << "(A+B)*D mod n:\n";
	(((A + B) * D) % base.n).showBig();
	cout << "A*D+B*D:\n";
	((A * D + B * D) % base.n).showBig();

	cout << "123 * B mod n = ";
	((B * 123ull) % base.n).showBig();

	uLongInt zero{ 0ull };
	for (int i = 0; i < 123; i++)
		zero = LongInt::MontRedPlus(zero, B, base);
	cout << "123 B++B mod n Mont = ";
	zero.showBig();
	cout << "123 * B mod n Mont = ";
	LongInt::MontRedTimes(B, 123ull, base).showBig();


	cout << "random A * B mod n: \n";
	random_measure([&base](const uLongInt& a, const uLongInt& b) {(a * b) % base.n; }, A.length(), B.length());
	//cout << "random mont A * B mod n: \n";
	//random_mont_measure([&base](const uLongInt& a, const uLongInt& b, const MontNum& base) {LongInt::MontRedTimes(a, b, base); }, base, A.length(), B.length());

	cout << "random A + B mod n: \n";
	random_measure([&base](const uLongInt& a, const uLongInt& b) {(a + b) % base.n; }, A.length(), 2);
	cout << "random mont A + B mod n: \n";
	random_mont_measure([&base](const uLongInt& a, const uLongInt& b, const MontNum& base) {LongInt::MontRedPlus(a, b, base); }, base, A.length(), B.length());
	cout << "random A - B mod n: \n";
	random_measure([&base](const uLongInt& a, const uLongInt& b) {(a - b) % base.n; }, A.length(), 2);
	cout << "random mont A - B mod n: \n";
	random_mont_measure([&base](const uLongInt& a, const uLongInt& b, const MontNum& base) {LongInt::MontRedMinus(a, b, base); }, base, A.length(), B.length());

	cout << "random A ^ 2 mod n: \n";
	random_measure([&base](const uLongInt& a, const uLongInt& b) {a.sqr() % base.n; }, 2);
	cout << "random mont A ^ 2 mod n: \n";
	random_mont_measure([&base](const uLongInt& a, const uLongInt& b, const MontNum& base) {LongInt::MontRedSquare(a, base); }, base, A.length());

	uLongInt C{ 3ull };

	cout << "random B ^ d mod n: \n";
	random_measure([&base, &C](const uLongInt& a, const uLongInt& b) {a.pow(C) % base.n; }, B.length());
	cout << "random mont B ^ D mod n: \n";
	random_mont_measure([&base, &C](const uLongInt& a, const uLongInt& b, const MontNum& base) {LongInt::MontRedPower(a, C, base); }, base, B.length());


}
int main()
{

	try
	{
		cout << "Enter lab number: ";
		int n;
		cin >> n;
		if (n == 1)
			Lab1_demo();
		else if (n == 2)
			Lab2_demo();

	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
	}
	system("PAUSE");
}
