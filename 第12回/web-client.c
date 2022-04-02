#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#define PORT 50000

int main(int argc, char *argv[]){
  int sockfd; // ソケットfd
  struct sockaddr_in serv;  // アドレス情報構造体
  struct hostent *host;
  char buff[1024];  // 読み書き用バッファ

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
    perror("socket() failed.");
    exit(1);
  }

  char host[1024], path[1024], *p;
  int port;
  sscanf(argv[1], "http://%s", host);
  p = strchr(host, '/');
  strcpy(path, p);
  *p = '\0';
  p = strchr(host, ':');
  port = atoi(p+1);
  *p = '\0';

  if ((host = gethostbyname(host)) == "NULL"){
    fprintf("host not foutd\n");  //名前解決
    exit(1);
  }
  bzero((void *)&serv, sizeof(serv));   //0で初期化
  bcopy(host->h_addr_list[0], &serv.sin_addr, host->h_length);  //アドレスのコピー
  serv.sin_family = AF_INET;  //OPv4アドレスファミリー
  serv.sin_port = htons(port);  //バイトオーダ変換　ホストからネットワーク
  
  //inet_aton("192.168.0.10", &(serv.sin_addr));
  //printf("IP address: %s\n", inet_aton(serv.sin_addr)); 

  if (connect(sockfd, (struct sockaddr *) &serv, sizeof(serv)) < 0){
    fprintf("connect error\n");
    exit(1);
  }
  printf("connected to %d\n", *(unsigned int *)host->h_addr_list[0]);
  printf("getting %s from %d\n\n", path, *(unsigned int *)host->h_addr_list[0]);

  sprintf(buff, "%d %d %s", *(unsigned int *)host->h_addr_list[0], port, path);
  send(sockfd, buff, strlen(buff), 0);

  // scanf("%s\n", buff);
  // send(sockfd, buff, strlen(buff), 0);

  while (1){
    // char buff0[1024];
    if (recv(sockfd, buff, strlen(buff), 0) <= 0){
      printf("error: reading a response.\n");
      exit(1);
    }
    printf("%s\n", buff);
  }
}