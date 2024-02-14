#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif

#if !defined(_WIN32)
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <tchar.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if !defined(const)
#define const
#endif

#if !defined(WIN32)
void sig_handler(int signum) {
  switch (signum) {
  case SIGTERM:
  case SIGALRM:
    printf("unknown.host\n\r");
    perror("Resolve: Could not get host name");
    exit(0);
    break;
  }
}

int main(int argc, char *argv[]) {
  int ip;
  static char addr_str[256];
  char *address;
  struct hostent *from;
  int addr;
  char buf[256];

  signal(SIGTERM, sig_handler);
  signal(SIGALRM, sig_handler);

  alarm(60);

  if (argc != 2) {
    printf("unknown.host\n\r");
    exit(0);
  }

  ip = atoi(argv[1]);

  if ((from = gethostbyaddr((char *)&ip, sizeof(ip), AF_INET)) != NULL) {
    strcpy(addr_str, strcmp(from->h_name, "unknown.host") ? from->h_name
    : "unknown.host");
  }
  else {
    addr = ntohl(ip);
    sprintf(addr_str, "%d.%d.%d.%d", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr)&0xFF);
  }

  alarm(0);
  printf("%s\n\r", addr_str);
  exit(0);
}
#else
int main(int argc, char *argv[]) {
  static char addr_str[NI_MAXHOST];
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSADATA wsaData;

  if (WSAStartup(wVersionRequested, &wsaData)) {
    // WSAStartup failed
    printf("unknown.host\n\r");
    return 0;
  }

  if (argc != 2) {
    printf("unknown.host\n\r");
    exit(0);
  }

  SOCKADDR_IN ssRemoteAddr;
  memset(&ssRemoteAddr, 0, sizeof(SOCKADDR_IN));
  ssRemoteAddr.sin_family = AF_INET6;
  ssRemoteAddr.sin_addr.S_un.S_addr = inet_addr(argv[1]);

  int nErr = getnameinfo((SOCKADDR *)&ssRemoteAddr, sizeof(ssRemoteAddr), addr_str, NI_MAXHOST, NULL, 0, 0);
  // szServ, NI_MAXSERV, NI_NUMERICHOST);

  if (nErr)
  strcpy(addr_str, "unknown.host");

  printf("%s\n\r", addr_str);

  WSACleanup();
  exit(0);
}
#endif
