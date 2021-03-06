#pragma once
/*
Example
Given E751(-1,1): y^2=x^3-x+1 mod 751
P(69,510), Q(62,372), R(74,170)
Find 2P - 3Q + R

1) 2P; k = (3x1^2 + a)/(2*y1) = (3*69^2 - 1)/(2 * 510) mod 751 = (14282 mod 751) / (1020 mod 751) =
= 13/269 = 13* 269^-1 mod 751 = -120 mod 751. Oh
x = k^2 - 2*x1 = 744 mod 751
y = -120*(69-744)-510 mod 751 = 133

*/