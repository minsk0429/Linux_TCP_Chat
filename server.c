#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 100

// '\0' 문자를 기준으로 문자열을 한 줄씩 읽는 함수
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
    int sfd, cfd, port;
    socklen_t clientlen;
    struct sockaddr_in serveraddr, clientaddr;
    char inmsg[MAXLINE], outmsg[MAXLINE];
    FILE *fp;

    if (argc != 2) {
        fprintf(stderr, "사용법: %s <port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);

    // 서버 소켓 생성
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    // 서버 주소 설정
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; // IPv4
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP에서 접속 허용
    serveraddr.sin_port = htons(port); // 포트 설정

    // 주소 바인딩
    bind(sfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    // 클라이언트 연결 대기
    listen(sfd, 5);

    // 클라이언트 연결 수락
    clientlen = sizeof(clientaddr);
    cfd = accept(sfd, (struct sockaddr *)&clientaddr, &clientlen);
    printf("클라이언트 연결됨: %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

    if (fork() == 0) {
        // 자식 프로세스: 클라이언트로부터 수신 처리
        while (readLine(cfd, inmsg)) {
            // "download" 명령어 처리
            if (strcmp(inmsg, "download") == 0) {
                write(cfd, "다운로드할 파일 이름 입력:", 25); // 안내 메시지 전송
                readLine(cfd, inmsg); // 파일 이름 수신

                fp = fopen(inmsg, "r"); // 파일 열기
                if (!fp) {
                    write(cfd, "해당 파일 없음", strlen("해당 파일 없음") + 1); // 파일 없을 경우
                } else {
                    // 파일 존재: 한 줄씩 전송
                    while (fgets(outmsg, MAXLINE, fp)) {
                        write(cfd, outmsg, strlen(outmsg) + 1); // 문자열 전송 (NULL 포함)
                    }
                    fclose(fp);
                }
            } else {
                // 일반 채팅 메시지 출력
                printf("[클라이언트] %s\n", inmsg);
            }
        }
        exit(0); // 자식 프로세스 종료
    } else {
        // 부모 프로세스: 사용자 입력을 클라이언트로 전송
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
