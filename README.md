dist-prime-fact
===============
  
## What is this ?

This is a distributed prime factorization system, it processes 64 bits integers.  
One PC acts as a master, the other PCs are the slaves (workers).  
The master connects to all workers and sends them numbers to crunch.


## How to

1. Put the numbers you want to factorize in numbers.txt. (1 number per line)  
2. Put all the IPs of the workers in ips.txt. (1 IP per line)  
3. Start the *esclave* program on the workers.  
4. Start the *maitre* program on the master PC.  

## Implementation details
* Communication is done via TCP sockets.
* Basic memoization is done on the master PC in order to avoid factorizing the same numbers.
* Primality test is done via the Miller Rabin algorithm.
* Factorization is done via the Brent-Pollard Rho algorithm.


## Thanks

Thanks to remcobloemen.nl for the math algorithms.

## Bugs/Improvements

This program was written on my lunch break, which means it's very far from perfect.
Feel free to report any bugs or suggestions.
