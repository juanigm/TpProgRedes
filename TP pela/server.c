#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <dirent.h>

#define IMAGE_PATH "alf.jpg"
#define PORT 8001
#define HEADERS "HTTP/1.1 200 OK\nContent-Type: image/png\nContent-Length: %lu" "\n\n"
#define MAX_IMGS 4


size_t getFilesize(const char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;   
}

int sendFile(int socket, const char* path, long fileSize) {

    char* string;
    //asprintf(&string, HEADERS, getFilesize(path));
	send(socket, string , strlen(string) , 0 );

    long bytesSent = 0, result = 0, offset = 0;
    int file = open(path, O_RDONLY);

    if(fileSize == -1 || file == -1) {
        return -1;
    }

    do {
        result = sendfile(socket, file, &offset, fileSize - bytesSent);
        if(result < 0) {
            printf("Socket: No se pudo encontrar el file '%s' en el socket %d\n", path, socket);
            break;
        }
        bytesSent += result;
    } while(bytesSent < fileSize || result < 0);
    
    free(string);
    close(file);

    return result;
}

void *SocketThread(void* socket) {
    const char *a[MAX_IMGS];
    a[0] = "imgs/car1.jpg";
    a[1] = "imgs/car2.jpg";
    a[2] = "imgs/nice.jpg";
    a[3] = "imgs/supercar.jpg";

    srand(time(0));
    int r = rand() % 4;
    
    sendFile(*(int*)socket,a[r],(char*)malloc(sizeof(a[r])));
    printf("response enviado\n"); 
}

int main(int argc, char const *argv[]) {
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
    pthread_t thread; 
    struct pollfd fds[2];
    fds[0].events = POLLIN;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

    fds[0].fd = server_fd;
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}

	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 
	
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}

    while(1==1) {
        int ret = poll(fds, 1, 5000);

        if (ret == -1) {
            fprintf(stderr,"Error al intentar agregar al poll: %s\n", strerror(errno));
            return 1;
        }
        
        if (fds[0].revents & POLLIN) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                (socklen_t*)&addrlen))<0) 
            { 
                perror("accept"); 
                exit(EXIT_FAILURE); 
            } 
            pthread_create(&thread, NULL, SocketThread, (void*) &new_socket);
        }
    }     
	
	return 0; 
}