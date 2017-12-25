#pragma once
#include "Eleptic.h"
#include <algorithm>

namespace Eleptic
{
	class Messi_AmuroCrypto
	{
	private:
		Curve& m_curve;
		unsigned int m_value;
		unsigned int m_InvValue;
	public:
		Messi_AmuroCrypto(Curve& curve, unsigned int value) : m_curve(curve), m_value(value),
			m_InvValue(modInverse(value, m_curve.Alph.size() + 1).toUnsignedInt())
			//m_InvValue(modInverse(value, m_curve.m_mod).toUnsignedInt())
		{}

		static bool CheckValue(Curve& curve, unsigned int value)
		{
			BigInt x, y;
			return gcd((curve.Alph.size() + 1), value, x, y) == 1;
		}
		static unsigned int getRandomCorrectValue(Curve& curve)
		{
			unsigned int Value;
			do {
				Value = ((rand() + 1) % curve.Alph.size());
			} while (!CheckValue(curve, Value));
			return Value;
		}

		/**!
		This algorithm has 4 steps.
		1)Sender StrightBlend's message and send it to a reciver
		2)Reciver StraightBlend's message and send it back
		3)Sender InverseBlend's message and send it again
		4)And then reciver finally InverseBlend's it, that gives him plaintext.
		*/
		std::vector<Eleptic::Point> StrightBlend(std::vector<Eleptic::Point> Points)
		{
			std::for_each(Points.begin(), Points.end(), [this](Point& P) {P = P*m_value; });
			return Points;
		}

		std::vector<Eleptic::Point> InverseBlend(std::vector<Eleptic::Point> Points)
		{		
			std::for_each(Points.begin(), Points.end(), [this](Point& P) {P = P*m_InvValue; });
			return Points;
		}

	};

}