#pragma once
#include "Messi_amuro.h"
#include "Alphabet_Helper_structures.h"
#include <optional>

class Messi_Amuro_FileHelper
{
private:

	/*template<typename T>
	static T LoadValueFromPrivate(const char* ValueName)
	{
		try {
			std::ifstream stream("ma_private.json", std::ios_base::in);
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
	*/
	template<typename T>
	static std::optional<T> LoadValueFromPrivate(const char* ValueName)
	{
		try {
			std::ifstream stream("ma_private.json", std::ios_base::in);
			cereal::JSONInputArchive Arch(stream);

			T Value;

			Arch(cereal::make_nvp(ValueName, Value));
			return std::make_optional(Value);
		}
		catch (...)
		{
		}
		return std::nullopt;
	}

	static void ToCipher(Eleptic::Curve& curve, std::vector<AlphabetPair> &Alphabet, Eleptic::Messi_AmuroCrypto& Crypto)
	{
		printf("Ciphering\n");
		printf("Insert kinda-plaintext file name\n");
		std::string FilenameFrom;
		std::cin >> FilenameFrom;

		printf("Insert kinda-ciphertext file name\n");
		std::string FilenameTo;
		std::cin >> FilenameTo;

		printf("Loading...\n");
		auto Msg = LoadMessage(FilenameFrom);

		printf("Pointing...\n");		
		auto PointedMsg = ProcessMessage(Alphabet, Msg);

		printf("Ciphering...\n");
		auto CipheredMsg = Crypto.StrightBlend(PointedMsg);

		printf("Alphabeting...\n");
		auto AlphabetedMsg = FromPointsToString(Alphabet, CipheredMsg);

		printf("Saving...\n");
		SaveMessage(AlphabetedMsg, FilenameTo);

		printf("Done\n");


	}
	static void ToDecipher(Eleptic::Curve& curve, std::vector<AlphabetPair> &Alphabet, Eleptic::Messi_AmuroCrypto& Crypto)
	{
		printf("Deciphering\n");
		printf("Insert kinda-ciphertext file name\n");
		std::string FilenameFrom;
		std::cin >> FilenameFrom;

		printf("Insert kinda-plaintext file name\n");
		std::string FilenameTo;
		std::cin >> FilenameTo;

		printf("Loading...\n");
		auto Msg = LoadMessage(FilenameFrom);

		printf("Pointing...\n");
		auto PointedMsg = ProcessMessage(Alphabet, Msg);

		printf("Deciphering...\n");
		auto CipheredMsg = Crypto.InverseBlend(PointedMsg);

		printf("Alphabeting...\n");
		auto AlphabetedMsg = FromPointsToString(Alphabet, CipheredMsg);

		printf("Saving...\n");
		SaveMessage(AlphabetedMsg, FilenameTo);

		printf("Done\n");


	}

	static void SelfTest(Eleptic::Curve& curve, std::vector<AlphabetPair> &Alphabet)
	{
		printf("Insert first secret Value:");
		unsigned int SendesS;
		std::cin >> SendesS;

		printf("Insert second secret Value:");
		unsigned int ReciverS;
		std::cin >> ReciverS;

		printf("Insert text filename:");
		std::string textName;
		std::cin >> textName;

		printf("Loading...\n");
		auto Msg = LoadMessage(textName);

		printf("Pointing...\n");
		auto PointedMsg = ProcessMessage(Alphabet, Msg);

		Eleptic::Messi_AmuroCrypto Crypto1(curve, SendesS);
		Eleptic::Messi_AmuroCrypto Crypto2(curve, ReciverS);


		//printf("AutoCypherDecypher ->...\n");
		//auto AutoCD1 = FromPointsToString(Alphabet,Crypto1.InverseBlend(Crypto1.StrightBlend(PointedMsg)));
		//auto AutoCD2 = FromPointsToString(Alphabet, Crypto1.StrightBlend(Crypto1.InverseBlend(PointedMsg)));

		printf("Cipher ->...\n");
		auto CipheredMsg = Crypto1.StrightBlend(PointedMsg);

		printf("Cipher <-...\n");
		CipheredMsg = Crypto2.StrightBlend(CipheredMsg);

		//--------------

		printf("Decipher ->...\n");
		CipheredMsg = Crypto1.InverseBlend(CipheredMsg);

		printf("Decipher <-...\n");
		CipheredMsg = Crypto2.InverseBlend(CipheredMsg);

		printf("Alphabeting...\n");
		auto AlphabetedMsg = FromPointsToString(Alphabet, CipheredMsg);

		printf("Checking...\n");
		
		if (std::equal(AlphabetedMsg.begin(), AlphabetedMsg.end(), Msg.begin(), Msg.end()))
		{
			printf("Correct\n");
		}
		else
		{
			printf("Incorrect!\n");
		}

		printf("Done\n");

	}

public:

	static void MessiAmuroRoutine(Eleptic::Curve& curve, std::vector<AlphabetPair> &Alphabet)
	{
		printf("Messi_amuro's routine is a big deal, actually!\n");
		printf("First off, let's get a secret value from ma_private.json\n");

		auto MaybeSecretValue = LoadValueFromPrivate<unsigned int>("SecretValue");
		unsigned int SecretValue;
		if (!MaybeSecretValue)
		{
			printf("There was no secret value. So a random one will be generated\n");
			SecretValue = Eleptic::Messi_AmuroCrypto::getRandomCorrectValue(curve);
		}
		else
		{
			SecretValue = *MaybeSecretValue;
			if (!Eleptic::Messi_AmuroCrypto::CheckValue(curve, SecretValue))
			{
				printf("The value is not exactly correct!\n");
			}
		}

		Eleptic::Messi_AmuroCrypto Crypto(curve, SecretValue);
		printf("A seceret value have been set!\n");

		printf("So it's time to [C]ipher/[D]ecipher. Or just [X]Leave. Or [T]est.\n");
		printf("Just for clarity: cipher means Straight, Decipher means Inverse\n");
		printf("So you must first Cipher, then reciver Ciphers, then you Decipher, then reciver Deciphers\n");


		{
			char answer;
			do
			{
				answer = getchar();
				switch (answer)
				{
				case 'x':
				case 'X': {return; }

				case 'c':
				case 'C': {ToCipher(curve, Alphabet, Crypto); break; }

				case 'd':
				case 'D': {ToDecipher(curve, Alphabet, Crypto); break; }
						  
				case 't':
				case 'T': {SelfTest(curve, Alphabet); break; }

				default:
				{printf("Chose either [C]ipher or [D]ecipher. Or just [X]Leave, Or [T]est\n"); }
				}

			} while (true);
		}


	}


};