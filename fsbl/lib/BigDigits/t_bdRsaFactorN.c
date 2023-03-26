/* $Id: t_bdRsaFactorN.c $ */

/* 
	This code uses the free BIGDIGITS library version 2.3 available from
	http://di-mgt.com.au/bigdigits.html
	to show how to factor the RSA modulus n given the secret exponent d
	Copyright (C) 2012 DI Management Services Pty Ltd. All rights reserved.
*/

/*
	Last updated:
	$Date: 2012-12-24 16:13 $
	$Revision: 1.0.1 $
	$Author: dai $
*/

#include <stdio.h>
#include "bigd.h"

int debug = 1;
#define DBDPRINT(pre, x, post) if(debug)bdPrintDecimal((pre),(x),(post))

const int primes[] = {
	2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 
	31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 
	73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 
	127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 
	179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 
	233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 
};
#define NPRIMES (sizeof(primes)/sizeof(primes[0]))

int find_factors_of_n(BIGD p, BIGD q, BIGD n, BIGD e, BIGD d)
{
	BIGD k, t, g, x, y, r;
	int i, isdone;
	k = bdNew();
	t = bdNew();
	g = bdNew();
	x = bdNew();
	y = bdNew();
	r = bdNew();

	bdSetZero(p);
	bdSetZero(q);

	/* 1. [Initialize] Set k <-- de - 1 */
	bdMultiply(k, d, e);
	bdDecrement(k);
	DBDPRINT("k=de-1=", k, "\n");

	/* 2. [Try a random g] Choose g at random from {2, ..., N-1} */
	/*    (we cheat a bit here and just try the first primes in order) */
	for (isdone = 0, i = 0; !isdone && i < NPRIMES; i++)
	{
		bdSetShort(g, primes[i]);
		DBDPRINT("Trying g=", g, "\n");
		/* Set t <-- k */
		bdSetEqual(t, k);
		/* 3. [Next t] If t is divisible by 2 ... */
		while (bdIsEven(t))
		{
			/* Set t <-- t / 2 */
			bdShiftRight(t, t, 1);
			DBDPRINT("t=", t, "\n");
			/* Set x = g^t mod N */
			bdModExp(x, g, t, n);
			DBDPRINT("x=g^t mod N=", x, "\n");
			/* 4. [Finished?] If x > 1 and y = gcd(x-1, N) 
			      then set p <-- y and q <-- N/y, output (p,q) and stop.
			*/
			if (bdShortCmp(x, 1) > 0)
			{
				bdDecrement(x);
				bdGcd(y, x, n);
				DBDPRINT("y=gcd(x-1,N)=", y, "\n");
				if (bdShortCmp(y, 1) > 0)
				{	/* We have it */
					bdSetEqual(p, y);
					bdDivide(q, r, n, y);
					isdone = 1;
					break;
				}
			}
		} /* 4a. ... otherwise go to step 3. */
	}	/* 3a. ... otherwise go to step 2. */
	/* Finally, to be consistent with convention, we make sure p > q */
	if (isdone && bdCompare(p, q) < 0)
	{
		bdSetEqual(r, p);
		bdSetEqual(p, q);
		bdSetEqual(q, r);
	}

	bdFree(&k);
	bdFree(&t);
	bdFree(&g);
	bdFree(&x);
	bdFree(&y);
	bdFree(&r);

	return isdone;
}

void test_simple(void)
{
	BIGD n, e, d, p, q;
	n = bdNew();
	e = bdNew();
	d = bdNew();
	p = bdNew();
	q = bdNew();

	bdSetShort(n, 25777);
	bdSetShort(e, 3);
	bdSetShort(d, 16971);
	
	printf("Input:\n");
	bdPrintDecimal("n=", n, "\n");
	bdPrintDecimal("e=", e, "\n");
	bdPrintDecimal("d=", d, "\n");

	find_factors_of_n(p, q, n, e, d);

	printf("Output:\n");
	bdPrintDecimal("p=", p, "\n");
	bdPrintDecimal("q=", q, "\n");

//clean_up:
	bdFree(&n);
	bdFree(&e);
	bdFree(&d);
	bdFree(&p);
	bdFree(&q);
}

void test_508(void)
{
	BIGD n, e, d, p, q;
	n = bdNew();
	e = bdNew();
	d = bdNew();
	p = bdNew();
	q = bdNew();
	/*
	Using 508-bit RSA key from 
	"Some Examples of the PKCS Standards"
	An RSA Laboratories Technical Note,
	Burton S. Kaliski Jr., November 1, 1993

	p = 33 d4 84 45 c8 59 e5 23 40 de 70 4b cd da 06 5f bb 40 58
	d7 40 bd 1d 67 d2 9e 9c 14 6c 11 cf 61
	q = 33 5e 84 08 86 6b 0f d3 8d c7 00 2d 3f 97 2c 67 38 9a 65
	d5 d8 30 65 66 d5 c4 f2 a5 aa 52 62 8b
	*/

	bdConvFromHex(n, "0a66791dc6988168de7ab77419bb7fb0c001c62710270075142942e19a8d8c51d053b3e3782a1de5dc5af4ebe99468170114a1dfe67cdc9a9af55d655620bbab");
	bdConvFromHex(e, "010001");
	bdConvFromHex(d, "0123c5b61ba36edb1d3679904199a89ea80c09b9122e1400c09adcf7784676d01d23356a7d44d6bd8bd50e94bfc723fa87d8862b75177691c11d757692df8881");
	
	printf("Input:\n");
	bdPrintHex("n=", n, "\n");
	bdPrintHex("e=", e, "\n");
	bdPrintHex("d=", d, "\n");

	find_factors_of_n(p, q, n, e, d);

	printf("Output:\n");
	bdPrintHex("p=", p, "\n");
	bdPrintHex("q=", q, "\n");

//clean_up:
	bdFree(&n);
	bdFree(&e);
	bdFree(&d);
	bdFree(&p);
	bdFree(&q);
}

void test_alice1024(void)
{
	BIGD n, e, d, p, q;
	n = bdNew();
	e = bdNew();
	d = bdNew();
	p = bdNew();
	q = bdNew();
	/*
	Using Alice's 1024-bit RSA key from [RFC4134]:
	Hoffman, P., Ed., "Examples of S/MIME Messages", RFC 4134, July 2005.
	*/

	bdConvFromHex(n, "E08973398DD8F5F5E88776397F4EB005BB5383DE0FB7ABDC7DC775290D052E6D12DFA68626D4D26FAA5829FC97ECFA82510F3080BEB1509E4644F12CBBD832CFC6686F07D9B060ACBEEE34096A13F5F7050593DF5EBA3556D961FF197FC981E6F86CEA874070EFAC6D2C749F2DFA553AB9997702A648528C4EF357385774575F");
	bdConvFromHex(e, "010001");
	bdConvFromHex(d, "A403C327477634346CA686B57949014B2E8AD2C862B2C7D748096A8B91F736F275D6E8CD15906027314735644D95CD6763CEB49F56AC2F376E1CEE0EBF282DF439906F34D86E085BD5656AD841F313D72D395EFE33CBFF29E4030B3D05A28FB7F18EA27637B07957D32F2BDE8706227D04665EC91BAF8B1AC3EC9144AB7F21");
	// p = F6D6E022214C5F0A70FF27FCE5B3506A9DE50FB58596C640FAA80AB49B9B0C55C2011DF937828A14C8F2930E92CDA56621B93CD206BFB45531C9DCADCA982DD1
	// q = E8DEB0112509D2025101DE8AE89850F5777761A445936B085596735DF4C85B129322738B7FD3707FF5A4AABB74FD3C226ADA38912A865B6C14E8AE4C9EFA8E2F
	printf("Input:\n");
	bdPrintHex("n=", n, "\n");
	bdPrintHex("e=", e, "\n");
	bdPrintHex("d=", d, "\n");

	find_factors_of_n(p, q, n, e, d);

	printf("Output:\n");
	bdPrintHex("p=", p, "\n");
	bdPrintHex("q=", q, "\n");

//clean_up:
	bdFree(&n);
	bdFree(&e);
	bdFree(&d);
	bdFree(&p);
	bdFree(&q);
}



int main(void)
{
	test_simple();
	test_508();
	//test_alice1024();

	return 0;
}
