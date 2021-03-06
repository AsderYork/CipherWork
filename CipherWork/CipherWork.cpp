// Example program
#include "stdafx.h"
#include <string>

#include <string>
#include <vector>
#include <tuple>
#include <cmath>
#include <functional>
#include <algorithm>

#include "RSA.h"
#include "Eleptic.h"

#include "FileInterface_Curve.h"


/**
* Variant 8.
* Something about Eulers and p,q
* if p and q are primes
* p*q=n
* p+q=n+1-f(n); p+q are even so we can think of  n+1-f(n) as 2k
* hence
* x^2-2kx+n=0
* x1=k+sqrt(k^2-n)=p
* x2=k-sqrt(k^2-n)=q
* where k = (n+1-f(n))/2
*
*/

/**
So, how does it work?
1) Alice and Bob creates a pair of valid RSA key
1.1) He(she) chooses 2 prime big number (p,q), then multiply them together N = p*q
1.2) Find f = (p-1)*(q-1)
1.3) Find random 0<e<f coprime with f
1.4) Find d, so that d*e = 1 mod f (multiplicative inverse)
1.5) Then pair(N,e) - public key, pair(N,d) - private key. Everything else should be destroyed

2)Prepare text to be encrypted
2.1)Use n-gramm or block. Or bits. I mean, reinterpret_cast<int>(std::string[i])

3)The ecrypt. Just take your number-representation of plaintext M and do M^e mod N = C (If you have e, otherwise swap e for d for this and every other stage)
4)The decrypt. Take you ciphertext C and do M = C^d mod N
*/




std::vector<BigInt> Scarmble(std::string text, const std::string alphabet, BigInt maxval)
{
	std::vector<BigInt> Ret;
	BigInt TmpVal = 0;
	for (int i = 0; i < text.size(); i++)
	{
		//printf("\n");
		int val = alphabet.find(text[i]);
		val += 10;
		//printf("Val:%i; Tmp:%i",val, TmpVal);
		if (TmpVal == 0) { TmpVal = val; continue; }

		BigInt DecTmp = TmpVal;
		int Dec = 0;
		while (DecTmp != 0) { Dec++; DecTmp /= 10; }
		//printf("Dec:%i;", Dec);
		if (TmpVal + (long long)(val*(pow(10, Dec))) > maxval) { Ret.push_back(TmpVal); TmpVal = val; continue; }
		TmpVal = TmpVal + val*((long long)pow(10, Dec));
	}
	if (TmpVal != 0) { Ret.push_back(TmpVal); };
	return Ret;
}

std::string Descramble(std::vector<BigInt> Vec, const std::string alphabet, BigInt maxval)
{
	std::string Ret;
	for (auto& V : Vec)
	{
		//printf("\nCV:%i",V);
		int Dec = 0;
		BigInt DecTmp = V;
		while (DecTmp != 0) { Dec++; DecTmp /= 10; }
		std::string Inv = "";
		while (Dec > 0)
		{
			Dec -= 2;
			int Val = V.toInt() / ((int)pow(10, Dec));
			V %= (int)pow(10, Dec);

			//printf("\nVAL:%i",Val);
			Inv += alphabet[Val - 10];
		}
		std::reverse(Inv.begin(), Inv.end());
		Ret += Inv;
	}
	return Ret;
}




struct ModuloEqu
{
	BigInt Equ = 10;
	BigInt modulo = 12;

	ModuloEqu(BigInt _Equ, BigInt _m) : Equ(_Equ), modulo(_m) {}
};
BigInt ModuloSystemSolver(std::vector<ModuloEqu> Vec)
{
	BigInt M1, M2, M3;
	M1 = Vec[1].modulo*Vec[2].modulo;
	M2 = Vec[0].modulo*Vec[2].modulo;
	M3 = Vec[0].modulo*Vec[1].modulo;

	BigInt d1, d2, d3;
	d1 = (modInverse(M1, Vec[0].modulo) * Vec[0].Equ) % Vec[0].modulo;
	d2 = (modInverse(M2, Vec[1].modulo) * Vec[1].Equ) % Vec[1].modulo;
	d3 = (modInverse(M3, Vec[2].modulo) * Vec[2].Equ) % Vec[2].modulo;
	auto SubRet = d1*M1 + M2*d2 + M3*d3;
	auto SubModulo = Vec[0].modulo * Vec[1].modulo * Vec[2].modulo;
	while (SubRet < 0) { SubRet += SubModulo; }
	return SubRet % SubModulo;
}


BigInt EqulidBreak(BigInt e1, BigInt e2, BigInt Modulo, BigInt y1, BigInt y2)
{
	BigInt s, r;
	auto gcdVal = gcd(e1, e2, r, s);


	//printf("s:%s\n", (-s).toString().c_str());
	//printf("r:%s\n", r.toString().c_str());
	if (r < 0) { y1 = modInverse(y1, Modulo); r = -r; }
	auto c1r = modExp(y1, r, Modulo);

	if (s < 0) { y2 = modInverse(y2, Modulo); s = -s; }
	auto c2s = modExp(y2, s, Modulo);
	//printf("c1^r:%s\n", c1r.toString().c_str());
	//printf("c2^s:%s\n", c2s.toString().c_str());

	//printf("M^e:%s\n", (c1r*c2s).toString().c_str());

	BigInt RetVal = (c1r*c2s) % Modulo;


	if (gcdVal < 0) { RetVal = modInverse(RetVal, Modulo); }
	return RetVal;

}

BigInt PowDecrypt(BigInt y, BigInt e, BigInt Modulo)
{
	auto TmpY = y;
	auto LastY = y;
	
	do {
		LastY = TmpY;
		TmpY = modExp(TmpY, e, Modulo);
	} while (TmpY != y);
	return LastY;
}


bool DoAuthor(bool self=false)
{
	long long  Rand = rand();
	while (Rand > 1000 * 1000) { Rand = rand(); }

	BigInt CipheredVal = 0;
	RSA::Key PublicKey;
	if (!self) {
		printf("Plase cipher number '%lli', then insert it. Then insert your public key(first modulo, then exponent)\n", Rand);
		std::cin >> CipheredVal;
		std::cin >> PublicKey.modulo;
		std::cin >> PublicKey.exponent;
	}
	else
	{
		auto Key = RSA::GenKey(rand());
		PublicKey = Key.first;
		CipheredVal = RSA::Cipher(Key.second, Rand);
	}

	if(RSA::Cipher(PublicKey, CipheredVal) == Rand)
	{
		printf("Correct!\n");
		return true;
	}
	else
	{
		printf("Incorrect!\n");
		return false;
	}
}


void AttackFromLab4()
{
	std::vector<BigInt> Data1{
		188779427301,
		142624237358,
		222856552604,
		64779987640,
		184552630472,
		357891671735,
		159800573947,
		320365191568,
		53704108470,
		29809614757,
		236651896578,
		5185872557,
		374026260505
	}, Data2{
		330155414629,
		183843269790,
		113231290101,
		381735803560,
		115846890704,
		117837936469,
		188064551177,
		241636957582,
		253908524873,
		219235963059,
		333424804843,
		278400905892,
		254102728294
	};


	printf("Attack from lab4\n");
	for (int i = 0; i < Data1.size(); i++)
	{
		auto EqBreak = EqulidBreak(744721, 1297633, 392117053283, Data1[i], Data2[i]);

		//auto EqBreak = EqulidBreak(1025537, 722983, 357114156277, 68639736967, 204258645263);
		printf("%s\n", EqBreak.toString().c_str());
	}
}

void AttackFromLab5()
{
	BigInt SRMod1 = 431972773933;
	BigInt SRMod2 = 432558060211;
	BigInt SRMod3 = 434276528083;
	std::vector<BigInt> Data1{ 43268974598,
		302331913599,
		47134049761,
		126642563008,
		165827503054,
		232086597542,
		31465887151,
		30373336865,
		284998624093,
		89084365158,
		322533676789,
		383736009455,
		108545189851
	}
		, Data2{ 330701159000,
		104807592171,
		45038416117,
		81063981859,
		427734601871,
		27505991527,
		81910363197,
		190166502949,
		116404011104,
		249933949107,
		90486698466,
		206265723002,
		276536042468
	}
		, Data3{
		269237460393,
		165034165638,
		207280715083,
		151936477226,
		7495879547,
		141105308724,
		316939568874,
		360819196331,
		46940627813,
		137301580237,
		168518778628,
		113124777920,
		282998095133
	}
	;

	printf("Attack from lab 5. Values must be pow(x,1/3)\n");
	for (int i = 0; i < Data1.size(); i++)
	{
		auto SolveRes = ModuloSystemSolver({ ModuloEqu(Data1[i], SRMod1), ModuloEqu(Data2[i],SRMod2), ModuloEqu(Data3[i],SRMod3) });
		printf("%s\n", SolveRes.toString().c_str());
	}
}

void AttackFromLab3()
{
	printf("Attack from Lab3\n");
	std::vector<BigInt> Data{
		61553353723258,
		11339642237403,
		55951185642146,
		38561524032018,
		34517298669793,
		33641624424571,
		78428225355946,
		50176820404544,
		68017840453091,
		5507834749606,
		26675763943141,
		47457759065088
	};

	RSA::Key Key(81177745546021, 2711039);
	auto BrokenKey = RSA::Brake(Key);

	for (int i = 0; i < Data.size(); i++)
	{
		auto PhermatResult = RSA::Cipher(BrokenKey, Data[i]);
		printf("%s\n", PhermatResult.toString().c_str());
	}

}

/*
int main()
{
	//

	Eleptic::Curve Cur(10, 9, 50);

	auto msgRes = LoadMessage("msg.txt");
	SaveMessage(msgRes, "msg_out.txt");

	auto Alph = Cur.GenAlphabet();
	for (int i = 0; i < Alph.size(); i++)
	{
		printf("%s\n", Alph[i].ToStr().c_str());
	}
	//auto Resq = Cur.GenAlphabet();
	auto Priv = Cur.GeneratePrivate(Cur.GetPoint(42, 30));
	printf("S:%i; Priv:%s\n", Priv.second, Priv.first.ToStr().c_str());

	
	
	auto Secret = Cur.GenShared(Cur.GetPoint(20,4), Priv.second);

	printf("Secret:%s\n", Secret.ToStr().c_str());

	/*
	4,17
1,35
29,32
31,16
4,17
1,35
	
	auto Cipher = Cur.Cipher({
		Cur.GetPoint(4,17),
		Cur.GetPoint(1,35),
		Cur.GetPoint(29,32),
		Cur.GetPoint(31,16),
		Cur.GetPoint(4,17),
		Cur.GetPoint(1,35)
	}, Secret);

	for (int i = 0; i < Cipher.size(); i++)
	{

		printf("C:%s\n", Cipher[i].ToStr().c_str());
	}

	auto Deciphered = Cur.Decipher({
		Cur.GetPoint(4,17),
		Cur.GetPoint(1,35),
		Cur.GetPoint(29,32),
		Cur.GetPoint(31,16),
		Cur.GetPoint(4,17),
		Cur.GetPoint(1,35)
	}, Secret);

	for (int i = 0; i < Deciphered.size(); i++)
	{

		printf("P:%s\n", Deciphered[i].ToStr().c_str());
	}


	Eleptic::Point P(48, 702, 751, -1);
	Eleptic::Point Q(69, 241, 751, -1);
	Eleptic::Point R(98, 338, 751, -1);
	Eleptic::Point	Mul(75, 318, 751, -1);

	auto Res = P*2 + Q*3 + R.inv();

	printf("%s:%s\n", Res.x.toString().c_str(), Res.y.toString().c_str());

	if (Res.Check(-1,1))
	{
		printf("OnLine\n");
	}
	
	
	int i = 142;
		auto ResMul = Mul*i;


		printf("%s:%s\n", ResMul.x.toString().c_str(), ResMul.y.toString().c_str());


		//Given E751(-1, 1) : y ^ 2 = x ^ 3 - x + 1 mod 751

		if (!ResMul.Check(-1, 1))
		{
			printf("[%4i]Incorrect!!!\n", i);
		}
		else{
			printf("[%4i]Correct\n", i);
		}

	return 0;
}
*/
int main()
{
	FileInterface();
	/*std::vector<BigInt> Datas{
		14462874,
		1046160,
		17601191,
		3499655,
		7702936,
		3164953,
		3133043,
		15644002,
		14804399,
		7702936,
		3164953,
		3672248,
		1859239,
		11035456,
		2115506,
		18947024
	};

	RSA::Key K;
	K.modulo = 22524247;
	K.exponent = 17185771;
	*/
	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	/*
	auto Rtsqt = 	RSA::DecipherWithT(Datas, K, alphabet);
	printf("%s\n", Rtsqt.c_str());
	for (int i = 0; i < Datas.size(); i++)
	{
		printf("%s\n",RSA::Cipher(K,Datas[i]).toString().c_str());
	}
	*/

	AttackFromLab3();
	


	
	auto Keypair = RSA::GenKey(10);

	auto Res = modExp(520, 6043, 22520299);
	Res = modExp(Res, 14539027, 22520299);
	printf("Re:%i\n", Res);
	printf("Public:%s\nPrivate:%s\n", Keypair.first.asString().c_str(), Keypair.second.asString().c_str());

	std::string plaintext = "THIS TEXT WILL BE CIPHERED";


	auto Ciph = RSA::CipherTextNGRAM(plaintext, Keypair.second, 2, alphabet);
	auto CiphRes = RSA::DecipherNGRAMS(Ciph, Keypair.first, 2, alphabet);
	printf("%s\n", CiphRes.c_str());


	auto BrokKey= RSA::Brake(Keypair.first);
	printf("Broken:%s\n", BrokKey.asString().c_str());

	return 0;

}
/*
x=2 mod 5
x=15 mod 17
x = 5 mod 12

M1 = 17*12=204
M2 = 5*12=60
M3 = 5*17 = 85

204*d1 = 2 mod 5
d1 = 2*204^-1 mod 5
200*d1 + 4*d1 = 2 mod 5
4 *d1 = 2 mod 5
d1 = 3

60*d2 = 15 mod 17
51*d2 + 9*d2 = 15 mod 17
9*d2 = 15 mod 17

d3 расчитывается так же

Единственное решение по модулю 5*17*12 = 1020
x = 204*3 + 60*13 + 85*5 mod 1020 = 797

Атака RSA на основе вот этого вот
RSA неспешный. Для повышения скорости, часто используют малую экспоненту шифрования. В этом случае у нескольких
адресатов могут оказаться одинаковые экспоненты. 
Пусть, например, три адресата имеют попарно взаимно-простые модули.(Можно проверить алгоритмом Евклида)
И общую экспонентушифрования (Для примера e =3)
k1(N1,3), K2(N2,3), K(N3,3); <N1,N2> = 1; <N2, N3> = 1; <N1, N3> = 1
Если кто-то посылает им всем одно и то же сообщение x, то атакующий получит
c1 = x^3 mod N1
c2 = x^3 mod N2
c3 = X^3 mod N3
Затем он находит решение системы
y = y1 mod N1
y = y2 mod N2
y = y3 mod N3
Но y = x^3, x^3 < <N1, N2, N3> то x = sqr(y,3);

А еще выбор малого ключа дешифрвания d маленьким, может позволить взлом простым перебором.
Так же если d < sqrt(N,4), то значение d можно восстановить, используя непрерывные дроби. Что это? А кто его знает

А теперь мы атакуем.
У нас есть 3 открытых ключа. k1(N1,e), k2(N2,e), K3(N3,e)
N1 = 265549;
N2 = 45901
N3 = 25351
e = 3;

Все три адресата получили одно и то же сообщениие. В резултате шифрования они приняли вид
y1 = 5366
y2 = 814
y3 = 4454

y = 5366 mod 26549
y = 814 mod 45901
y = 4454 mod 25351

Надеюсь 30 минут достаточно на реализцаию. Нет? Очень жаль
Атака методом безключевого чтения
Пусть наоборот выбраны одинаковые модули, но разныне экспоненты e1, e2
Если другой пользователь посылает им сообщение, то атакующий получает
c1 = x^e1 mod N
c2 = x^e2 mod N
Тогда исходное сообщение x можно восстановить при помощи расширенного алгоритма Евклидиуса
1) По алгоритму Евклида находятся R, S, такие что R*e1 + S*e2 = 1
2) x = y1^R * y2^S mod N

Пример
N = 137759
e1 = 191
e2 = 233

c1 = 60197
c2 = 63656

Вот еще, e1 и e2 должны быть взаимно-простыми

прокатываем алгоритм Евклида
r = 61, s = -50
Восстанавливаем текст
x = 60197^61 + 63656^-50 mod 137759 = 1234

И еще одна атака!
Атака повторным шифрованием
Строим последовательность значений
y0 = y
y1 = y0^e mod n
y(i) = y(i-1)^e mod n

Так как e и f(n) взаимно-простые, то по теореме Эйлера, возведение в степень зациклится
Но как быстро? И да, как только мы встретим yi = y0, это значит, что y(i-1) было шифротекстом!

А если y и N взаимно-просты, то по теореме Эйлера. На самом деле написанное выше будет справедливо только при этом условии
Пример
N = 84517
e = 397
y = 8646
Тогда x = 16137

Анализ метода повторного шифрования показывает необходимость соблюдения требований на выбор p,q для обеспечения стойкости
В данном примере d был 82225. Неудачный выбор криптосистемы? Нужно было 3DES. Привет к тому, что атака сработала очень быстро
Тогда как нахождение такого d потребовало бы чуть больших вычислений. На самом деле не на порядок, а на 5 порядков, но разницы же нет, да?



*/