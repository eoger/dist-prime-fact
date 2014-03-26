#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>

#define COMMON_PORT 43210
#define QUANTITY 1999
#define MAX_FACTORS 64 // Car premier minimum = 2 et dans 2^64 on a 64 * 2

#define bool int
#define true 1
#define false 0


#define FOPEN_MODE "r"

typedef struct resultat
{
    uint64_t n;
    uint64_t factors[MAX_FACTORS];
    int nb_factors;
} resultat;

typedef struct msg
{
    int ctl;
    uint64_t nbr;
    uint16_t nb_factors;
    uint64_t factors[MAX_FACTORS];
} msg;

#define CTL_M_SENDNUMBER 1
#define CTL_M_NOMORENUMBERS 2
#define CTL_S_READY 3
#define CTL_S_RETURNRESULT 4
#define CTL_S_DONE 5

#endif
