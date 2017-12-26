#pragma once
#include <fstream>
#include <string>
#include <algorithm>
#include <optional>

#include <cereal\cereal.hpp>
#include <cereal\archives\json.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
#include "Eleptic.h"

#include "Alphabet_Helper_structures.h"
#include "Messi_Amuro_FileHelper.h"
#include "Signatures.h"


struct AlgoCryptoPair
{
	Eleptic::Point pointC;
	Eleptic::Point pointH;
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
		pointC = Eleptic::Point(px,py,0);
		pointH = Eleptic::Point(hx, hy, 0);

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

Eleptic::Curve RequetCurve(std::string Filename = std::string("Curve.json"))
{
	std::ifstream stream(Filename, std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);

	int a = 0, b = 0, mod = 0;
	Arch(cereal::make_nvp("curve_a", a), cereal::make_nvp("curve_b", b), cereal::make_nvp("curve_mod", mod));
	return Eleptic::Curve(a, b, mod);
}

void GenerateAlphabetTemplateIfNeeded(Eleptic::Curve& curve, std::string Filename = std::string("Alphabet.json"))
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
	Arch(cereal::make_nvp("Alphabet",toSave));

}

std::vector<AlphabetPair> LoadAlphabet(std::string Filename = std::string("Alphabet.json"))
{
	std::ifstream stream(Filename, std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);
	
	std::vector<AlphabetPair> Result;

	Arch(cereal::make_nvp("Alphabet", Result));

	return Result;
}

std::vector<Eleptic::Point> AlgomalDecrypt(Eleptic::Curve& Cur, std::string ParamsFile, std::string CiphertextFile)
{
	std::ifstream stream(ParamsFile, std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);

	int gpx = 0, gpy = 0, RandKey = 0;
	Arch(cereal::make_nvp("Genpoint_x", gpx), cereal::make_nvp("Genpoint_y", gpy), cereal::make_nvp("RandKey", RandKey));
	Eleptic::AlgomalCrypto AlgoCrypt(Cur, Cur.GetPoint(gpx, gpy), RandKey);

	std::ifstream Cypherstream(CiphertextFile, std::ios_base::in);
	cereal::JSONInputArchive Cipherar(Cypherstream);

	std::vector<AlgoCryptoPair> Crypt;

	Cipherar(Crypt);

	std::vector<std::pair<Eleptic::Point, Eleptic::Point>> PointVec;
	for (auto& C : Crypt)
	{
		Eleptic::Point Cr = Cur.GetPoint(C.pointC.x.toUnsignedInt(), C.pointC.y.toUnsignedInt());

		Eleptic::Point Hr = Cur.GetPoint(C.pointH.x.toUnsignedInt(), C.pointH.y.toUnsignedInt());


		PointVec.push_back(std::make_pair(Cr, Hr));
	}

	return AlgoCrypt.Decipher(PointVec);	
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

void AlgomalEncrypt(std::vector<AlphabetPair> Alph, Eleptic::Curve &Cur, std::string MessageFile, std::string ParamsFile, std::string OutFile) {
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

	Eleptic::AlgomalCrypto AlgoCrypto(Cur, Cur.GetPoint(gpx, gpy));

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




template<typename T>
T LoadValueFromDHPrivate(const char* ValueName)
{
	try {
		std::ifstream stream("dh_private.json", std::ios_base::in);
		cereal::JSONInputArchive Arch(stream);

		T Value;

		Arch(cereal::make_nvp(ValueName, Value));
		return Value;
	}
	catch (...)
	{
	}
	return T();
}

template<>
std::optional<int> LoadValueFromDHPrivate(const char* ValueName)
{
	try {
		std::ifstream stream("dh_private.json", std::ios_base::in);
		cereal::JSONInputArchive Arch(stream);

		int PrivateValue = 0;

		Arch(cereal::make_nvp(ValueName, PrivateValue));
		return std::make_optional(PrivateValue);
	}
	catch(...)
	{ }
	return std::nullopt;
}

void DiffieHellmanGenerativeRoutine(Eleptic::Curve &Curve, std::vector<AlphabetPair> &Alphabet)
{
	printf("Alphabet and curve must be loaded allready\n");
	printf("Then it's time to load genpoint from dh_private.json\n");

	std::ifstream stream("dh_private.json", std::ios_base::in);
	cereal::JSONInputArchive Arch(stream);
	int Genpoint_x = 0;
	int Genpoint_y = 0;
	Arch(cereal::make_nvp("genpoint_x", Genpoint_x), cereal::make_nvp("genpoint_y", Genpoint_y));

	
	Eleptic::Point Genpoint = Curve.GetPoint(Genpoint_x, Genpoint_y);
		
	printf("Point %s were set as genpoint\n", Genpoint.ToStr().c_str());

	printf("Now it's time for private value. If possible, the value will be loaded from dh_private.json\n");

	auto MaybePrivateValue = LoadValueFromDHPrivate<std::optional<int>>("PrivateValue");

	int PrivateValue;
	if (!MaybePrivateValue)
	{
		printf("Value wasn't loaded, for some reason. A random value from [1; 10000]/0 will be chosen\n");
		PrivateValue = (rand() % 9999) + 1;
	}
	else {
		PrivateValue = *MaybePrivateValue;
		printf("Value was loaded successfully\n");
	}

	auto PublicPoint = Genpoint*PrivateValue; 
	printf("So point %s is a public key.\n", PublicPoint.ToStr().c_str());
	printf("This point, along with Curve parameters and alphabet, will be saved in file, specified in dh_private.json\n");


	printf("And now the time has come to do some shared stuff!\n", PublicPoint.ToStr().c_str());
	auto SaveFilename = LoadValueFromDHPrivate<std::string>("File_to_save_results");
	if (SaveFilename.size() == 0)
	{
		printf("Well, it havn't specify the place. You have to do it yourself!\n");
		std::cin >> SaveFilename;
	}

	printf("So results will be saved in %s!\n", SaveFilename.c_str());

	std::ofstream outstr(SaveFilename, std::ios_base::out);
	cereal::JSONOutputArchive OutArch(outstr);

	auto PreparedAlphabetVector = AlphabetPair::ToSubpointVec(Alphabet);
	
	OutArch(cereal::make_nvp("Curve_a", Curve.m_a),
		cereal::make_nvp("Curve_a", Curve.m_b),
		cereal::make_nvp("Curve_mod", Curve.m_mod),
		cereal::make_nvp("genpoint_x", Genpoint.x.toInt()),
		cereal::make_nvp("genpoint_y", Genpoint.y.toInt()),
		cereal::make_nvp("Public_point_x", PublicPoint.x.toInt()),
		cereal::make_nvp("Public_point_y", PublicPoint.y.toInt()),
		cereal::make_nvp("AlphabetSize", PreparedAlphabetVector.size()),
		cereal::make_nvp("Alphabet", PreparedAlphabetVector));

}

void DiffieHellmanDecipheringRoutine()
{
	printf("So, a deciphering routine. A cipher-data will be loaded from file, specified in dh_private.json\n");
	auto CipherDataFileName = LoadValueFromDHPrivate<std::string>("CipherData_Filename");
	if (CipherDataFileName.size() == 0)
	{
		printf("No file were specified in a config. So specify it now, here\n");
		std::cin >> CipherDataFileName;
	}

	printf("Ciphering Data will be loaded from %s\n", CipherDataFileName.c_str());

	std::ifstream outstr(CipherDataFileName, std::ios_base::in);
	cereal::JSONInputArchive OutArch(outstr);

	int Curve_a=0, Curve_b=0, PP_x=0, PP_y=0;
	unsigned int Curve_mod = 0;
	std::vector<SubPointAndLetter> PackedAlphabet;
	OutArch(cereal::make_nvp("Curve_a", Curve_a),
		cereal::make_nvp("Curve_a", Curve_b),
		cereal::make_nvp("Curve_mod", Curve_mod),
		cereal::make_nvp("Public_point_x", PP_x),
		cereal::make_nvp("Public_point_y", PP_y),
		cereal::make_nvp("Alphabet", PackedAlphabet)
	);

	auto Curve = Eleptic::Curve(Curve_a, Curve_b, Curve_mod);
	auto Alphabet = AlphabetPair::FromSubPointVec(PackedAlphabet, Curve);

	auto PublicKey = Curve.GetPoint(PP_x, PP_y);

	printf("The data was loaded successfully. The time to load a private value has come\n");
	auto MaybePrivateValue = LoadValueFromDHPrivate<std::optional<int>>("PrivateValue");
	int PrivateValue = 0;
	if (!MaybePrivateValue)
	{
		printf("So there was no private value. Add 'PrivateValue' filed in dh_private.json and repeat\n");
		return;
	}
	else {
		PrivateValue = (*MaybePrivateValue);
	}

	printf("Private value were loaded. Now it's time to generate shared secret\n");
	auto SharedSecret = PublicKey;
	if (PrivateValue < 0) { SharedSecret = SharedSecret.inv()*PrivateValue; }
	else { SharedSecret = SharedSecret*PrivateValue; }

	printf("Shared secret have been generated!\n");

	printf("!!!DEBUG!!! SharedSecret:%s\n", SharedSecret.ToStr().c_str());


	printf("Time to decipher has come! loaded(Not default) alphabet will be used to decipher text from file, specified in dh_private.json\n");

	auto CipheredTextFilename = LoadValueFromDHPrivate<std::string>("ciphered_text_filename");
	if (CipheredTextFilename.size() == 0)
	{
		printf("Too bad! No file were specified by key 'ciphered_text_filename'. You must insert filename by yourslef\n");
		std::cin >> CipheredTextFilename;
	}

	printf("File %s will be loaded and deciphered!\n", CipheredTextFilename.c_str());

	auto Message = LoadMessage(CipheredTextFilename);

	if (Message.size() == 0)
	{
		printf("Well. The file was empty. Fill it with something that can be encrypted. Be careful and stay in the alphabet!\n");
		return;
	}

	printf("Now the message will parsed with the alphabet!\n");

	auto ProcessedMessage = ProcessMessage(Alphabet, Message);

	if (Message.size() != ProcessedMessage.size())
	{
		printf("Some(or all) letters in message do not exist in alphabet being used. Fix it.\n");
		return;
	}

	printf("Finally, time to decipher!\n");

	auto DecipheredMsg = Curve.Decipher(ProcessedMessage, SharedSecret);

	printf("Deciphered. Translate back to strings\n");

	auto CipheredPreparedMessage = FromPointsToString(Alphabet, DecipheredMsg);

	printf("Done! No let's just save it in 'File_to_save_deciphered' specified in dh_private.json\n");

	auto CipherFileName = LoadValueFromDHPrivate<std::string>("File_to_save_deciphered");
	if (CipherFileName.size() == 0)
	{
		printf("Too bad! No file were specified by key 'File_to_save_deciphered'. You must insert filename by yourslef\n");
		std::cin >> CipherFileName;
	}

	SaveMessage(CipheredPreparedMessage, CipherFileName);

	printf("Done!\n");
	
}

void DiffieHellmanCipheringRoutine(Eleptic::Curve &Curve, std::vector<AlphabetPair> &Alphabet)
{
	printf("So, a ciphering routine. A public key will be loaded from file, specified in dh_private.json\n");
	auto CipherDataFileName = LoadValueFromDHPrivate<std::string>("CipherData_Filename");
	if (CipherDataFileName.size() == 0)
	{
		printf("No file were specified in a config. So specify it now, here\n");
		std::cin >> CipherDataFileName;
	}

	printf("Public key Data will be loaded from %s\n", CipherDataFileName.c_str());

	std::ifstream outstr(CipherDataFileName, std::ios_base::in);
	cereal::JSONInputArchive OutArch(outstr);

	int Curve_a = 0, Curve_b = 0, PP_x = 0, PP_y = 0;
	unsigned int Curve_mod = 0;
	OutArch(cereal::make_nvp("Curve_a", Curve_a),
		cereal::make_nvp("Curve_a", Curve_b),
		cereal::make_nvp("Curve_mod", Curve_mod),
		cereal::make_nvp("Public_point_x", PP_x),
		cereal::make_nvp("Public_point_y", PP_y)
	);

	auto OtherCurve = Eleptic::Curve(Curve_a, Curve_b, Curve_mod);
	auto PublicKey = OtherCurve.GetPoint(PP_x, PP_y);

	printf("The data was loaded successfully\n");

	printf("Then the private value will be loaded from dh_private.json\n");

	auto MaybePrivateValue = LoadValueFromDHPrivate<std::optional<int>>("PrivateValue");
	int PrivateValue = 0;
	if (!MaybePrivateValue)
	{
		printf("So there was no private value. Add 'PrivateValue' filed in dh_private.json and repeat\n");
		return;
	}
	else {
		PrivateValue = (*MaybePrivateValue);
	}

	printf("Private value were loaded. Now it's time to generate shared secret\n");
	auto SharedSecret = PublicKey;
	if (PrivateValue < 0) { SharedSecret = SharedSecret.inv()*PrivateValue; }
	else { SharedSecret = SharedSecret*PrivateValue; }

	printf("Shared secret have been generated!\n");

	printf("!!!DEBUG!!! SharedSecret:%s\n", SharedSecret.ToStr().c_str());

	printf("Now it's time to cipher something with it! Loaded alphabet will be used to cipher text from file, specified in dh_private.json\n");

	auto PlaintextFilename = LoadValueFromDHPrivate<std::string>("File_with_text_to_cipher");
	if (PlaintextFilename.size() == 0)
	{
		printf("Too bad! No file were specified by key 'File_with_text_to_cipher'. You must insert filename by yourslef\n");
		std::cin >> PlaintextFilename;
	}
	
	printf("File %s will be loaded and ciphered!\n", PlaintextFilename.c_str());

	auto Message = LoadMessage(PlaintextFilename);

	if (Message.size() == 0)
	{
		printf("Well. The file was empty. Fill it with something that can be encrypted. Be careful and stay in the alphabet!\n");
		return;
	}
	
	printf("Now the message will parsed with the alphabet!\n");

	auto ProcessedMessage = ProcessMessage(Alphabet, Message);

	if (Message.size() != ProcessedMessage.size())
	{
		printf("Some(or all) letters in message do not exist in alphabet being used. Fix it.\n");
		return;
	}

	printf("Finally, time to cipher!\n");

	auto CipheredMessage = Curve.Cipher(ProcessedMessage, SharedSecret);

	printf("Ciphered. Translate back to strings\n");

	auto CipheredPreparedMessage = FromPointsToString(Alphabet, CipheredMessage);
	
	printf("Done! No let's just save it in 'ciphered_text_filename' specified in dh_private.json\n");

	auto CipherFileName = LoadValueFromDHPrivate<std::string>("ciphered_text_filename");
	if (CipherFileName.size() == 0)
	{
		printf("Too bad! No file were specified by key 'ciphered_text_filename'. You must insert filename by yourslef\n");
		std::cin >> CipherFileName;
	}

	SaveMessage(CipheredPreparedMessage, CipherFileName);

	printf("Done!\n");
}

void FileInterface()
{
	CreateCurveTemplateIfNeeded();
	auto Curve = RequetCurve();
	GenerateAlphabetTemplateIfNeeded(Curve);
	auto Alph = LoadAlphabet();

	MakeAlgomalTemplateIfNeeded();

	std::vector<Eleptic::Point> AlgomalBuf;

	while (true)
	{
		printf("1)reload curve\n");
		printf("2)make alphabet\n");
		printf("3)reload alphabet\n");
		printf("4)AlgomalDecrypt\n");
		printf("5)AlgomalEncrypt\n");
		printf("6)Diffie-Hellman generative routine\n");
		printf("7)Diffie-Hellman deciphering routine\n");
		printf("8)Diffie-Hellman ciphering routine\n");
		printf("9)Messi-Amuro routine\n");
		printf("S)Signatures routine\n");

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
			printf("Params filename:");
			std::string Paramfname;
			std::cin >> Paramfname;
			printf("Ciphertext filename:");
			std::string cfname;
			std::cin >> cfname;

			printf("Plaintext filename:");
			std::string pfname;
			std::cin >> pfname;
			try {
				AlgomalBuf = AlgomalDecrypt(Curve, Paramfname, cfname);
				auto Msg = FromPointsToString(Alph, AlgomalBuf);
				SaveMessage(Msg, pfname);
			}
			catch (std::exception& e)
			{
				printf("Exception:%s\n", e.what());
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
		case'6': {
			
			try {
				DiffieHellmanGenerativeRoutine(Curve, Alph);
			}
			catch (std::exception& e)
			{
				printf("Exception: %s\n", e.what());
			}
			catch (...)
			{
				printf("Unknown error\n");
			}
			break;
		}
		case'7': {

			try {
				DiffieHellmanDecipheringRoutine();
			}
			catch (std::exception& e)
			{
				printf("Exception: %s\n", e.what());
			}
			catch (...)
			{
				printf("Unknown error\n");
			}
			break;
		}
		case'8': {

			try {
				DiffieHellmanCipheringRoutine(Curve, Alph);
			}
			catch (std::exception& e)
			{
				printf("Exception: %s\n", e.what());
			}
			catch (...)
			{
				printf("Unknown error\n");
			}
			break;
		}
		case'9': {

			try {
				Messi_Amuro_FileHelper::MessiAmuroRoutine(Curve, Alph);
			}
			catch (std::exception& e)
			{
				printf("Exception: %s\n", e.what());
			}
			catch (...)
			{
				printf("Unknown error\n");
			}
			break;
		}
		case's':
		case'S': {
			try {
				Eleptic::Signatures::SignaturesRoutine();
			}
			catch (std::exception& e)
			{
				printf("Exception: %s\n", e.what());
			}
			catch (...)
			{
				printf("Unknown error\n");
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
