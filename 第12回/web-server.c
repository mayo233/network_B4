#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // バイトオーダ変換用
#include <strings.h>  // bzero()用
#define PORT 50000
int i;
void http(int sockfd) 

{

  int len;

  int read_fd;

  char buf[1024];

  char meth_name[16];

  char uri_addr[256];

  char http_ver[64];

  char *uri_file;



  if (read(sockfd, buf, 1024) <= 0 ) {

    fprintf(stderr,"error: reading a request.\n");

  }

  else {

    sscanf(buf, "%s %s %s", meth_name, uri_addr, http_ver);

    if (strcmp(meth_name, "GET") != 0) {

      send_msg(sockfd, "501 Not Implemented");

    }

    else {

      uri_file = uri_addr+1;
      
      if ((read_fd = open(uri_file, O_RDONLY, 0666)) == -1) {

        	send_msg(sockfd, "404 Not Found");

      }

      else {

          send_msg(sockfd, "HTTP/1.0 200 OK\r\n");

          send_msg(sockfd, "text/html\r\n");

          send_msg(sockfd, "\r\n");

	while((len = read(read_fd, buf, 1024)) > 0) {

	  if (write(sockfd, buf, len) != len) {

	    fprintf(stderr,"error: writing a response.\n");

	    break;

	  }

	}

	close(read_fd);

      }

    }

  }	

}

int send_msg(int fd, char *msg) {

  int len;

  len = strlen(msg);

  if ( write(fd, msg, len) != len ){

    fprintf(stderr, "error: writing.");

  }

  return len;

}

int main(){
  int sockfd; // 待受用ソケット（ソケットファイルディスクリプタ）
  int connectfd;  // 通信用ソケット
  struct sockaddr_in serv;  // アドレス情報構造体
  struct sockaddr_in addr;  // 接続相手（クライアント）のアドレス情報格納用
  socklen_t addrlen;
  // pid_t childpid; // fork用
  char buff[1024];

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
    perror("socket() failed.");
    exit(1);
  }

  bzero((void *)&serv, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = htonl(INADDR_ANY);
  serv.sin_port = htons(PORT);  // ホストの任意アドレスに設定

  if (bind(sockfd, (struct sockaddr *) &serv, sizeof(serv)) < 0){
    fprintf(stderr,"bind error\n");
    exit(1);
  } 
  listen(sockfd, 10);
  printf("listening...\n");

  char buff0[1024];
  char buff1[1024];
  char host[1024], port[1024], path[1024];
  addrlen = sizeof(addr);
  //for (i=0; i<=10; i++){
  if ((connectfd = accept(sockfd, (struct sockaddr *) &addr, &addrlen)) < 0){
    //printf("aaaaa");
    http(connectfd);
    //printf("aaaa");
    fprintf(stderr,"accept error\n");
    exit(1);
    //close(connectfd);
  }
  //}
  
  //recv(connectfd, buff0, sizeof(buff0), 0);  // ソケットから受信
  //fgets(buff,sizeof(buff),stdin);
  
  sscanf(buff0, "%s %s %s", host, port, path);
  printf("GET %s HTTP/1.1\n", path);
  printf("Host: %s\n\n", host);

  sprintf(buff1, "HTTP/1.1 200 OK \nContent-Lnegth: \nConnection: close\nContent-Type: text/html");
  printf("%s\n", buff1);
  
 //send(buff,buff,strlen(buff),0);
  send(connectfd, buff1, strlen(buff1), 0);  // ソケットへ送信

}