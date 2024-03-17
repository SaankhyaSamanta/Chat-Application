#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <error.h>

void* Chat(void *arg)
{
    int client_socket = *((int*) arg);
    int value=1, flag=1, num, i;
    ssize_t length;
    char Chatbuffer[251], Chatempty[251], Chatmsg[251];
    
    while(value==1)
    {
            //printf("Myself: \n");
            printf("Enter client to send message to: ");
            scanf("%d", &num);
            printf("Enter message: ");
            fgets(Chatempty, 251, stdin);
            fgets(Chatbuffer, 251, stdin);
            i=0;
            Chatmsg[0] = num;
            //printf("\nChatbuffer: %s\n", Chatbuffer);
            while(Chatbuffer[i]!='\n')
            {
                Chatmsg[i+1] = Chatbuffer[i];
                i++;
            }
            Chatmsg[i+1] = '\0';
            //printf("\nChatmsg: %s\n", Chatmsg);
            send(client_socket, Chatmsg, i+2, 0);
            if(Chatbuffer[0]=='B' && Chatbuffer[1]=='y' && Chatbuffer[2]=='e' && Chatbuffer[3]=='\n')
                break;
    }

    close(client_socket); 

    exit(9);
}

int main(int argc, char *argv[]) {
    int client_socket, i, value=1, Thread, num, NUM;
    struct sockaddr_in server_addr;
    ssize_t length;
    char Mainbuffer[251], Mainempty[251];;
    pthread_t thread;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    length = recv(client_socket, Mainbuffer, 251, 0);
    num = (int)Mainbuffer[length-1];
    Mainbuffer[length-1] = '\0';
    printf("%s %d\n", Mainbuffer, num);
    NUM = num;


    Thread = pthread_create(&thread, NULL, Chat, (void *) &client_socket);
    if(Thread!=0)
    {
        perror("Thread Creation Failed");
        close(client_socket);
    }

    while(value==1)
    {
        length = recv(client_socket, Mainbuffer, 251, 0);
        //printf("\nMainbuffer: %s\n",  Mainbuffer);
        num = Mainbuffer[0];
        char Mainmsg[251];

        if(length<=0)
        {
                //printf("Server Disconnected\n");
                break;
        }
        i=1;
        while(Mainbuffer[i]!='\0')
        {
            Mainmsg[i-1] = Mainbuffer[i];
            i++;
        }
        Mainmsg[i-1] = '\0';
        if(num==83) //ASCII CODE OF S
            printf("\nServer disconnected\n");
        else if(num==67) //ASCII CODE OF C
            printf("\nClient not found, Message not sent\n");
        else 
            printf("\n\t\t\t\t\tClient %d: %s\n", num, Mainmsg);            
        
        //printf("Myself: \n");
        

    }

    close(client_socket);


    return 0;
}

