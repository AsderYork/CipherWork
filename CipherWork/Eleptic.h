#pragma once

#include "MathHelper.h"

class ELeptic
{
public:
	struct Point{ BigInt x = 0, y = 0, mod =0, curveCoef=0;
	Point(BigInt _x, BigInt _y, BigInt _mod) : x(_x), y(_y), mod(_mod) {}
	Point(BigInt _x, BigInt _y, BigInt _mod, BigInt _cur) : x(_x), y(_y), mod(_mod), curveCoef(_cur){}

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
		if ((x==other.x)&&(y==other.y))
		{
			Alpha = ((x*x) * 3 + curveCoef) * modInverse(y * 2,mod);
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
			Mul = Mul/2;
			//printf("Tmp<%s; %s> Acc<%s; %s>\n", Tmp.x.toString().c_str(), Tmp.y.toString().c_str(), Acc.x.toString().c_str(), Acc.y.toString().c_str());
		}
		return Tmp;
	}


	Point inv() { return Point(x, norm(-y), mod, curveCoef); }

	bool Check(BigInt a, BigInt b) {
		return norm(modExp(y, 2, mod)) == norm(modExp(x,3,mod) + a*x + b);
	}

	};



};