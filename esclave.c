#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "brent-pollard.h"
#include "common.h"

#define NB_THREADS 4
#define NB_OF_SMALL_PRIMES 10000000

static void * handle_requests(void * rawsock)
{
    int sock = *((int *) rawsock);
    int matched;
    msg m;
    memset(&m, 0, sizeof(m));
    m.ctl = CTL_S_READY;
    send(sock, &m, sizeof(m), 0);

    for(;;)
    {
        matched = recv(sock, &m, sizeof(m), 0);

        if(matched == EOF)
        {
            return NULL;
        }

        switch(m.ctl)
        {
        case CTL_M_SENDNUMBER:
            printf("Received number %" SCNu64 "\n", m.nbr);
            m.ctl = CTL_S_RETURNRESULT;
            m.nb_factors = get_prime_factors(m.nbr, m.factors);
            printf("Sending result for number %" SCNu64 "\n", m.nbr);
            send(sock, &m, sizeof(m), 0);
            printf("Result for number %" SCNu64 " sent\n", m.nbr);
            break;
        case CTL_M_NOMORENUMBERS:
            printf("No more numbers for me :(\n");
            return NULL;
        }
    }

}
int main(int argc, char **argv)
{
    for(;;)
    {
        pthread_t threads[NB_THREADS];
        uint16_t i;

        struct sockaddr_in lsa;
        lsa.sin_family = AF_INET;
        lsa.sin_port = htons(COMMON_PORT);
        lsa.sin_addr.s_addr = htonl(INADDR_ANY);
        memset(lsa.sin_zero, '\0', sizeof(lsa.sin_zero));


        int lsock = socket(PF_INET, SOCK_STREAM, 0);

        int yes = 1;
        if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(lsock)) == -1)
        {
            perror("setsockopt SO_REUSEADDR");
            exit(1);
        }

        bind(lsock, (struct sockaddr*)&lsa, sizeof(lsa));
        listen(lsock, 1);

        struct sockaddr_in sa;
        socklen_t sa_size = sizeof(sa);
        int sock = accept(lsock, (struct sockaddr*)&sa, &sa_size);

        if (sock != -1)
        {
            printf("Master connected\n");
        }

        for(i = 0; i < NB_THREADS; i++)
        {
            pthread_create (&threads[i], NULL, handle_requests, &sock);
        }

        for(i = 0; i < NB_THREADS; i++)
        {
            pthread_join(threads[i], NULL);
        }

        msg m;
        printf("Our work is done here, closing shop\n");
        m.ctl = CTL_S_DONE;
        send(sock, &m, sizeof(m), 0);
        close(sock);
        close(lsock);
    }

}

