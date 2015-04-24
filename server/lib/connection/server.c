#include "./../../include/server.h"

/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/


void  writeTimeDelay(long long delayTime){
    FILE *fp;
    char dd[100];
    fp = fopen("./timeDellay.txt", "a+W" );
    if (fp==NULL)
    {
        printf("Error al abrir el archivo \n");
    }
    //printf("\t%lld\t%lld\n", current_timestamp(),delayTime);
    sprintf(dd,"\t%lld\t%lld\n", current_timestamp(),delayTime);
    fputs(dd,fp);

    fflush(fp);
    fclose(fp);
}

/******** sendTimeToStart() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established and sends
 the time that the superwav player has to start.
 *****************************************/
void sendTimeToStart(int sock, long long timeToStart){
    long long int   timer       =   timeToStart;
    char            string_time[64];/*Descubrir tamaño aproximado*/
    sprintf( string_time, "%lld", timer );
    //printf("Sending time %s, to socket %d \n",string_time, sock);
    Send(sock,&string_time,strlen(string_time),0);
}

void sendFlag(int sock, int flag){
    char    stringflag[64];
    sprintf( stringflag, "%d", flag );
    //printf("Flag %s,sended to socket %d \n",stringflag, sock);
    Send(sock,&stringflag,strlen(stringflag),0);

}

/* **********************************************************************
* Returns a struct with the server configuration needed,
* so in other parts of the code you can be able to send and recive messeges
* *********************************************************************** */
ServerConnection startConfigurationServer(int portNumber){
    int opt = TRUE;

    ServerConnection server;
    server.socketFileDescriptor = Socket(AF_INET, SOCK_STREAM, 0);

    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt( server.socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    server.max_clients = 30;
    //initialise all client_socket[] or newFileDescriptor to 0 so not checked
    int i;
    for (i = 0; i < server.max_clients; i++)
    {
        server.newSocketFileDescriptor[i] = 0;
    }

    //fcntl(server.socketFileDescriptor, F_SETFL, O_NONBLOCK);

    bzero((char *) &server.serv_addr, sizeof(server.serv_addr));

    server.serv_addr.sin_family = AF_INET;
    server.serv_addr.sin_addr.s_addr = INADDR_ANY;
    server.serv_addr.sin_port = htons(portNumber);

    Bind(server.socketFileDescriptor, (struct sockaddr *) &server.serv_addr, sizeof(server.serv_addr));

    Listen(server.socketFileDescriptor,5);
    printf("Waiting for connections ...\n");
    server.clientLenght = sizeof(server.cli_addr);

    return server;
}

void notifyClients(ServerConnection server, long long timeToStart, int flag){
    int i;
    printf("Time to start: %lld\n",timeToStart);
    for (i = 0; i < server.max_clients; i++)
    {
        if (server.newSocketFileDescriptor[i]!= 0){
            //printf("Connection, cont = %d, %d \n",i, server.newSocketFileDescriptor[i]);

            if (timeToStart > 0){
                sendTimeToStart(server.newSocketFileDescriptor[i],timeToStart);
            }/*else{
                printf("\n¡¡No envío timestamp!!\n");
            }*/
            sleep(1);
            sendFlag(server.newSocketFileDescriptor[i],flag);
        }
    }
}


/**********************************************************************/
/* START SERVER */
/**********************************************************************/

int startServerConnection(int portNumber, int flag){
    int sd,max_sd;
    int cont = 0;
    int server_flag = flag;
    int client_flag = changeFlag(flag);
    int playing = FALSE;

    ServerConnection server = startConfigurationServer(portNumber);

    /* For select */
    fd_set readfds;
    struct timeval timeout;
    int rv;
    //clear the socket set
    FD_ZERO(&readfds);
    //add our file descriptor to the set
    FD_SET(server.socketFileDescriptor, &readfds);
    max_sd = server.socketFileDescriptor;
    cont = cont+1;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    /* ************************************* */

    while(1) {
        int search = TRUE;

        if(search){
            sd = server.newSocketFileDescriptor[cont];

            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;

            rv = Select(server.socketFileDescriptor + cont, &readfds, NULL, NULL, &timeout);

            if (rv == -1) {
                perror("select error"); /// an error accured
                return 1;
            }
            else if (rv == 0) {
                //printf(" ************************** timeout occurred (1 second) **************************\n"); // a timeout occured
                //return 1;
                timeout.tv_sec = 1;
                timeout.tv_usec = 0;

                /*Hay que inicializar estos datos cada vez que se utiliza resetea la configuración*/
                //add our file descriptor to the set
                FD_SET(server.socketFileDescriptor, &readfds);
                /**/
            }
            else {
                if (FD_ISSET(server.socketFileDescriptor, &readfds)) {
                    server.newSocketFileDescriptor[cont] = Accept(server.socketFileDescriptor,
                                                                  (struct sockaddr *) &server.cli_addr,
                                                                  &server.clientLenght);
                    //inform user of socket number - used in send and receive commands
                    printf("New connection , socket fd is %d , ip is : %s , port : %d \n",
                           server.newSocketFileDescriptor[cont], inet_ntoa(server.cli_addr.sin_addr),
                           ntohs(server.cli_addr.sin_port));
                }
                cont += 1;
            }
        }

        if (kbhit()) {
            char c = getchar();
            int pID;
            switch (c) {
                case 'a':
                    printf("Iniciating client");
                    pID = fork();
                    if (pID == 0)                // child
                    {
                        char stringPortNumber[sizeof(int)*3+4];
                        snprintf(stringPortNumber, sizeof stringPortNumber, "%d", portNumber);
                        printf("%s",stringPortNumber);
                        //execl ("/bin/pwd", "pwd", NULL);
                        char *aruments[4] = {"./../client/SuperWavAppClient", "localhost", stringPortNumber, NULL };
                        execv("./../client/SuperWavAppClient", aruments);
                        exit(-1);

                    }
                    else if (pID < 0)            // failed to fork
                    {
                        printf("Error fork client");
                        exit(1);
                    }else{
                        break;
                    }

                case 's':
                    if(playing){
                        printf("\nAlready playing!\n");
                    }else{
                        search = FALSE;
                        playing = TRUE;
                        printf("\nSTART Music in 10 seconds !\n");
                        long long timeToStart = timeToStartInSeconds(10);
                        // clients of time to start
                        notifyClients(server, timeToStart,client_flag);
                    }
                    break;

                case 'p':

                    client_flag = changeFlag(client_flag);
                    server_flag = changeFlag(server_flag);

                    notifyClients(server,0,client_flag);

                    break;
                case 'e':
                    notifyClients(server,0,-1);
                    exit(0);
                default:
                    printf("\n Has presionado %c\n", c);

            }
        }
    }
    return 0;

}

/**********************************************************************/
/* FIN SERVER */
/**********************************************************************/