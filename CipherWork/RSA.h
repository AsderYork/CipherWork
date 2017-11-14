#pragma once
#include "AlpabetHepers.h"
#include "MathHelper.h"

class RSA
{
public:
	struct Key
	{
		BigInt modulo = 0;
		BigInt exponent = 0;

		Key(BigInt _m, BigInt _e) : modulo(_m), exponent(_e) {}
		Key() {}
		std::string asString() const { return std::string("m:" + modulo.toString() + "; e:" + exponent.toString()); }
	};

	static std::pair<RSA::Key, RSA::Key> GenKey(int seed, BigInt p = 0, BigInt q = 0, BigInt e = 0);

	static BigInt Cipher(Key key, BigInt Val)
	{
		return modExp(Val, key.exponent, key.modulo);
	}

	static std::vector<BigInt> CipherTextNGRAM(std::string& Text, RSA::Key Key, int Arity, std::string alphabet)
	{
		std::vector<BigInt> RetVec;
		for (int i = 0; i < Text.size(); i += Arity)
		{
			RetVec.push_back(Cipher(Key, getIntFromStrAndFromNGramList(Text.substr(i, Arity), alphabet, Arity)));
		}
		return RetVec;
	}

	static std::string DecipherNGRAMS(std::vector<BigInt> Blocks, RSA::Key Key, int Arity, std::string alphabet)
	{
		std::string Retval;
		for (auto& Block : Blocks)
		{
			Retval += getStrNGramFromNGramList(Cipher(Key, Block).toInt(), alphabet, 2);
		}
		return Retval;
	}


	static RSA::Key Brake(Key PubKey)
	{
		BigInt x = (long long)ceil((long double)sqrt(PubKey.modulo.toLongLong()));

		BigInt k = 0;
		while (k < PubKey.modulo)
		{
			x = k + (long long)(ceil((long double)sqrt(PubKey.modulo.toLongLong())));

			auto Squere = sqrt(pow(x.toLongLong(), 2) - PubKey.modulo.toLongLong());
			auto diff = abs(floor((long double)Squere) - Squere);
			if (abs(floor((long double)Squere) - Squere) < 0.0001f)
			{
				auto p = x + (int)Squere;
				auto q = x - (int)Squere;
				//	printf("A:%lli, B:%lli\n", x + (int)Squere, x- (int)Squere);
				auto d = modInverse(PubKey.exponent, (p - 1)*(q - 1));

				return RSA::Key(PubKey.modulo, d);
			}

			k++;
		}


		return RSA::Key(0, 0);
	}


};


std::pair<RSA::Key, RSA::Key>  RSA::GenKey(int seed, BigInt p, BigInt q, BigInt e)
{
	printf("Eh\n");
	srand(seed);
	if ((p == 0) || (q == 0))
	{//1060 - amount of primes
		int prand = rand() % 1060;
		int qrand = rand() % 1060;
		while (qrand == prand) { qrand = rand() % 1060; }

		p = Primes[prand];
		q = Primes[qrand];
		printf("p:%s; q:%s\n", p.toString().c_str(), q.toString().c_str());
	}

	BigInt f = (p - 1)*(q - 1);//Euler's function
	if (e == 0)
	{
		int erand = rand() % 1060;
		while (Primes[erand] > f.toLongLong()) { erand = rand() % 1060; }
		e = Primes[erand];
		//Generate random 0<e<f
	}

	BigInt n = p*q;
	BigInt d = modInverse(e, f);

	return std::make_pair(Key(n, e), Key(n, d));
}