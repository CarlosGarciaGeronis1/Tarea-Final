#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define PORT 8000
#define SIZE 8
#define MSGSIZE 1024
void serve(int s)
{
    char buffer[MSGSIZE];
    int size, i = 0;
    struct stat buf;
    const char espacio[2] = " ";
    const char punto[2] = ".";
    char *token;
    char *token2;
    char filetosend[100];
    char fileType[100];
    FILE *sin = fdopen(s, "r");
    FILE *sout = fdopen(s, "w");
    int status;

    // Reads the request from the client
    while (fgets(buffer, MSGSIZE, sin) != NULL)
    {
        printf("%d - [%s]\n", ++i, buffer);
        // A blank line is found -> end of headers
        if (i == 1)
        {
            int indice = 1;
            token = strtok(buffer, espacio);
            while (token != NULL)
            {
                if (indice == 2)
                {
                    if (strcmp(strcpy(filetosend, token), "/") == 0)
                    {
                        strcpy(filetosend, "listoffiles.txt");
                        strcpy(fileType, ".txt");
                        char *filename = "listoffiles.txt";
                        FILE *fp = fopen(filename, "w");
                        if (fp == NULL)
                        {
                            printf("Error opening the file %s", filename);
                            return -1;
                        }
                        struct dirent *de;
                        DIR *dr = opendir(".");
                        if (dr == NULL)
                        {
                            printf("Error opening the file %s", filename);
                            printf("Could not open current directory");
                            return 0;
                        }
                        while ((de = readdir(dr)) != NULL)
                        {
                            fprintf(fp, "%s\n", de->d_name);
                        }
                        fclose(fp);
                        closedir(dr);
                    }
                    else
                    {
                        strcpy(filetosend, token + 1);
                    }
                }
                indice++;
                token = strtok(NULL, espacio);
            }
            //==========================================
            char *contiene;
            contiene = strstr(filetosend, ".html");
            if (contiene)
            {
                strcpy(fileType,"Content-Type: text/html");
            }
            char *contiene2;
            contiene2 = strstr(filetosend, ".jpg");
            if (contiene2)
            {
                strcpy(fileType,"Content-Type: image/jpg");
            }
            char *contiene3;
            contiene3 = strstr(filetosend, ".png");
            if (contiene3)
            {
                strcpy(fileType,"Content-Type: image/png");
            }
            char *contiene4;
            contiene4 = strstr(filetosend, ".txt");
            if (contiene4)
            {
                strcpy(fileType,"Content-Type: text/plain");
            }
            //==========================================
        }
        if (buffer[0] == '\r' && buffer[1] == '\n')
        {
            break;
        }
    }

    // Builds response
    sprintf(buffer, "HTTP/1.0 200 OK\r\n");
    fputs(buffer, sout);

    sprintf(buffer, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
    fputs(buffer, sout);

    sprintf("%s\r\n",fileType);
    fputs(buffer, sout);
    
    stat(filetosend, &buf);
    printf("Size -----------> %d\n", (int)buf.st_size);

    sprintf(buffer, "Content-Length: %d\r\n", (int)buf.st_size);
    fputs(buffer, sout);

    sprintf(buffer, "\r\n");
    fputs(buffer, sout);

    FILE *fin = fopen(filetosend, "r");
    while ((size = fread(buffer, 1, MSGSIZE, fin)) != 0)
    {
        size = fwrite(buffer, 1, size, sout);
    }

    fflush(0);
}

int main()
{
    int sd, sdo, size, r;
    struct sockaddr_in sin, pin;
    socklen_t addrlen;

    // 1. Crear el socket
    sd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);

    // 2. Asociar el socket a IP:port
    r = bind(sd, (struct sockaddr *)&sin, sizeof(sin));
    if (r < 0)
    {
        perror("bind");
        return -1;
    }
    // 3. Establecer backlog
    listen(sd, 5);

    addrlen = sizeof(pin);
    // 4. Esperar conexion
    while ((sdo = accept(sd, (struct sockaddr *)&pin, &addrlen)) > 0)
    {
        if (!fork())
        {
            printf("Connected from %s\n", inet_ntoa(pin.sin_addr));
            printf("Port %d\n", ntohs(pin.sin_port));

            serve(sdo);

            close(sdo);
            exit(0);
        }
    }
    close(sd);

    sleep(1);
}
