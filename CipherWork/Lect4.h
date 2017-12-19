#pragma once
/*
Diffie-helman with eleptic curves

...There was a part about generating shared secret, i missed it

Ciphering with shared secret.
Even block get (XORed) with X0 of shared secret
Odd blocks get (XORed) with Y0 of shared secret

To decipher
Even block get (XORed^-1)
Odd block get (XORed^-1)

For example we can summate instead of XOR
In that case on decipher we must first find Ordinal of set of all points on a curve, then modinverse x0 and y0 with respect
to found mod,(ord(E(a,b)) and only then you can summate.

Even worse, if Y0 and X0 of shared secret is not co-prime with N; N=Ord (E(a,b))

So you might want to shift X0 and Y0 from the curve, untill they are co-prime with N. And yeah, you must do this on both sides.



Building an alphabet on a points of eleptic curve.

Delta = -4*a^3 -27*b^2 mod m
Delta = -4*11^3 - 27*9^2 mod 43 = -7511 = 14 => Is a good curve

y(43)	|0	|1	|3	|4	|5	|6	|7	|8	|9	|10	|11	|12	|13 |14	|15	|16	|17	|18	|19	|20	|21
y^2		|0	|2	|9	|16	|25	|36	|6	|21	|38 |14	|35	|15	|40	|24	|10	|41	|31	|23	|17	|13	|11

((p+1)/2 + k) mod p = ((p-1)/2 - k)^2 mod p

for any y[0..p-1] where p is prime: y^2 mod p = (p-y)^2 mod p
or y1^2 + y2^2 = p for y1^2 = y2^2 and y1 != y2;

x(p)			|0	|1	|2	|
x^3 + 11x + 9	|9	|21	|39X|
Check evry possible x. Save those, that in result give any of the previous y

btw Ord(E(a,b)) is equal to amount of all that points plus infinite point

Hossey theorem:
Ord(Ep(a,b))	is between p+1-2*sqrt(p) and p+1+2*sqrt(p)
for any Q in Ep(a,b) N*Q = 0



Elgomal's cryptosystem

You need Ep(a,b)
Any point G on Ep(a,b)

Every use choses random number Sa;
Generates Priv = G*Sa

Ciphering
There are masks(Mi) for every letter in the text
Mi = Ki*Other.Priv; Ki - random number per given letter; Pb - open key of the reciver
Hi = G*Ki;
Ci = Pi + Mi;

And then you send (Ci, Hi)

Deciphering
Pi = Ci - Priv*Hi








*/