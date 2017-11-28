#pragma once

/*
Eleptic crypto!
Number theory, algebraic geometriy, eleptic curves in finite fields. All of this is used in cyptography.
Main reason fo this is that it's freakin' hard. I mean, its cryptography! What are we talking about?
Eleptic curves provide infinite source of finite simple groups
Which, even if they're huge, calculates well and have reach structure.

Some facts about eleptic curves
F - field(Integer/Rational/Complex/modulo)
Characteristic of filed F is a natural number p (p=charF) that p*(1)F=(0)F. Where (1)F is a unit element. (0)F is a neutral element.

Algebraic curve of order N in field F is a set of paris<x,y> from F^2, so that p(x,y)=0 where p is a polynom of order n with coefficients from F.

Point x0, y0 of algebraic curve p(x,y)=0 is called not-special, if at least one partial direvative in this point is not zero.

Curve p(x,y) is called not-special, if all it's points is not-special.

Then in every in every point there is a (x-x0)F'x(x0,yo)+(y-y0)F'y(x0,y0)=0

Not-special algebraic cuve of order 3 in field F is called eleptic cuve

In rational field, any eleptic curve can be expressed as y^2=x^3+ax+b

In general case, it can be expressed as y^2+a1*x*y + a3*y = x^3 + a2*x^2 + a4*x + a6

Let char(F) = 3; The eleptic curve is a set of points y^2=x^3+ax^2+bx+c

Disxr of cubic polynom -4a^3-37b^2!=0

Cubic curve is special when disc=0 and vice versa

Operations
p,q - points on a curve
-p:
if p=inf, then -p=inf. So inf is a neutral element
if p=(x,y) on a cuve, then -(x,y) = (x,-y)

If Px != Qx then line l=P*Q have only three intersection with a curve

P+Q = -R Where R is the third intersection point.





*/