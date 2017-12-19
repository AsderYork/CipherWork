#pragma once
#include <fstream>
#include <string>

#include <cereal\cereal.hpp>
#include <cereal\archives\json.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include "Eleptic.h"

struct AlphabetPair
{
	ELeptic::Point point;
	std::string String;
	AlphabetPair(std::pair<ELeptic::Point, std::string> in) : point(in.first), String(in.second) {}
	AlphabetPair() : point(0,0,0) {};

	template <class Archive>
	void save(Archive & ar) const
	{
		ar(cereal::make_nvp("point_x", point.x.toUnsignedInt()),
			cereal::make_nvp("point_y", point.y.toUnsignedInt()),
			cereal::make_nvp("point_mod", point.mod.toUnsignedInt()),
			cereal::make_nvp("point_curveCoef", point.curveCoef.toInt()),
			cereal::make_nvp("letter", String));
	}

	template <class Archive>
	void load(Archive & ar)
	{
		int x, y, mod, cc;
		ar(cereal::make_nvp("point_x", x),
			cereal::make_nvp("point_y", y),
			cereal::make_nvp("point_mod", mod),
			cereal::make_nvp("point_curveCoef", cc),
			cereal::make_nvp("letter", String));
		point = ELeptic::Point(x, y, mod, cc);
	}


};


struct AlgoCryptoPair
{
	ELeptic::Point pointC;
	ELeptic::Point pointH;
	AlgoCryptoPair() : pointC(0, 0, 0), pointH(0, 0, 0) {};

	template <class Archive>
	void save(Archive & ar) const
	{
		ar(cereal::make_nvp("pointC_x", pointC.x.toUnsignedInt()),
			cereal::make_nvp("poinC_y", pointC.y.toUnsignedInt()),
			cereal::make_nvp("pointH_x", pointH.x.toUnsignedInt()),
			cereal::make_nvp("poinH_y", pointH.y.toUnsignedInt()));
	}

	template <class Archive>
	void load(Archive & ar)
	{
		unsigned int px, py, hx, hy;
		ar(cereal::make_nvp("pointC_x", px),
			cereal::make_nvp("poinC_y", py),
			cereal::make_nvp("pointH_x", hx),
			cereal::make_nvp("poinH_y", hy));
		pointC = ELeptic::Point(px,py,0);
		pointH = ELeptic::Point(hx, hy, 0);

	}


};


void CreateCurveTemplateIfNeeded(std::string Filename = std::string("Curve.json"))
{
	std::ifstream Test(Filename);
	if (!Test)
	{
		std::ofstream stream("Curve.json", std::ios_base::out);
		cereal::JSONOutputArchive Arch(stream);

		unsigned int a = 0, b = 0, mod = 0;
		Arch(cereal::make_nvp("curve_a", a), cereal::make_nvp("curve_b", b), cereal::make_nvp("curve_mod", mod));
	}
}

ELeptic::Curve RequetCurve(std::string Filename = std::string("Curve.json"))
{
	std::ifstream stream(Filename, std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);

	int a = 0, b = 0, mod = 0;
	Arch(cereal::make_nvp("curve_a", a), cereal::make_nvp("curve_b", b), cereal::make_nvp("curve_mod", mod));
	return ELeptic::Curve(a, b, mod);
}

void GenerateAlphabetTemplateIfNeeded(ELeptic::Curve& curve, std::string Filename = std::string("Alphabet.json"))
{
	std::ifstream Test(Filename);
	if (Test) { return; }
	std::ofstream stream(Filename, std::ios_base::out);
	cereal::JSONOutputArchive Arch(stream);

	auto Alph = curve.GenAlphabetBound();
	
	std::vector<AlphabetPair> toSave;
	for (auto& Letter : Alph)
	{
		toSave.emplace_back(Letter);
	}
	Arch(toSave);
	
}

std::vector<AlphabetPair> LoadAlphabet(std::string Filename = std::string("Alphabet.json"))
{
	std::ifstream stream(Filename, std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);
	
	std::vector<AlphabetPair> Result;

	Arch(Result);

	return Result;
}

std::vector<std::string> LoadMessage(std::string Filename = std::string("message.json"))
{
	std::wifstream stream(Filename, std::ios_base::out);

	std::vector<std::string> Result;

	std::wstring tmp;
	while (true)
	{
		std::getline(stream, tmp);
		for (int i =0; i< tmp.size(); i++)
		{
			auto& substr = tmp.substr(i, 1);
			Result.push_back(std::string(substr.begin(), substr.end()));
		}
		if (stream.eof()) { break; }
	}
	return Result;
}

void SaveMessage(std::vector<std::string> &in,std::string Filename = std::string("message.json"))
{
	std::ofstream stream(Filename, std::ios_base::out);
	
	std::wstring buf;
	for (auto& substr : in)
	{
		stream << substr;
		//buf += std::wstring(substr.begin(), substr.end());
	}

	//stream << buf;
}

std::vector<ELeptic::Point> AlgomalDecrypt(ELeptic::Curve& Cur, std::string Filename = std::string("AlgomalCrypt.json"))
{
	std::ifstream stream(Filename, std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);

	unsigned int a = 0, b = 0;
	Arch(cereal::make_nvp("GenPoint_x", a), cereal::make_nvp("GenPoint_y", b));
	ELeptic::AlgomalCrypto AlgoCrypt(Cur, Cur.GetPoint(a, b));

	std::ifstream Cypherstream(Filename, std::ios_base::in);
	cereal::JSONInputArchive Cipherar(Cypherstream);

	std::vector<AlgoCryptoPair> Crypt;

	Cipherar(Crypt);

	std::vector<std::pair<ELeptic::Point, ELeptic::Point>> PointVec;
	for (auto& C : Crypt)
	{
		ELeptic::Point Cr = Cur.GetPoint(C.pointC.x.toUnsignedInt(), C.pointC.y.toUnsignedInt());

		ELeptic::Point Hr = Cur.GetPoint(C.pointH.x.toUnsignedInt(), C.pointH.y.toUnsignedInt());


		PointVec.push_back(std::make_pair(Cr, Hr));
	}

	return AlgoCrypt.Decipher(PointVec);	
}

std::vector<ELeptic::Point> ProcessMessage(std::vector<AlphabetPair> &Alph, std::vector<std::string> Msg)
{
	std::vector<ELeptic::Point> Result;
	for (auto& letter : Msg)
	{
		for (auto Sear : Alph)
		{
			if (letter == Sear.String)
			{
				Result.push_back(Sear.point);
				break;
			}
		}
	}
	return Result;
}

void MakeAlgomalTemplateIfNeeded(std::string File="Algomal.json")
{
	std::ifstream stream(File, std::ios_base::in);
	if (stream) { return; }

	std::ofstream outstr(File, std::ios_base::out);
	cereal::JSONOutputArchive OutArch(outstr);

	unsigned int Keyx=10, Keyy=12;
	OutArch(cereal::make_nvp("PubKey_x", Keyx), cereal::make_nvp("PubKey_y", Keyy));


	unsigned int gpx=0, gpy=1;
	OutArch(cereal::make_nvp("Genpoint_x", gpx), cereal::make_nvp("Genpoint_y", gpy));

	std::vector<unsigned int> MaskVals;
	MaskVals.push_back(1);
	MaskVals.push_back(2);
	MaskVals.push_back(3);
	OutArch(cereal::make_nvp("MaskVals", MaskVals));
}

void AlgomalEncrypt(std::vector<AlphabetPair> Alph, ELeptic::Curve &Cur, std::string MessageFile, std::string ParamsFile, std::string OutFile) {
	auto Msg = LoadMessage(MessageFile);

	std::ifstream stream(ParamsFile, std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);

	unsigned int Keyx, Keyy;
	Arch(cereal::make_nvp("PubKey_x", Keyx), cereal::make_nvp("PubKey_y", Keyy));


	unsigned int gpx, gpy;
	Arch(cereal::make_nvp("Genpoint_x", gpx), cereal::make_nvp("Genpoint_y", gpy));

	std::vector<unsigned int> MaskVals;
	Arch(cereal::make_nvp("MaskVals", MaskVals));

	auto ProcessedMessage = ProcessMessage(Alph, Msg);

	ELeptic::AlgomalCrypto AlgoCrypto(Cur, Cur.GetPoint(gpx, gpy));

	auto Result = AlgoCrypto.Cipher(ProcessedMessage, Cur.GetPoint(Keyx, Keyy), MaskVals);

	std::vector<AlgoCryptoPair> Pairs;

	for (auto& R : Result)
	{
		AlgoCryptoPair Pair;
		Pair.pointC.x = R.first.x;
		Pair.pointC.y = R.first.y;


		Pair.pointH.x = R.second.x;
		Pair.pointH.y = R.second.y;

		Pairs.push_back(Pair);
	}

	std::ofstream outstr(OutFile, std::ios_base::out);
	cereal::JSONOutputArchive OutArch(outstr);

	OutArch(Pairs);
}

void FileInterface()
{
	CreateCurveTemplateIfNeeded();
	auto Curve = RequetCurve();
	GenerateAlphabetTemplateIfNeeded(Curve);
	auto Alph = LoadAlphabet();

	MakeAlgomalTemplateIfNeeded();

	std::vector<ELeptic::Point> AlgomalBuf;

	while (true)
	{
		printf("1)reload curve\n");
		printf("2)make alphabet\n");
		printf("3)reload alphabet\n");
		printf("4)AlgomalDecrypt\n");
		printf("5)AlgomalEncrypt\n");

		printf("x)exit\n");

		char ch;
		ch = getchar();
		switch (ch)
		{
		case'1': {
			printf("filename:");
			std::string fname;
			std::cin >> fname;
			try {
				Curve = RequetCurve(fname);
			}
			catch (...)
			{
				printf("Can't do\n");
			}

			break;
		}
		case'2': {
			printf("filename:");
			std::string fname;
			std::cin >> fname;
			try {
				GenerateAlphabetTemplateIfNeeded(Curve, fname);
			}
			catch (...)
			{
				printf("Can't do\n");
			}
			break;
		}
		case'3': {
			printf("filename:");
			std::string fname;
			std::cin >> fname;
			try {
				Alph = LoadAlphabet(fname);
			}
			catch (...)
			{
				printf("Can't do\n");
			}
			break;
		}
		case'4': {
			printf("filename:");
			std::string fname;
			std::cin >> fname;
			try {
				AlgomalBuf = AlgomalDecrypt(Curve, fname);
			}
			catch (...)
			{
				printf("Can't do\n");
			}
			break;
		}
		case'5': {
			printf("MessageFile:");
			std::string mfname;
			std::cin >> mfname;

			printf("ParamFile:");
			std::string pfname;
			std::cin >> pfname;

			printf("OutFile:");
			std::string Outfile;
			std::cin >> Outfile;
			try {
				AlgomalEncrypt(Alph, Curve, mfname, pfname, Outfile);
			}
			catch (...)
			{
				printf("Can't do\n");
			}
			break;
		}

		case'X':
		case'x': {
			return;
		}

		default:
		{break; }
		}

	}

	
}
