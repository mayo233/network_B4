#include<netinet/in.h>
#include<arpa/inet.h>
#include<strings.h>
#include<sys/fcntl.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<netdb.h>
#define PORT 50000

//HTTPの処理を行っている関数
void http(int sockfd){
  int len;
  int read_fd;
  char buf[1024];
  char meth_name[16];
  char uri_addr[256];
  char http_ver[64];
  char *uri_file;
  
  //sockfdはwebクライアントとの間の仮想通信路のソケット番号
  if (read(sockfd, buf, 1024) <= 0 ) {
    printf("error: reading a request.\n");
  }
  else {
    sscanf(buf, "%s %s %s", meth_name, uri_addr, http_ver);
    if (strcmp(meth_name, "GET") != 0) {
      send_msg(sockfd, "501 Not Implemented");
    }
   else{
     uri_file = uri_addr+1;
     //指定ファイルがあったら開く
     if ((read_fd = open(uri_file, O_RDONLY, 0666)) == -1) {
       send_msg(sockfd, "404 Not Found");
     }
     else {
       send_msg(sockfd, "HTTP/1.1 200 OK\r\n");
       send_msg(sockfd, "text/html\r\n");
       send_msg(sockfd, "\r\n");
       while((len = read(read_fd, buf, 1024)) > 0) {
         if (write(sockfd, buf, len) != len) {
           printf("error: writing a response.\n");
           break;
         }
       }
       close(read_fd);
     }
   }
 }
}

//HTTPレスポンスを行う関数
int send_msg(int fd, char *msg) {
  int len;
  len = strlen(msg);
  if ( write(fd, msg, len) != len ){
    printf("error: writing.");
  }
  return len;
}
int main(){
    int sockfd;
    int connectfd;
    struct sockaddr_in addr;
    socklen_t addrlen;
    struct sockaddr_in serv;
    struct hostent *host;
    char buff[65000];
    //ソケットを生成
    sockfd = socket(PF_INET,SOCK_STREAM,0);

    //IPアドレス　ポート番号の設定
    bzero((void*)&serv,sizeof(serv));
    serv.sin_family=AF_INET;
    serv.sin_addr.s_addr=htonl(INADDR_ANY);
    serv.sin_port=htons(PORT);
    if(bind(sockfd,(struct sockaddr *)&serv,sizeof(serv))<0){
        fprintf(stderr,"bind error\n");
        exit(1);
    }
    //コネクション待受
    listen(sockfd,10);
    printf("connecting...\n");

    //コネクション確立 
    addrlen = sizeof(addr);
    while(1){
      if((connectfd=accept(sockfd,(struct sockaddr *)&addr,&addrlen))<0){
          fprintf(stderr,"accept error/n");
          break;
      }
      else{
        http(connectfd);
        close(connectfd);
      }
    }
    close(sockfd);
}