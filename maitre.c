#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common.h"

#define NUMBERS_FILE "numbers.txt"
#define IPS_FILE "ips.txt"
#define MAX_WORKERS 256


static pthread_mutex_t mtx_resultat;
static resultat resultats[QUANTITY];
static int nbResultats;

static pthread_mutex_t mtx_scanf;
static pthread_mutex_t mtx_printf;
static FILE * numbersTxt;

static void print_msg_factors(msg * m)
{
    pthread_mutex_lock(&mtx_printf);
    printf("%" SCNu64 ": ", m->nbr);
    uint16_t i = 0;
    for(i = 0; i<m->nb_factors; i++)
    {
        printf("%" SCNu64 " ", m->factors[i]);
    }
    printf("\n");
    pthread_mutex_unlock(&mtx_printf);
}

static int get_next_number(uint64_t * n)
{
    int matched;

    pthread_mutex_lock(&mtx_scanf);
    matched = fscanf(numbersTxt, "%" SCNu64 "\n", n);
    pthread_mutex_unlock(&mtx_scanf);

    return matched;
}

static void send_next_number(int sock, msg * m)
{
    uint64_t n = 0;
    if(get_next_number(&n) != EOF)
    {
        pthread_mutex_lock(&mtx_resultat);
        int i;
        for(i = 0; i < nbResultats; i++)
        {
            if(resultats[i].n == n)
            {
                memcpy(m->factors, resultats[i].factors, MAX_FACTORS * sizeof(uint64_t));
                m->nb_factors = resultats[i].nb_factors;
            }
        }
        pthread_mutex_unlock(&mtx_resultat);

        if(n == 0) {
            print_msg_factors(m);
        }
        else {
            m->ctl = CTL_M_SENDNUMBER;
            m->nbr = n;
            send(sock, m, sizeof(msg), 0);
        }
    }
    else
    {
        m->ctl = CTL_M_NOMORENUMBERS;
        m->nbr = 0;
        send(sock, m, sizeof(msg), 0);
    }
}
static void * handle_slave(void * rawip)
{
    char * ip = (char *) rawip;

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(COMMON_PORT);
    sa.sin_addr.s_addr = inet_addr(ip);
    memset(sa.sin_zero, '\0', sizeof(sa.sin_zero));

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Could not create socket!\n");
    }
    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) == -1)
    {
        printf("Error connecting to %s\n", ip);
    }

    msg m;

    for(;;)
    {
        if(recv(sock, &m, sizeof(m), 0) <= 0)
        {
            m.ctl = CTL_S_DONE;
        }

        switch(m.ctl)
        {
        case CTL_S_READY:
            send_next_number(sock, &m);
            break;
        case CTL_S_RETURNRESULT:
            pthread_mutex_lock(&mtx_resultat);
            resultats[nbResultats].n = m.nbr;
            memcpy(resultats[nbResultats].factors, m.factors, MAX_FACTORS * sizeof(uint64_t));
            resultats[nbResultats].nb_factors = m.nb_factors;
            nbResultats++;
             pthread_mutex_unlock(&mtx_resultat);
    

            print_msg_factors(&m);
            send_next_number(sock, &m);
            break;
        case CTL_S_DONE:
            close(sock);
            return NULL;
        }
    }
    free(ip);
}

int main()
{
    nbResultats = 0;
    pthread_mutex_init(&mtx_scanf, NULL);
    pthread_mutex_init(&mtx_printf, NULL);
    numbersTxt = fopen(NUMBERS_FILE, FOPEN_MODE);

    uint16_t workers = 0;
    pthread_t threads[MAX_WORKERS];
    char ip[15];
    FILE * ips = fopen(IPS_FILE, FOPEN_MODE);
    while(fscanf(ips, "%s\n", ip)!=EOF)
    {
        if(workers == MAX_WORKERS)
        {
            break;
        }

        char * copy = malloc(15 * sizeof(copy));
        memcpy(copy, ip, 15 * sizeof(copy));

        pthread_create (&threads[workers], NULL, handle_slave, copy);
        workers++;
    }

    uint16_t i;
    for(i = 0; i < workers; i++)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mtx_scanf);
    pthread_mutex_destroy(&mtx_printf);

    return 0;
}

