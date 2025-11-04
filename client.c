#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 100

// '\0' 기준으로 문자열 수신
int readLine(int fd, char *buf) {
    int i = 0;
    char c;
    while (read(fd, &c, 1) > 0) {
        if (c == '\0') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

int main(int argc, char *argv[]) {
    int cfd, port;
    struct sockaddr_in serveraddr;
    struct hostent *hp;
    char inmsg[MAXLINE], outmsg[MAXLINE];

    if (argc != 3) {
        fprintf(stderr, "사용법: %s <host> <port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);
    hp = gethostbyname(argv[1]); // 호스트 이름 -> IP 주소

    // 클라이언트 소켓 생성
    cfd = socket(AF_INET, SOCK_STREAM, 0);

    // 서버 주소 설정
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    memcpy(&serveraddr.sin_addr, hp->h_addr_list[0], hp->h_length);
    serveraddr.sin_port = htons(port);

    // 서버에 연결 요청
    connect(cfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    if (fork() == 0) {
        // 자식 프로세스: 서버로부터 메시지 수신
        while (readLine(cfd, inmsg)) {
            printf("[서버] %s\n", inmsg);
        }
        exit(0);
    } else {
        // 부모 프로세스: 사용자 입력을 서버로 전송
        while (1) {
            printf("입력 > ");
            fgets(outmsg, MAXLINE, stdin);
            outmsg[strcspn(outmsg, "\n")] = '\0'; // 개행 제거
            write(cfd, outmsg, strlen(outmsg) + 1); // 전송
        }
    }

    close(cfd);
    return 0;
}
