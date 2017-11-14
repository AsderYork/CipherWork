// Example program
#include "stdafx.h"
#include <iostream>
#include <string>

#include <string>
#include <vector>
#include <tuple>
#include <math.h>
#include <functional>
#include <algorithm>
#include <infint\InfInt.h>

using BigInt = InfInt;

/**
* Variant 8. Str - 11
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

int Primes[] = { 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097,
1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237,
1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399,
1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523,
1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657,
1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801,
1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973,
1979, 1987, 1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081, 2083, 2087, 2089, 2099,
2111, 2113, 2129, 2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267,
2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381, 2383, 2389,
2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549, 2551,
2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699,
2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837,
2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001,
3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181,
3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329,
3331, 3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491,
3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607, 3613, 3617, 3623,
3631, 3637, 3643, 3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727, 3733, 3739, 3761, 3767, 3769, 3779,
3793, 3797, 3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923, 3929,
3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057, 4073, 4079, 4091, 4093,
4099, 4111, 4127, 4129, 4133, 4139, 4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231, 4241, 4243, 4253,
4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409, 4421, 4423,
4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583, 4591,
4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751,
4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933,
4937, 4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077,
5081, 5087, 5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237,
5261, 5273, 5279, 5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419,
5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569,
5573, 5581, 5591, 5623, 5639, 5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693, 5701, 5711, 5717, 5737,
5741, 5743, 5749, 5779, 5783, 5791, 5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857, 5861, 5867, 5869,
5879, 5881, 5897, 5903, 5923, 5927, 5939, 5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053, 6067, 6073,
6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133, 6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221, 6229,
6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301, 6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367,
6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473, 6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569,
6571, 6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673, 6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733,
6737, 6761, 6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833, 6841, 6857, 6863, 6869, 6871, 6883, 6899,
6907, 6911, 6917, 6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997, 7001, 7013, 7019, 7027, 7039, 7043,
7057, 7069, 7079, 7103, 7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207, 7211, 7213, 7219, 7229, 7237,
7243, 7247, 7253, 7283, 7297, 7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411, 7417, 7433, 7451, 7457,
7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561, 7573, 7577, 7583,
7589, 7591, 7603, 7607, 7621, 7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723, 7727, 7741,
7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829, 7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919, 7927,
7933, 7937, 7949, 7951, 7963, 7993, 8009, 8011, 8017, 8039, 8053, 8059, 8069, 8081, 8087, 8089, 8093, 8101, 8111,
8117, 8123, 8147, 8161, 8167, 8171, 8179, 8191, 8209, 8219, 8221, 8231, 8233, 8237, 8243, 8263, 8269, 8273, 8287,
8291, 8293, 8297, 8311, 8317, 8329, 8353, 8363, 8369, 8377, 8387, 8389, 8419, 8423, 8429, 8431, 8443, 8447, 8461,
8467, 8501, 8513, 8521, 8527, 8537, 8539, 8543, 8563, 8573, 8581, 8597, 8599, 8609, 8623, 8627, 8629, 8641, 8647,
8663, 8669, 8677, 8681, 8689, 8693, 8699, 8707, 8713, 8719, 8731, 8737, 8741, 8747, 8753, 8761, 8779, 8783, 8803,
8807, 8819, 8821, 8831, 8837, 8839, 8849, 8861, 8863, 8867, 8887, 8893, 8923, 8929, 8933, 8941, 8951, 8963, 8969,
8971, 8999, 9001, 9007, 9011, 9013, 9029, 9041, 9043, 9049, 9059, 9067, 9091, 9103, 9109, 9127, 9133, 9137, 9151,
9157, 9161, 9173, 9181, 9187, 9199, 9203, 9209, 9221, 9227, 9239, 9241, 9257, 9277, 9281, 9283, 9293, 9311, 9319,
9323, 9337, 9341, 9343, 9349, 9371, 9377, 9391, 9397, 9403, 9413, 9419, 9421, 9431, 9433, 9437, 9439, 9461, 9463,
9467, 9473, 9479, 9491, 9497, 9511, 9521, 9533, 9539, 9547, 9551, 9587, 9601, 9613, 9619, 9623, 9629, 9631, 9643,
9649, 9661, 9677, 9679, 9689, 9697, 9719, 9721, 9733, 9739, 9743, 9749, 9767, 9769, 9781, 9787, 9791, 9803, 9811,
9817, 9829, 9833, 9839, 9851, 9857, 9859, 9871, 9883, 9887, 9901, 9907, 9923, 9929, 9931, 9941, 9949, 9967, 9973
};

BigInt modExp(BigInt b, BigInt e, BigInt m)
{
	BigInt remainder;
	BigInt x = 1;

	while (e != 0)
	{
		remainder = e % 2;
		e = e / 2;

		if (remainder == 1)
			x = (x * b) % m;
		b = (b * b) % m; // New base equal b^2 % m
	}
	return x;
}

BigInt gcdExtended(BigInt a, BigInt b, BigInt *x, BigInt *y)
{
	// Base Case
	if (a == 0)
	{
		*x = 0, *y = 1;
		return b;
	}

	BigInt x1, y1; // To store results of recursive call
	BigInt gcd = gcdExtended(b%a, a, &x1, &y1);

	// Update x and y using results of recursive
	// call
	*x = y1 - (b / a) * x1;
	*y = x1;

	return gcd;
}

BigInt modInverse(BigInt a, BigInt m)
{
	BigInt x, y;
	BigInt g = gcdExtended(a, m, &x, &y);
	if (g != 1)
	{
		return -1;
	}
	else
	{
		// m is added to handle negative x
		return (x%m + m) % m;
	}
}

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

	static std::vector<BigInt> CipherSCRAMB(std::string& Text, RSA::Key Key, int MaxVal, std::string alphabet)
	{
		std::vector<BigInt> RetVec = Scarmble(Text, alphabet, MaxVal);

		for (auto& V : RetVec) { V = Cipher(Key, V); }
		return RetVec;
	}

	static std::string DecipherSCRAMB(std::vector<BigInt> In, RSA::Key Key, int MaxVal, std::string alphabet)
	{
		for (auto& V : In) { V = Cipher(Key, V); }
		return Descramble(In, alphabet, MaxVal);
	}

	static RSA::Key Brake(Key PubKey)
	{
		BigInt x = (long long)ceil(sqrt(PubKey.modulo.toLongLong()));

		BigInt k = 0;
		while (k < PubKey.modulo)
		{
			x = k + (long long)(ceil(sqrt(PubKey.modulo.toLongLong())));

			auto Squere = sqrt(pow(x.toLongLong(),2) - PubKey.modulo.toLongLong());
			auto diff = abs(floor(Squere) - Squere);
			if (abs(floor(Squere) - Squere) < 0.0001f)
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
		printf("p:%lli; q:%lli\n", p, q);
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

BigInt gcd(BigInt a, BigInt b, BigInt& x, BigInt& y)
{
	if (a == 0) { x = 0; y = 1; return b; }

	BigInt x1, y1;
	BigInt d = gcd(b%a, a, x1, y1);
	x = y1 - (b / a) *x1;
	y = x1;
	return d;

}

BigInt EqulidBreak(BigInt e1, BigInt e2, BigInt Modulo, BigInt y1, BigInt y2)
{
	BigInt S, P;
	auto gcdVal = gcd(e1, e2, S, P);

	//while (S < 0) { S += Modulo; }
	//while (P < 0) { P += Modulo; }
	BigInt RetVal = (modExp(y1, S, Modulo) + modExp(y2, P, Modulo)) % Modulo;


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


int main()
{
	/*
	y = 5366 mod 26549
	y = 814 mod 45901
	y = 4454 mod 25351
	*/
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

	for (int i = 0; i < Data1.size(); i++)
	{
		auto SolveRes = ModuloSystemSolver({ ModuloEqu(Data1[i], SRMod1), ModuloEqu(Data2[i],SRMod2), ModuloEqu(Data3[i],SRMod3) });
		printf("%s\n", SolveRes.intSqrt().intSqrt().toString().c_str());
	}

	/*
	N = 137759
	e1 = 191
	e2 = 233

	c1 = 60197
	c2 = 63656

	*/
	auto EqBreak = EqulidBreak(191,233,137759,60197,63656);
	printf("EqBreak:%lli\n", EqBreak);


	auto RepeatBreak = PowDecrypt(8646, 397, 84517);

	printf("RepeatBreak:%lli\n", RepeatBreak);

	auto Keypair = RSA::GenKey(10);

	auto Res = modExp(520, 6043, 22520299);
	Res = modExp(Res, 14539027, 22520299);
	printf("Re:%i\n", Res);
	printf("Public:%s\nPrivate:%s\n", Keypair.first.asString().c_str(), Keypair.second.asString().c_str());

	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	std::string plaintext = "THIS TEXT WILL BE CIPHERED";


	auto Ciph = RSA::CipherTextNGRAM(plaintext, Keypair.second, 2, alphabet);
	auto CiphRes = RSA::DecipherNGRAMS(Ciph, Keypair.first, 2, alphabet);
	printf("%s\n", CiphRes.c_str());

	auto Bl = RSA::CipherSCRAMB(plaintext, Keypair.first, Keypair.first.modulo.toLongLong(), alphabet);
	//for(auto& B : Bl){printf("%i\n", B);}
	auto BlStr = RSA::DecipherSCRAMB(Bl, Keypair.second, Keypair.second.modulo.toLongLong(), alphabet);
	printf("BL:%s\n", BlStr.c_str());

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