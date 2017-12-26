#pragma once
#include "Eleptic.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <optional>

#include <cereal\cereal.hpp>
#include <cereal\archives\json.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>

namespace Eleptic
{
	class Signatures
	{
	public:
		static std::pair<int,int> GenerateSignatureFromFile(std::string Filename)
		{
			std::ifstream stream(Filename, std::ios_base::in);
			cereal::JSONInputArchive Arch(stream);
			
			int Curve_a=0, Curve_b=0, GenpointX=0, GenpointY=0, HashVal=0, SecretKey=0, RandomNum=0;
			unsigned int Curve_mod=0, PointOrder=0;

			Arch(cereal::make_nvp("Curve_a", Curve_a),
				cereal::make_nvp("Curve_b", Curve_b),
				cereal::make_nvp("Curve_mod", Curve_mod),
				cereal::make_nvp("GenpointX", GenpointX),
				cereal::make_nvp("GenpointY", GenpointY),
				cereal::make_nvp("PointOrder", PointOrder),
				cereal::make_nvp("HashVal", HashVal),
				cereal::make_nvp("SecretKey", SecretKey),
				cereal::make_nvp("RandomNum", RandomNum)
				);

			Curve tmpCurve(Curve_a, Curve_b, Curve_mod);
			auto Genpoint = tmpCurve.GetPoint(GenpointX, GenpointY);

			auto kG = Genpoint*RandomNum;
			auto r = kG.x % PointOrder;
			auto z = modInverse(RandomNum, PointOrder);
			auto s = z*(HashVal + r.toInt()*SecretKey) % PointOrder;

			return std::make_pair(r.toInt(), s.toInt());

		}

		static bool CheckSignatureFromFile(std::string Filename)
		{
			std::ifstream stream(Filename, std::ios_base::in);
			cereal::JSONInputArchive Arch(stream);

			int Curve_a = 0, Curve_b = 0, GenpointX = 0, GenpointY = 0, HashVal = 0;
			unsigned int Curve_mod = 0, PointOrder = 0;
			int OpenQX = 0, OpenQY = 0, SigR = 0, SigS = 0;

			Arch(cereal::make_nvp("Curve_a", Curve_a),
				cereal::make_nvp("Curve_b", Curve_b),
				cereal::make_nvp("Curve_mod", Curve_mod),
				cereal::make_nvp("GenpointX", GenpointX),
				cereal::make_nvp("GenpointY", GenpointY),
				cereal::make_nvp("PointOrder", PointOrder),
				cereal::make_nvp("HashVal", HashVal),
				cereal::make_nvp("OpenQX", OpenQX),
				cereal::make_nvp("OpenQY", OpenQY),
				cereal::make_nvp("SigR", SigR),
				cereal::make_nvp("SigS", SigS)
			);

			Curve tmpCurve(Curve_a, Curve_b, Curve_mod);
			auto Genpoint = tmpCurve.GetPoint(GenpointX, GenpointY);
			auto Q = tmpCurve.GetPoint(OpenQX, OpenQY);

			if ((SigR < 1) || (SigR > PointOrder - 1)) { return false; }
			if ((SigS < 1) || (SigS > PointOrder - 1)) { return false; }

			auto v = modInverse(SigS, PointOrder).toInt();
			auto u1 = (HashVal*v) % PointOrder;
			auto u2 = (SigR*v) % PointOrder;

			auto X = Genpoint*u1 + Q*u2;
			auto Xx = X.x % PointOrder;

			if (Xx == SigR) { return true; }
			return false;
		}

		static void SignaturesRoutine()
		{
			char ch;
			while (true)
			{
				printf("Signatures routine. You can either [G]enerate or [C]heck a signature. Or [X]leave\n");
				ch = getchar();
				switch (ch)
				{
				case 'G':
				case 'g': {
					printf("'Signatures_gen.json' will be used\n");
					std::string filename;
					filename = "Signatures_gen.json";
					auto Signature = GenerateSignatureFromFile(filename);
					printf("Signature:<%i;%i>\n", Signature.first, Signature.second);
					break;
				}

				case 'C':
				case 'c': {
					printf("'Signatures_chek.json' will be used\n");
					std::string filename;
					filename = "Signatures_chek.json";
					if(CheckSignatureFromFile(filename))
					{printf("Signature is correct\n");	}
					else
					{printf("Signature is incorrect!\n");}
					break;
				}

				case 'x':
				case 'X': {
					return;
				}
				
				default: {break; }
				}
			}
		}
	};
}