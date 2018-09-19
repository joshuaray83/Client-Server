/*
    Name:   Josh Heyer
    Class:  CSCI 2240
    Assign: Program #5

    Function: This program interacts with the server program to "get" student
    files sorted by last name, first name, student ID, or GPA. It can create
    new student files using a "put" command. The program can "delete" student
    files based on student ID. Finally, the program can "save" the database
    changes made during its use.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 20637

int inet_addr (char *);
void print ();

int num_of_SRECs = 0;
int x, y, socketid, temp;
char buff[1024];

int main () {

    int command_sent;
    char *command_recv;
    struct sockaddr_in server_address;
    char command[40];
    socklen_t message_size;
    
    socketid = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUM);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset (server_address.sin_zero, '\0', sizeof(server_address.sin_zero));
    message_size = sizeof(server_address);

    connect(socketid, (struct sockaddr *) &server_address, message_size);

    do {
        num_of_SRECs = 0;
        recv(socketid, &x, sizeof(int), 0);
        num_of_SRECs = htonl(x);

        printf ("Enter Command: ");
        fgets(command, 40, stdin);

        switch (command[0]) {

            case 'g':
                if (command[4] == 'l' && command[5] == 'n' && command[6] == 'a' && command[7] == 'm' && command[8] == 'e') {
                    command_sent = 1;
                    send(socketid, &command_sent, sizeof(int), 0);
                    print();
                }
                else if (command[4] == 'f' && command[5] == 'n' && command[6] == 'a' && command[7] == 'm' && command[8] == 'e') {
                    command_sent = 2;
                    send(socketid, &command_sent, sizeof(int), 0);
                    print();
                }
                else if (command[4] == 'S' && command[5] == 'I' && command[6] == 'D') {
                    command_sent = 3;
                    send(socketid, &command_sent, sizeof(int), 0);
                    print();
                }
                else if (command[4] == 'G' && command[5] == 'P' && command[6] == 'A') {
                    command_sent = 4;
                    send(socketid, &command_sent, sizeof(int), 0);
                    print();
                }
                else {
                    printf ("Invalid Command. Try Again.\n");
                    command_sent = 8;
                    send(socketid, &command_sent, sizeof(int), 0);
                }
                break;

            case 'p':
                if (command[1] == 'u' && command[2] == 't') {
                    command_sent = 5;
                    send(socketid, &command_sent, sizeof(int), 0);
                    command_recv = strtok(command, " ,");
                    command_recv = strtok(NULL, " ,");
                    command_sent = 1;
                    while (command_sent < 6) {
                        if (command_recv != NULL) {
                            temp = strlen(command_recv);
                            if (command_sent == 5) {
                                temp -= 1;
                            }
                        }
                        send(socketid, &temp, sizeof(temp), 0);
                        command_sent++;
                        send(socketid, command_recv, strlen(command_recv), 0);
                        command_recv = strtok(NULL, " ,");
                    }
                }
                else {
                    printf ("Invalid Command. Try Again.\n");
                    command_sent = 8;
                    send(socketid, &command_sent, sizeof(int), 0);
                }
                break;

            case 'd':
                if (command[1] == 'e' && command[2] == 'l' && command[3] == 'e' && command[4] == 't' && command[5] == 'e') {
                    command_sent = 6;
                    send(socketid, &command_sent, sizeof(int), 0);
                    command_recv = strtok(command, " ,");
                    command_recv = strtok(NULL, " ,");
                    command_sent = atoi(command_recv);
                    printf("Deleting SID #: %d\n", command_sent);
                    send(socketid, &command_sent, sizeof(int), 0);
                }
                else {
                    printf ("Invalid Command. Try Again.\n");
                    command_sent = 8;
                    send(socketid, &command_sent, sizeof(int), 0);
                }
                break;

            case 's':
                if (command[1] == 'a' && command[2] == 'v' && command[3] == 'e') {
                    command_sent = 7;
                    send(socketid, &command_sent, sizeof(int), 0);
                }
                else {
                    printf ("Invalid Command. Try Again.\n");
                    command_sent = 8;
                    send(socketid, &command_sent, sizeof(int), 0);
                }
                break;

            default:
                printf ("Invalid Command. Try Again.\n");
                command_sent = 8;
                send(socketid, &command_sent, sizeof(int), 0);
                break;
        }
    } while(command_sent != 7);

    return 0;
}

void print () {

    printf ("Number of Records: %d\n", num_of_SRECs);
    printf ("| SID   | Lname     | Fname     | M | GPA  |\n");
    printf ("+-------+-----------+-----------+---+------+\n");
    for (x = 0; x < num_of_SRECs; x++) {
        recv(socketid, buff, 1024, 0);
        printf ("%s\n", buff);
        for (y = 0; y < 1024; y++) {
            buff[y] = '\0';
        }
    }
}
