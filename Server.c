#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

pthread_t StartThread, TerminateThread;

struct Clients
{
    int client_socket;
    struct sockaddr_in client;
    unsigned int status;
} clientsocks[10];

void *Chat(void *arg)
{
    int i, num, value=1;
    char Chatmsg[251], Chatbuffer[251];
    int client_socket = *((int *) arg);
    ssize_t length;

    for(i=0; i<10; i++)
    {
        if(clientsocks[i].status==0)
            continue;
        else if(clientsocks[i].client_socket==client_socket)
            break;
    }

    while(value==1)
    {

        length = recv(client_socket, Chatbuffer, 251, 0);
        //printf("\nChatbuffer: %s\n", Chatbuffer);
        num = Chatbuffer[0];
        Chatmsg[0] = i+1;
        int j=1;
        if(Chatbuffer[1]=='B' && Chatbuffer[2]=='y' && Chatbuffer[3]=='e' && Chatbuffer[4]=='\0')
        {
            printf("Client %d disconnected\n", i+1);
            clientsocks[i].status=0;
            break;
        }
        while(Chatbuffer[j]!='\0')
        {
            Chatmsg[j] = Chatbuffer[j];
            j++;
        }
            
        Chatmsg[j]='\0';
        //printf("\nChatmsg: %s\n", Chatmsg);
        if(clientsocks[num-1].status==0)
            send(client_socket, "C", 1, 0);
        else
        {
            send(clientsocks[num-1].client_socket, Chatmsg, j+1, 0);
        }
    }

    pthread_exit(0);
}

void *Terminate(void *arg)
{
    int server_socket = *((int *) arg);
    int value=1;
    while(value!=0)
    {
        scanf("%d", &value);
    }

    int i;
    for(i=0; i<10; i++)
    {
        if(clientsocks[i].status==0)
            continue;
        printf("Client %d disconnected\n", i+1);
        send(clientsocks[i].client_socket, "S", 1, 0);
        close(clientsocks[i].client_socket);
    }
    close(server_socket);
    exit(9);
}

void *Start(void *arg)
{
    int server_socket = *((int *) arg);
    int value=1, Thread;
    socklen_t addr_len = sizeof(clientsocks[0].client);
    pthread_t ClientThreads[10];

    while(value==1)
    {
        if (listen(server_socket, 10) == -1) 
        {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        int i=0;
        while(i<10)
        {
            if(clientsocks[i].status==0)
            {
                if ((clientsocks[i].client_socket = accept(server_socket, (struct sockaddr*)&clientsocks[i].client, &addr_len)) == -1) 
                {
                    perror("Accept failed");
                    exit(EXIT_FAILURE);
                }
                char Startbuffer[251] = "Welcome Client ";
                int size = strlen(Startbuffer);
                Startbuffer[size-1] = i+1;
                Startbuffer[size] = '\0';
                printf("Client %d connected\n", i+1); 
                clientsocks[i].status=1;                 
                send(clientsocks[i].client_socket, Startbuffer, strlen(Startbuffer), 0);    
                Thread = pthread_create(&ClientThreads[i], NULL, Chat, (void *) &clientsocks[i].client_socket);
                if(Thread!=0)
                    perror("Error occurred while creating thread\n");         
                break;
            }
            else
                i++;
        }
    }

    pthread_exit(0);
}


int main(int argc, char *argv[]) 
{
    int server_socket, client_socket, i, value=1, Thread1, Thread2, Thread, num;
    unsigned int count=0, pos=0;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(clientsocks[0].client);
    char Mainbuffer[251], Mainempty[251];
    ssize_t length;
    pthread_t ClientThreads[10];

    for(i=0; i<10; i++)
        clientsocks[i].status=0;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", atoi(argv[2]));
    Thread1 = pthread_create(&StartThread, NULL, Start, (void *) &server_socket);
    if(Thread1!=0)
        perror("Error occurred while creating thread\n");
    Thread2 = pthread_create(&TerminateThread, NULL, Terminate, (void *) &server_socket);
    if(Thread2!=0)
        perror("Error occurred while creating thread\n");


    while(value==1)
    {
    }

    
    
    return 0;
}




