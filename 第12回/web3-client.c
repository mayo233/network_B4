/* $Id: http-client.c,v 1.6 2013/01/23 06:57:19 68user Exp $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUF_LEN 256                      /* バッファのサイズ */

int main(int argc, char *argv[]){
    int s;                               /* ソケットのためのファイルディスクリプタ */
    struct hostent *servhost;            /* ホスト叹と IP アドレスを胺うための菇陇挛 */
    struct sockaddr_in server;           /* ソケットを胺うための菇陇挛 */
    struct servent *service;             /* サ〖ビス (http など) を胺うための菇陇挛 */

    char send_buf[BUF_LEN];              /* サ〖バに流る HTTP プロトコル脱バッファ */
    char host[BUF_LEN] = "localhost";    /* 儡鲁するホスト叹 */
    char path[BUF_LEN] = "/";            /* 妥滇するパス */
    unsigned short port = 80;            /* 儡鲁するポ〖ト戎规 */

    if ( argc > 1 ){                     /* URLが回年されていたら */
        char host_path[BUF_LEN];

        if ( strlen(argv[1]) > BUF_LEN-1 ){
            fprintf(stderr, "URL が墓すぎます。\n");
            return 1;
        }
                                         /* http:// から幌まる矢机误で */
                                         /* sscanf が喇根して */
                                         /* http:// の稿に部か矢机误が赂哼するなら */
        if ( strstr(argv[1], "http://") &&
             sscanf(argv[1], "http://%s", host_path) &&
             strcmp(argv[1], "http://" ) ){
            char *p;

            p = strchr(host_path, '/');  /* ホストとパスの惰磊り "/" を拇べる */
            if ( p != NULL ){
                strcpy(path, p);        /* "/"笆惯の矢机误を path にコピ〖 */
                *p = '\0';
                strcpy(host, host_path); /* "/"より涟の矢机误を host にコピ〖 */
            } else {                     /* "/"がないなら♂http://host という苞眶なら */
                strcpy(host, host_path); /* 矢机误链挛を host にコピ〖 */
            }

            p = strchr(host, ':');       /* ホスト叹の婶尸に ":" が崔まれていたら */
            if ( p != NULL ){
                port = atoi(p+1);        /* ポ〖ト戎规を艰评 */
                if ( port <= 0 ){        /* 眶机でない (atoi が己窃) か、0 だったら */
                    port = 80;           /* ポ〖ト戎规は 80 に疯め虑ち */
                }
                *p = '\0';
            }
        } else {
            fprintf(stderr, "URL は http://host/path の妨及で回年してください。\n");
            return 1;
        }
    }

    printf("http://%s%s を艰评します。\n\n", host, path);

                                /* ホストの攫鼠(IPアドレスなど)を艰评 */
    servhost = gethostbyname(host);
    if ( servhost == NULL ){
        fprintf(stderr, "[%s] から IP アドレスへの恃垂に己窃しました。\n", host);
        return 0;
    }

    bzero(&server, sizeof(server));            /* 菇陇挛をゼロクリア */

    server.sin_family = AF_INET;

                                               /* IPアドレスを绩す菇陇挛をコピ〖 */
    bcopy(servhost->h_addr, &server.sin_addr, servhost->h_length);

    if ( port != 0 ){                          /* 苞眶でポ〖ト戎规が回年されていたら */
        server.sin_port = htons(port);
    } else {                                   /* そうでないなら getservbyname でポ〖ト戎规を艰评 */
        service = getservbyname("http", "tcp");
        if ( service != NULL ){                /* 喇根したらポ〖ト戎规をコピ〖 */
            server.sin_port = service->s_port;
        } else {                               /* 己窃したら 80 戎に疯め虑ち */
            server.sin_port = htons(80);
        }
    }
                                /* ソケット栏喇 */
    if ( ( s = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ){
        fprintf(stderr, "ソケットの栏喇に己窃しました。\n");
        return 1;
    }
                                /* サ〖バに儡鲁 */
    if ( connect(s, (struct sockaddr *)&server, sizeof(server)) == -1 ){
        fprintf(stderr, "connect に己窃しました。\n");
        return 1;
    }

                                /* HTTP プロトコル栏喇 & サ〖バに流慨 */
    sprintf(send_buf, "GET %s HTTP/1.0\r\n", path);
    write(s, send_buf, strlen(send_buf));

    sprintf(send_buf, "Host: %s:%d\r\n", host, port);
    write(s, send_buf, strlen(send_buf));

    sprintf(send_buf, "\r\n");
    write(s, send_buf, strlen(send_buf));

                                /* あとは减慨して、山绩するだけ */
    while (1){
        char buf[BUF_LEN];
        int read_size;
        read_size = read(s, buf, BUF_LEN);
        if ( read_size > 0 ){
            write(1, buf, read_size);
        } else {
            break;
        }
    }
                                /* 稿幌琐 */
    close(s);

    return 0;
}