#include "stdafx.h"
#include "galua_field.h"

void Lab3_demo()
{
	// init

	generow generator;
	generator.set(0);
	generator.set(293, 1);
	generator.set(11, 1);
	generator.set(6, 1);
	generator.set(1, 1);
	generator.set(0, 1);
	string gen = generator.to_string();
	cout << gen << '\n';
	uLI base{ bToULi(gen.c_str(), m + 1) };

	pGF first{ &base, "01010000100110000101010001010100000101110101011011001101101100110110100010100010111000010010101111101000100111110111110011100000011010001110001010101101111010011000110010100111010010010010111101001010101010010001000110000001110111110010101001011110000101011001001000011000001110010110101011011" };
	pGF second{ &base, "00011100011010101000011111110101100100101100111011101111101111111101011011111000100101110010000001101011100010011011110000101110101110000010110010000110001010010010010001010100100011001100100110001111001010100011101100001100100000011001110000110101100010111001110010000110011111110110100011100" };
	pGF zero{ &base, "0" };
	pGF one{ &base, "1" };
	cout << "A:\n";
	cout << first.to_string() << '\n';
	cout << "B:\n";
	cout << second.to_string() << '\n';

	// 1)
	cout << "1) zero:\n";
	cout << zero.to_string() << '\n';
	// 2)
	cout << "2) one:\n";
	cout << one.to_string() << '\n';
	// 3)
	cout << "3) A + B:\n";
	cout << (first + second).to_string() << '\n';
	// 4)
	cout << "4) A * B:\n";
	cout << (first * second).to_string() << '\n';
	// 5)
	cout << "5) trace A:\n";
	cout << (first.trace()).to_string() << '\n';
	// 6)
	cout << "6) A^2:\n";
	cout << (first.sqr()).to_string() << '\n';
	// 7)
	cout << "7) A^q:\n";
	cout << (first.pow(20)).to_string() << '\n';
	// 8)
	cout << "8) inverse to A:\n";
	cout << (first.inverse()).to_string() << '\n';
	cout << "9) A*A^-1:\n";
	cout << (first*first.inverse()).to_string() << '\n';


	pGF third{ &base, "100011" };
	cout << "C:\n";
	cout << third.to_string() << '\n';
	cout << "(A+B)*C:\n";
	cout << ((first + second) * third).to_string() << '\n';
	cout << "A*C+B*C:\n";
	cout << (first * third + second * third).to_string() << '\n';

}

void Lab4_demo()
{
	generow pi_2 = 0b1;
	generow pi_1 = 0b11;
	generow pi;
	for (int i = 2; i <= m; i++)
	{
		pi = (pi_1 << 1) ^ pi_2;
		pi_2 = pi_1;
		pi_1 = pi;
	}


	const char* gen = "11101001111110000110110101000010010100001101101011100100000001010001011000100010110100100111111110110010001101101000111110101100111000111101001111010111101001101001110001001001000100101001011100110001101001011011100110100010100100010100101101111011101101011010110100000001010001101010101101001";

	uLI base{ bToULi(gen, m + 1) };
	nGF first{ &base, "01001110101101111101100011111100101011110010100000001100111101111111001011001110001110111001101100101101111010100111000111101100011011110100111010011001101011111001111010100010100111001001001001101000011100010100011011000111110011011101001001001111001010010000001011111010100100010101010011010" };
	nGF second{ &base, "00101111001110011111100000101110001100011100010011011000110011000100101101111011011100001101100111001010110100100010000010011101101110000011010010100100110010100110010001000100011100101100110101011110101100001110100101001011010110010100000100000011111000110100110110011011100100000001011001010" };


	cout << '\n';
	cout << "A:\n";
	cout << first.to_string() << '\n';
	cout << "B:\n";
	cout << second.to_string() << '\n';

	// 1)
	cout << "1) zero:\n";
	cout << nGF::zero(&base).to_string() << '\n';
	// 2)
	cout << "2) one:\n";
	cout << nGF::one(&base).to_string() << '\n';
	// 3)
	cout << "3) A + B:\n";
	cout << (first + second).to_string() << '\n';
	// 4)
	cout << "4) A * B:\n";
	cout << (first * second).to_string() << '\n';
	// 5)
	cout << "5) trace A:\n";
	cout << (first.trace()).to_string() << '\n';
	// 6)
	cout << "6) A^2:\n";
	cout << (first.sqr()).to_string() << '\n';
	// 7)
	cout << "7) A^q:\n";
	cout << (first.pow(5)).to_string() << '\n';
	// 8)
	cout << "8) inverse to A:\n";
	cout << (first.inverse()).to_string() << '\n';
	

}

int main()
{
	try
	{
		cout << "Enter lab number: ";
		int n;
		cin >> n;
		if (n == 3)
		    Lab3_demo();
		else if (n == 4)
		Lab4_demo();

	}
	catch (std::exception e)
	{
		std::cerr << e.what() << std::endl;
	}
	system("PAUSE");
}