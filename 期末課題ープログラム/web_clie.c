#include<netinet/in.h>
#include<arpa/inet.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<netdb.h>
char buff[65000];
#define PORT 50000
int main(int argc,char *argv[]){
    int sockfd;
    int port;
    struct sockaddr_in serv;
    struct hostent *host;
    char hostname[65536];
    char path[65536];
    char *p;
    //コマンドラインからホスト名を取得
    //URIの処理例より
    if(sscanf(argv[1],"http://%s",hostname)<=0){
      printf("Address error\n");
    };
    //ホスト名から"/"までを抜き出す
    p=strchr(hostname,'/');
    //パスを取得
    strcpy(path,p);
    *p='\0';
    p=strchr(hostname,':');
    port = atoi(p+1);
    if ( port <= 0 ){        
      port = 80;           //ポート番号を80にする
    }
    *p='\0';
    printf("host:%s\npass:%s\nport num:%d\n",hostname,path,port);
    /*ホストの情報を取得*/
    struct hostent *servhost;
    servhost=gethostbyname(hostname);
    if(servhost==NULL){
      printf("IP addless error :%s",hostname);
      return 0;
    }
    /* 構造体をゼロクリア */
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    /* IPアドレスを示す構造体をコピー */
    bcopy(servhost->h_addr, &serv.sin_addr, servhost->h_length);
    serv.sin_port = htons(port);
    /*ソケットの生成 socket()*/
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    /*コネクション確立 connect()*/
    if(connect(sockfd,(struct sockaddr *)&serv,sizeof(serv))<0){
        fprintf(stderr,"connect error\n");
        exit(1);
    }
    /* HTTP プロトコル生成 & サーバに送信 */
    sprintf(buff, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, hostname);
    send(sockfd, buff, strlen(buff),0);

    /*データの送受信*/
    while(1){
        int size = recv(sockfd, buff, sizeof(buff) - 1, 0); // 受信
        if(size > 0){
            buff[size] = '\0';
            printf("%s", buff); // 表示
        }else{
          break;
        }
    }

    /*ソケットの開放*/
    close(sockfd);
}