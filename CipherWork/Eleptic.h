#pragma once

#include "MathHelper.h"
#include <algorithm>
#include <string>
#include <algorithm>

class ELeptic
{
public:
	struct Curve;
	struct Point;

	struct Point {
		BigInt x = 0, y = 0, mod = 0, curveCoef = 0;
		Point(BigInt _x, BigInt _y, BigInt _mod) : x(_x), y(_y), mod(_mod) {}
		Point(BigInt _x, BigInt _y, BigInt _mod, BigInt _cur) : x(_x), y(_y), mod(_mod), curveCoef(_cur) {}

		std::string ToStr()
		{
			return "<" + x.toString() + ", " + y.toString() + ">";
		}
		BigInt norm(BigInt Val) {
			while (Val < 0) { Val += mod; }
			return Val % mod;
		}

		bool operator==(Point& other)
		{
			if ((x == other.x) && (y == other.y)) { return true; }
			return false;
		}
		Point operator+(Point& other)
		{
			if ((other.x == 0) && (other.y == 0)) { return *this; }
			else if ((other.x == x) && (other.y == norm(-y))) { return Point(0, 0, mod, curveCoef); }

			BigInt Alpha;
			if ((x == other.x) && (y == other.y))
			{
				Alpha = ((x*x) * 3 + curveCoef) * modInverse(y * 2, mod);
			}
			else
			{
				//printf("y-y:%i\n", (other.y - y).toInt());
				//printf("x-x%i\n", norm(other.x - x).toInt());
				//printf("modinv:%i\n", modInverse(norm(other.x - x), mod).toInt());
				Alpha = (other.y - y) * modInverse(norm(other.x - x), mod);
			}
			//printf("PreAlpha:%i\n", Alpha.toInt());
			Alpha = norm(Alpha);
			//printf("Alpha:%i\n", Alpha.toInt());

			BigInt RetX = modExp(Alpha, 2, mod) - x - other.x;
			BigInt RetY = Alpha*(x - RetX) - y;



			auto RetPoint = Point(norm(RetX), norm(RetY), mod, curveCoef);

			return RetPoint;
		}
		Point operator-(Point& other)
		{
			return *this + other.inv();
		}
		Point operator*(unsigned int Mul)
		{
			Point Tmp(0, 0, mod, curveCoef);

			Point Acc = *this;
			while (Mul != 0)
			{

				if (Mul % 2 == 1)
				{
					Tmp = Acc + Tmp;
				}
				Acc = Acc + Acc;
				Mul = Mul / 2;
				//printf("Tmp<%s; %s> Acc<%s; %s>\n", Tmp.x.toString().c_str(), Tmp.y.toString().c_str(), Acc.x.toString().c_str(), Acc.y.toString().c_str());
			}
			return Tmp;
		}


		Point inv() { return Point(x, norm(-y), mod, curveCoef); }

		bool Check(BigInt a, BigInt b) {
			return norm(modExp(y, 2, mod)) == norm(modExp(x, 3, mod) + a*x + b);
		}

	};

	struct Curve
	{
		Curve(unsigned int a, unsigned int b, unsigned int mod) : m_a(a), m_b(b), m_mod(mod) {
			GenAlphabet();
		};
		Point GetPoint(unsigned int x, unsigned int y) {
			auto Ret = Point(x, y, m_mod, m_a);
			Ret.x = Ret.norm(Ret.x);
			Ret.y = Ret.norm(Ret.y);
			return Ret;
		}

		bool Check(Point& p)
		{
			if ((p.mod != m_mod) || (p.curveCoef != m_a)) { return false; }
			return p.Check(m_a, m_b);
		}

		bool CheckHossey()
		{
			auto Alph = GenAlphabet();

			auto LowEnd = m_mod + 1 - 2 * ((unsigned int)floor(sqrt(m_mod)));
			auto HighEnd = m_mod + 1 + 2 * ((unsigned int)ceil(sqrt(m_mod)));

			if ((Alph.size() > LowEnd) && (Alph.size() < HighEnd))
			{
				return true;
			}
			return false;
		}



		std::vector<Point> GenAlphabet()
		{
			std::vector<std::pair<unsigned int, unsigned int>> ys;
			for (int i = 0; i <= m_mod / 2; i++)
			{
				ys.push_back(std::make_pair(i, (i*i) % m_mod));
			}
			std::vector<Point> Result;

			for (int xs = 0; xs < m_mod; xs++)
			{
				auto Num = ((xs*xs*xs) % m_mod + m_a*xs + m_b) % m_mod;
				//printf("x:%i->%i\n", xs, Num);
				for (auto& y : ys)
				{
					if (y.second == Num)
					{
						//printf("%i->%i\n", xs, y.first);
						Result.push_back(this->GetPoint(xs, y.first));
						Result.push_back(this->GetPoint(xs, m_mod - y.first));
						break;
					}
				}
			}
			Alph = Result;
			return Alph;
		}

		std::vector<std::pair<Point, std::string>> GenAlphabetBound(std::string Alphabet)
		{
			std::vector<std::pair<Point, std::string>> Ret;
			if (Alph.size() < Alphabet.size())
			{
				printf("Size is too small!");
				return Ret;
			}

			for (int i = 0; i < Alphabet.size(); i++)
			{
				Ret.push_back(std::make_pair(Alph[i], Alphabet.substr(i, 1)));
			}


		}
		std::vector<std::pair<Point, std::string>> GenAlphabetBound(std::vector<std::string> Alphabet)
		{
			std::vector<std::pair<Point, std::string>> Ret;
			if (Alph.size() < Alphabet.size())
			{
				printf("Size is too small!");
				return Ret;
			}

			for (int i = 0; i < Alphabet.size(); i++)
			{
				Ret.push_back(std::make_pair(Alph[i], Alphabet[i]));
			}


		}
		std::vector<std::pair<Point, std::string>> GenAlphabetBound()
		{
			std::vector<std::pair<Point, std::string>> Ret;

			for (int i = 0; i < Alph.size(); i++)
			{
				Ret.push_back(std::make_pair(Alph[i], std::to_string(i)));
			}

			return Ret;
		}

		std::pair<Point, unsigned int> GeneratePrivate(Point G)
		{
			auto Alph = GenAlphabet();
			unsigned int S = rand() % 4096;

			return  std::make_pair(G*S, S);

		}

		Point GenShared(Point Public, unsigned int Priv)
		{
			return Public*Priv;

		}

		std::vector<Point> Cipher(std::vector<Point> Input, Point Shared)
		{
			std::vector<Point> Result;
			auto even = Shared.y.toInt();
			auto odd = Shared.x.toInt();

			for (int i = 0; i < Input.size(); i++)
			{
				if (i % 2 == 0)
				{
					Result.push_back(Input[i] * odd);
				}
				else
				{
					Result.push_back(Input[i] * even);

				}
			}
			return Result;
		}

		std::vector<Point> Decipher(std::vector<Point> Input, Point Shared)
		{
			std::vector<Point> Result;
			auto even = modInverse(Shared.y.toInt(), Alph.size() + 1).toInt();
			auto odd = modInverse(Shared.x.toInt(), Alph.size() + 1).toInt();

			for (int i = 0; i < Input.size(); i++)
			{
				if (i % 2 == 0)
				{
					Result.push_back(Input[i] * odd);
				}
				else
				{
					Result.push_back(Input[i] * even);

				}
			}
			return Result;
		}

		unsigned int m_a, m_b, m_mod;
		std::vector<Point> Alph;
	};

	struct AlgomalCrypto
	{
		Curve Cur;
		Point GenPoint;
		unsigned int RandKey;

		Point Priv;

		std::vector<std::pair<Point, std::string>> Alph;
		std::vector<Point> Mask;

		AlgomalCrypto(Curve _Cur, Point _Gen) : Cur(_Cur), GenPoint(_Gen), Priv(0, 0, 0) {
			RandKey = rand() % 10000;
			Priv = _Gen * RandKey;
		}
		AlgomalCrypto(Curve _Cur, Point _Gen, unsigned int Key) : Cur(_Cur), GenPoint(_Gen), Priv(0, 0, 0) {
			RandKey = Key;
			Priv = _Gen * RandKey;
		}

		void LoadAlphabet(std::vector<std::pair<Point, std::string>> _Alph)
		{
			Alph = _Alph;
		}
		void LoadMask(std::vector<Point> _Mask)
		{
			Mask = _Mask;
		}

		void GenMask(Point OtherPriv)
		{
			for (auto& Letter : Alph)
			{
				Mask.push_back(OtherPriv * (rand() % 10000));
			}
		};

		std::vector<std::pair<Point, Point>> Cipher(std::vector<Point> Plaintext,
			Point OtherPriv,
			std::vector<unsigned int> Mask = std::vector<unsigned int>())
		{
			bool GenMask = false;
			if (Mask.size() == 0)
			{
				GenMask = true;
			}

			std::vector<std::pair<Point, Point>> Result;
			for (int i = 0; i < Plaintext.size(); i++)
			{
				unsigned int Rand;
				if (GenMask)
				{
					Rand = rand() % 1000;
				}
				Rand = Mask[i];

				auto LetterMask = OtherPriv * Rand;
				auto LetterHelp = GenPoint * Rand;
				auto LetterCipher = Plaintext[i] + LetterMask;

				Result.push_back(std::make_pair(LetterCipher, LetterHelp));
			}
			return Result;
		}

		std::vector<Point> Decipher(std::vector<std::pair<Point, Point>> Ciphertext)
		{
			std::vector<Point> Result;

			for (auto& Cipher : Ciphertext)
			{
				Result.push_back(Cipher.first - (Priv*RandKey));
			}
			return Result;
		}

	};


};