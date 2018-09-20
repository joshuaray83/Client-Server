/*
    Name:   Josh Heyer
    Email:  joshuaray83@gmail.com

    Function: This program interacts with the client program to edit and save
    information from a database of students. Each student has a last name,
    middle initial, first name, student ID, and GPA.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 20637

int strcmp (const char *, const char *);
void send_sortedSRECs ();
void get_by_lname ();
void get_by_fname ();
void get_by_SID ();
void get_by_GPA ();
void put ();
void delete ();
void save ();
int inet_addr (char *);

typedef struct student {

    char lname[10];
    char initial;
    char fname[10];
    unsigned long SID;
    float GPA;

} SREC;

char LNAME[10], FNAME[10], INITIAL[1], STID[5], STGPA[4], extra[10];
SREC array[1024];
int num_of_SRECs = 0, x = 0, slen = 0, temp, y, command_recv, iteration = 0;
int temp1, temp2, temp3, temp4;
SREC new, new1, temp_SREC;
FILE *file;
int socketid1, socketid2;
char buff[1024];
struct sockaddr_in server_address;
struct sockaddr_storage space;
socklen_t message_size;
char *ptr;

int main () {

    socketid1 = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUM);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));

    bind(socketid1,(struct sockaddr *) &server_address,sizeof(server_address));
    message_size = sizeof(space);

    if (listen(socketid1, 5) != 0) {

        printf("Error while listening. Shutting down server.\n");
        exit(0);
    }

    socketid2 = accept(socketid1, (struct sockaddr *) &space, &message_size);

    do {

        printf("Awaiting Command\n");
        iteration++;

        file = fopen ("database.dat", "r+");
        if (file == NULL) {
            file = fopen ("database.dat", "w");
            if (file == NULL) {
                printf ("File Creation Error. Exiting Program.\n");
                exit(0);
            }
        }
        while (fread (&new1, sizeof(SREC), 1, file) == 1) {
            array[num_of_SRECs] = new1;
            if (iteration == 1) {
                num_of_SRECs++;
            }
        }

        fclose(file);

/* This section is to manually generate SRECs. Only needed for testing. */

        /*new.lname[0] = 'H'; new.lname[1] = 'e'; new.lname[2] = 'y';
        new.fname[0] = 'J'; new.fname[1] = 'o'; new.fname[2] = 's';
        new.initial = 'R'; new.GPA = 3.2; new.SID = 12345;
        array[0] = new;

        new.lname[0] = 'H'; new.lname[1] = 'e'; new.lname[2] = 'y';
        new.fname[0] = 'S'; new.fname[1] = 't'; new.fname[2] = 'e';
        new.initial = 'R'; new.GPA = 4.0; new.SID = 432;
        array[1] = new;

        new.lname[0] = 'K'; new.lname[1] = 'r'; new.lname[2] = 'y';
        new.fname[0] = 'M'; new.fname[1] = 'a'; new.fname[2] = 'd';
        new.initial = 'B'; new.GPA = 3.8; new.SID = 54221;
        array[2] = new;

        num_of_SRECs = 3;*/

        temp = htonl(num_of_SRECs);
        send(socketid2, &temp, sizeof(int), 0);

        recv(socketid2, &command_recv, sizeof(int), 0);
        
        switch (command_recv) {

            case 1:
                get_by_lname();
                break;

            case 2:
                get_by_fname();
                break;

            case 3:
                get_by_SID();
                break;

            case 4:
                get_by_GPA();
                break;

            case 5:
                put();
                break;

            case 6:
                delete();
                break;

            case 7:
                save();
                socketid2 = accept(socketid1, (struct sockaddr *) &space, &message_size);
                break;

            case 8:
                break;
        }
    } while(1);

    return 0;
}

void send_sorted_SRECs () {

    for (x = 0; x < num_of_SRECs; x++) {
    
        slen = 0;    
        slen += sprintf(buff+slen, "| %05lu | ", array[x].SID);
        slen += sprintf(buff+slen, "%-9s | ", array[x].lname);
        slen += sprintf(buff+slen, "%-9s | ", array[x].fname);
        slen += sprintf(buff+slen, "%c | ", array[x].initial);
        slen += sprintf(buff+slen, "%.2f |", array[x].GPA);

        send(socketid2, buff, 1024, 0);
        
        for (y = 0; y < 1024; y++) {
            buff[y] = '\0';
        }
    }
}

void get_by_lname () {

    for (x = 0; x < num_of_SRECs - 1; x++) {
        for (y = 0; y < num_of_SRECs - x - 1; y++) {
            if (strcmp(array[y].lname, array[y+1].lname) > 0) {
                temp_SREC = array[y];
                array[y] = array[y+1];
                array[y+1] = temp_SREC;
            }
        }
    }
    send_sorted_SRECs();
}

void get_by_fname () {

    for (x = 0; x < num_of_SRECs - 1; x++) {
        for (y = 0; y < num_of_SRECs - x - 1; y++) {
            if (strcmp(array[y].fname, array[y+1].fname) > 0) {
                temp_SREC = array[y];
                array[y] = array[y+1];
                array[y+1] = temp_SREC;
            }
        }
    }
    send_sorted_SRECs();
}

void get_by_SID () {

    for (x = 0; x < num_of_SRECs - 1; x++) {
        for (y = 0; y < num_of_SRECs - x - 1; y++) {
            if (array[y].SID > array[y+1].SID) {
                temp_SREC = array[y];
                array[y] = array[y+1];
                array[y+1] = temp_SREC;
            }
        }
    }
    send_sorted_SRECs();
}

void get_by_GPA () {

    for (x = 0; x < num_of_SRECs - 1; x++) {
        for (y = 0; y < num_of_SRECs - x - 1; y++) {
            if (array[y].GPA < array[y+1].GPA) {
                temp_SREC = array[y];
                array[y] = array[y+1];
                array[y+1] = temp_SREC;
            }
        }
    }
    send_sorted_SRECs();
}

void put () {
    
    recv(socketid2, &temp, sizeof(temp), 0);
    recv(socketid2, LNAME, temp, 0);
    recv(socketid2, &temp, sizeof(temp), 0);
    recv(socketid2, FNAME, temp, 0);
    recv(socketid2, &temp, sizeof(temp), 0);
    recv(socketid2, INITIAL, temp, 0);
    recv(socketid2, &temp, sizeof(temp), 0);
    recv(socketid2, STID, temp, 0);
    recv(socketid2, &temp, sizeof(temp), 0);
    recv(socketid2, STGPA, temp+1, 0);
    strcpy(array[num_of_SRECs].lname, LNAME);
    strcpy(array[num_of_SRECs].fname, FNAME);
    array[num_of_SRECs].initial = INITIAL[0];
    array[num_of_SRECs].SID = atoi(STID);
    array[num_of_SRECs].GPA = atof(STGPA);
    num_of_SRECs++;
}

void delete () {

    recv(socketid2, &temp, sizeof(y), 0);
    for (x = 0; x < num_of_SRECs; x++) {
        if (array[x].SID == temp) {
            printf ("Deleting SID #: %d\n", temp);
            array[x] = array[num_of_SRECs-1];
            for (y = 0; y < 10; y++) {
                array[num_of_SRECs-1].lname[y] = '\0';
                array[num_of_SRECs-1].fname[y] = '\0';
            }
            array[num_of_SRECs].initial = '\0';
            array[num_of_SRECs-1].SID = 0;
            array[num_of_SRECs-1].GPA = 0;
            num_of_SRECs--;
        }
        else {
            printf ("SID # Not Found\n");
        }
    }
}

void save () {

    file = fopen ("database.dat", "w");
    if (file == NULL) {
        printf ("File Creation Error. Exiting Program.\n");
        exit(0);
    }
    else {
        for (x = 0; x < num_of_SRECs; x++) {
            fwrite(&array[x], sizeof(array[x]), 1, file);
        }
        fclose(file);
    }
}

