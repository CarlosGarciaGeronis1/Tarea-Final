#include <stdio.h>
#include <dirent.h>
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

#define PORT 8000
#define SIZE 8
#define MSGSIZE 1024
void serve(int s)
{
    char buffer[MSGSIZE];
    int size, i = 0;
    struct stat buf;
    const char space[2] = " ";
    char *token;
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
            token = strtok(buffer, space);
            while (token != NULL)
            {
                if (indice == 2)
                {
                    if (strcmp(strcpy(filetosend, token), "/") == 0)
                    {
                        strcpy(filetosend, "listoffiles.txt");
                        strcpy(fileType,".txt");
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
                        strcpy(fileType,token+1);
                    }
                }
                indice++;
                token = strtok(NULL, space);
            }
            //==========================================

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
    char *comparacion;
    strcpy(comparacion,fileType);
    char *contiene;
    contiene = strstr(comparacion, ".html");
    if (contiene)
    {
        sprintf("Content-Type:", "text/html\r\n");
        fputs(buffer, sout);
       
        //strcpy(fileType, "Content-Type: text/html");
    }
    char *contiene2;
    contiene2 = strstr(comparacion, ".jpg");
    if (contiene2)
    {
        sprintf("Content-Type:", "image/jpg\r\n");
        fputs(buffer, sout);
        //strcpy(fileType, "Content-Type: image/jpg");
    }
    char *contiene3;
    contiene3 = strstr(comparacion, ".png");
    if (contiene3)
    {
        sprintf("Content-Type:", "image/png\r\n");
        fputs(buffer, sout);
        //strcpy(fileType, "Content-Type: image/png");
    }
    char *contiene4;
    contiene4 = strstr(comparacion, ".txt");
    if (contiene4)
    {
        sprintf("Content-Type:", "text/plain\r\n");
        fputs(buffer, sout);
        //strcpy(fileType, "Content-Type: text/plain");
    }

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
