# Makefile for Linux TCP Chat Project

CC = gcc
CFLAGS = -Wall -g

# 기본 타겟: 모든 실행 파일 생성
all: server client

# 서버 컴파일
server: server.c
	$(CC) $(CFLAGS) -o server server.c

# 클라이언트 컴파일
client: client.c
	$(CC) $(CFLAGS) -o client client.c

# 정리: 생성된 실행 파일 삭제
clean:
	rm -f server client

# 테스트를 위한 샘플 파일 생성
test-file:
	echo "안녕하세요! 이것은 테스트 파일입니다." > test.txt
	echo "파일 다운로드 기능을 테스트하고 있습니다." >> test.txt
	echo "Linux TCP Chat 프로그램입니다." >> test.txt

# 도움말
help:
	@echo "사용 가능한 명령어:"
	@echo "  make all      - 서버와 클라이언트 모두 컴파일"
	@echo "  make server   - 서버만 컴파일"
	@echo "  make client   - 클라이언트만 컴파일"
	@echo "  make clean    - 컴파일된 파일들 삭제"
	@echo "  make test-file - 테스트용 파일 생성"
	@echo "  make help     - 이 도움말 표시"

.PHONY: all clean test-file help