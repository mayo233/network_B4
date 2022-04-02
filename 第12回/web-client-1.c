#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#define PORT 50000

int main(int argc, char *argv[]){
    int sockfd;   //コネクション確立済みソケット 
    char buff[1024];  //読み書き用バッファ
    struct sockaddr_in serv;
    struct hostent *host;

    if ((host=gethostbyname(argv[1]))==NULL){
        fprintf(stderr,"host not found\n");
        exit(1);
    }
    bzero ((void *)&serv,sizeof(serv));
    bcopy(host->h_addr_list[0],&serv.sin_addr,host->h_length);
    serv.sin_family=AF_INET;
    serv.sin_port=htons(PORT);

    if(connect(sockfd,(struct sockaddr *)&serv, sizeof(serv))<0{
        fprintf(stderr,"connect error\n");
        exit(1);
    }

    
    sockfd=socket(PF_INET,SOCK_STREAM,0);  //ソケットの生成
    recv(sockfd,buff,sizeof(buff),0);  //ソケットから受信
    printf("%s\n", buff);

    fgets(buff,sizeof(buff),stdin);
    send(sockfd,buff,strlen(buff),0);          //ソケットへ送信

    close(sockfd);  //ソケットの解放
}