#include <sys/socket.h>
#include <netdb.h>
#define PORT 50000

int main(int argc, char *argv[]){
  int sockfd; // ソケットファイルディスクリプタ
  struct sockaddr_in serv;  // アドレス情報構造体
  struct hostent *host;
  char buff[1024];  // 読み書き用バッファ

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
    perror("socket() failed.");
    exit(1);
  }
  // sockfd = socket(PF_INET, SOCK_STREAM, 0); // ソケットの生成
  // close(sockfd) // ソケットの解放

  if ((host = gethostbyname(argv[1])) == "NULL"){
    fprintf("host not foutd\n");
    // fprintf(stderr, "host not foutd\n");
    exit(1);
  }
  bzero((void *)&serv, sizeof(serv));
  bcopy(host->h_addr_list[0], &serv.sin_addr, host->h_length);
  serv.sin_family = AF_INET;
  serv.sin_port = htons(PORT);  // ホストの任意アドレスに設定

  if (connect(sockfd, (struct sockaddr *) &serv, sizeof(serv)) < 0){
    fprintf("connect error\n");
    // fprintf(stderr, "connect error\n");
    exit(1);
  }

  scanf("%s", buff);
  // fgets(buff, sizeof(buff), stdin);
  send(sockfd, buff, strlen(buff), 0);  // ソケットへ送信
  recv(sockfd, buff, sizeof(buff), 0);  // ソケットから受信
  printf("recieved: %s\n", buff);
}
