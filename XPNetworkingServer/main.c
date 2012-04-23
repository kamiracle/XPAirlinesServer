//
//  main.c
//  XPNetworkingServer
//
//  Created by Kyle Miracle on 4/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main( int argc, char **argv )
{
    struct sockaddr_in serverAddress;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int listenfd, connectfd;
    int pid;
    char client_addr_s[INET6_ADDRSTRLEN];
    
    
    if ( (listenfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
        perror( "socket" );
        exit(1);
    }
    
    bzero( &serverAddress, sizeof(serverAddress) );
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = htonl( INADDR_ANY );
    
    if ( bind( listenfd, (struct sockaddr *)&serverAddress, 
              sizeof(serverAddress) ) < 0 ) {
        perror( "bind" );
        exit(1);
    }
    
    if ( listen( listenfd, 5 ) < 0 ) {
        perror( "listen" );
        exit(1);
    }
    
   /*
    * Loop forever until server it stopped with Control-C
    */
    
    printf("Server: started and waiting for connections...\n");
    
    while(1)
    {
    
        char input_buffer[256];
        bzero(input_buffer, 256);
        
        sin_size = sizeof(client_addr);
        
        if ( (connectfd = accept( listenfd, 
                                 (struct sockaddr *)&client_addr, &sin_size )) < 0 ) {
            perror( "accept" );
            exit(1);
        }
        
        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *)&client_addr),
                  client_addr_s, sizeof client_addr_s);
        
        printf("Server: got connection from %s\n", client_addr_s);
        
        /* Fork off child process to allow multiple connections */
        pid = fork();
        if ( pid < 0 ) {
            perror("fork");
            exit(1);
        }
        if ( pid == 0 ) 
        {            
            if ( read( connectfd, input_buffer, 255 ) < 0 ) {
                perror( "read" );
                exit(1);
            }
            printf("pid=%d listenfd:%d connectfd: %d: %s\n", pid, listenfd, connectfd, input_buffer);
        }
        else 
        {
            close(connectfd);
        }
                
    } /* End of while */
}