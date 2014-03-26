#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    uint64_t number ;
    uint32_t  * word = (void*) & number ;
    uint64_t *previous_numbers;

    // how many numbers to generate
    int quantity = 1999;
    if( argc > 1)
        quantity=atoi(argv[1]);

    // maximum magnitude of numbers, in bits (0..64)
    int magnitude= 57;
    if( argc > 2)
        magnitude=atoi(argv[2]);

    // percentage of redundancy (0..100)
    // 30% means each number only has 2/3 chance to be a brand new one
    int redundancy=40;
    if( argc > 3)
        redundancy=atoi(argv[3]);

    // we seed the the generator with a constant value so as to get
    // reproducible results.
    srandom(0);

    previous_numbers=malloc(quantity*sizeof(uint64_t));
    
    int i;
    for(i=0; i<quantity; i++)
    {
        if( i==0 || random() % 100 > redundancy)
        {
            // let's generate a new number
            word[0] = random();
            word[1] = random();
            
            // shift right to reduce magnitude
            number >>= 64-magnitude ;
        }
        else
        {
            // let's pick from previously generated numbers
            number = previous_numbers[ random() % i ];
        }
            
        previous_numbers[i] = number;
        printf("%llu\n",(long long)number);
    }
    
    return 0;
}
