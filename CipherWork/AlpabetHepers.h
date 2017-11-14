#pragma once
#include <string>

int CharToIntFromAlphabet(char C, const std::string Alphabet)
{
	return Alphabet.find(C);
}

std::string getStrNGramFromNGramList(int i, const std::string alphabet, const  int n)
{
	int Pos = i;
	std::string TestRep;
	for (int j = 0; j < n; j++)
	{
		Pos = Pos / static_cast<int>(pow(alphabet.size(), n - j - 1));
		TestRep += alphabet.c_str()[Pos];
		Pos = i - Pos*static_cast<int>(pow(alphabet.size(), n - j - 1));
	};

	return  TestRep;
}
int getIntFromStrAndFromNGramList(std::string Str, const std::string alphabet, const  int n)
{
	int Ret = 0;
	for (int i = 0; i < n; i++)
	{
		Ret += CharToIntFromAlphabet(Str.c_str()[i], alphabet)*static_cast<int>(pow(alphabet.size(), n - i - 1));
	}
	return  Ret;
}