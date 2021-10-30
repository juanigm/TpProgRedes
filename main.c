#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_IMGS 4
#define HEADERS "HTTP/1.1 200 OK\nContent-Type: image/png\nContent-Length: %lu" "\n\n"

size_t getFilesize(const char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;   
}

int sendFile(int socket, const char* path, long fileSize) {

    char* string;
    asprintf(&string, HEADERS, getFilesize(path));
	send(socket, string , strlen(string) , 0 );

    long bytesSent = 0, result = 0, offset = 0;
    int file = open(path, O_RDONLY);

    if(fileSize == -1 || file == -1) {
        return -1;
    }

    do {
        result = sendfile(socket, file, &offset, fileSize - bytesSent);
        if(result < 0) {
            tf("Socket: No se pudo encontrar el file '%s' en el soprincket %d\n", path, socket);
            break;
        }
        bytesSent += result;
    } while(bytesSent < fileSize || result < 0);
    
    free(string);
    close(file);

    return result;
}

void *atenderCliente(void* socket) {
    const char *a[1];
    a[0] = "image1.jpg";
    
    sendFile(*(int*)socket,a[0],(char*)malloc(sizeof(a[0])));
    printf("response enviado\n"); 
}


int main(int argc, char *argv[])
{
    int listenfd, connfd = 0;
    pthread_t thread1;
    struct sockaddr_in serv_addr;
    int puerto;
    int addrlen = sizeof(serv_addr); 
    struct pollfd fds[2];
    fds[0].events = POLLIN;
    int opt = 1;    

    char sendBuff[1024] = {0};

    puerto = atoi(argv[1]);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if(listenfd == 0){
        perror("socket failed"); 
		exit(EXIT_FAILURE); 
    }

    fds[0].fd = listenfd;
    
    

    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt"); 
		exit(EXIT_FAILURE); 
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(puerto);


    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        perror("bind failed"); 
		exit(EXIT_FAILURE);
    }

    if(listen(listenfd, 10) < 0){
        perror("listen"); 
		exit(EXIT_FAILURE); 
    }
    
    printf("listening port %d...\n", puerto);
    
    while(1)
    {
        int rv = poll(fds, 1, 3500);

        if (rv == -1) {
            fprintf(stderr,"Ocurrio un error en el poll: %s\n", strerror(errno));
            return 1;
        }

        if(fds[0].revents & POLLIN){

            connfd = accept(listenfd, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
            
            if(connfd < 0) {
                perror("accept"); 
                exit(EXIT_FAILURE); 
            }

            pthread_create(&thread1, NULL, atenderCliente, (void*) &connfd);
        
        }
    
    }

    return 0;

}



