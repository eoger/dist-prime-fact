#include <stdlib.h>

#include "brent-pollard.h"
#include "../shared/common.h"

/*
 * Maths provided by http://remcobloemen.nl/
 * Adapted from C++ to C
 */

uint64_t gcd(uint64_t u, uint64_t v)
{
    if(u == 0 || v == 0) return u | v;

    // Remove common twos in u and v
    int shift;

    for(shift = 0; ((u | v) & 1) == 0; ++shift)
    {
        u >>= 1;
        v >>= 1;
    }

    // Remove twos from u
    while((u & 1) == 0) u >>= 1;

    do
    {
        // Remove twos in v
        while((v & 1) == 0) v >>= 1;

        /* Now u and v are both odd, so diff(u, v) is even.
        Let u = min(u, v), v = diff(u, v)/2. */
        if(u < v)
        {
            v -= u;
        }
        else
        {
            uint64_t diff = u - v;
            u = v;
            v = diff;
        }

        v >>= 1;
    }
    while(v != 0);

    return u << shift;
}
/** Miller-Rabin */
uint64_t mulmod(uint64_t a, uint64_t b, uint64_t c)
{
    uint64_t x = 0, y = a % c;

    while(b > 0)
    {
        if(b % 2 == 1)
        {
            x = (x + y) % c;
        }

        y = (y * 2) % c;
        b /= 2;
    }

    return x % c;
}
uint64_t powmod(uint64_t a, uint64_t b, uint64_t c) //(a^b)%c
{
    uint64_t temp;

    if(!b)
        return 1;

    if(b == 1)
        return a % c;

    if(b % 2)
    {
        temp = powmod(a, b - 1, c);
        return mulmod(temp, a, c);
    }

    temp = powmod(a, b / 2, c);
    return mulmod(temp, temp, c);
}
bool is_prime(uint64_t P)
{
    if(P != 2 && P % 2 == 0)
        return false;

    uint64_t s = P - 1;

    while(s % 2 == 0)
    {
        s /= 2;
    }

    for(int i = 0; i < 25; i++)
    {
        uint64_t a = rand() % (P - 1) + 1, temp = s;
        uint64_t mod = powmod(a, temp, P);

        while(temp != (P - 1) && mod != 1 && mod != (P - 1))
        {
            mod = mulmod(mod, mod, P);
            temp *= 2;
        }

        if(mod != (P - 1) && temp % 2 == 0)
        {
            return false;
        }
    }

    return true;
}

uint64_t brent_pollard_factor(uint64_t n)
{
    const uint64_t m = 1000;
    uint64_t a, x, y, ys, r, q, g;

    do
    {
        a = random() % n;
    }
    while(a == 0 || a == n - 2);

    y = random() % n;
    r = 1;
    q = 1;

    do
    {
        x = y;

        for(uint64_t i = 0; i < r; i++)
        {
            // y = y² + a mod n
            y = mulmod(y, y, n);
            y += a;

            if(y < a) y += (UINT64_MAX - n) + 1;

            y %= n;
        }

        uint64_t k = 0;

        do
        {
            for(uint64_t i = 0; i < m && i < r - k; i++)
            {
                ys = y;
                // y = y² + a mod n
                y = mulmod(y, y, n);
                y += a;

                if(y < a) y += (UINT64_MAX - n) + 1;

                y %= n;
                // q = q |x-y| mod n
                q = mulmod(q, (x > y) ? x - y : y - x, n);
            }

            g = gcd(q, n);
            k += m;
        }
        while(k < r && g == 1);

        r <<= 1;
    }
    while(g == 1);

    if(g == n)
    {
        do
        {
            // ys = ys² + a mod n
            ys = mulmod(ys, ys, n);
            ys += a;

            if(ys < a) ys += (UINT64_MAX - n) + 1;

            ys %= n;
            g = gcd((x > ys) ? x - ys : ys - x, n);
        }
        while(g == 1);
    }

    return g;
}
unsigned int get_prime_factors(uint64_t n, uint64_t * dest)
{

    if(is_prime(n))
    {
        dest[0] = n;
        return 1;
    }

    unsigned int nbFactors = 0;
    unsigned int nbPrimes = 0;
    uint64_t factors[MAX_FACTORS];

    uint64_t factor = brent_pollard_factor(n);
    factors[nbFactors++] = n / factor;
    factors[nbFactors++] = factor;

    do
    {
        uint64_t m = factors[nbFactors - 1];
        nbFactors--;

        if(m == 1) continue;

        if(is_prime(m))
        {
            dest[nbPrimes++] = m;

            // Remove the prime from the other factors
            for(unsigned int i = 0; i < nbFactors; i++)
            {
                uint64_t k = factors[i];

                if(k % m == 0)
                {
                    do k /= m;

                    while(k % m == 0);

                    factors[i] = k;
                }
            }
        }
        else
        {
            factor = brent_pollard_factor(m);
            factors[nbFactors++] = m / factor;
            factors[nbFactors++] = factor;
        }
    }
    while(nbFactors);

    return nbPrimes;
}


