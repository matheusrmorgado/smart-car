#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <cekeikon.h>
#include <iostream>


class CLIENT {
  const string PORT="3490"; // the port client will be connecting to 
  
  int sockfd, numbytes;  
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  // get sockaddr, IPv4 or IPv6:
  static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
public:
  CLIENT(string endereco) {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(endereco.c_str(), PORT.c_str(), &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); exit(1);
    }
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) {
        perror("client: socket"); continue;
      }
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        perror("client: connect"); close(sockfd); continue;
      }
      break;
    }
    if (p == NULL) {
      fprintf(stderr, "client: failed to connect\n"); exit(1);
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
        s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure
  }
  
  ~CLIENT() {
    close(sockfd);
  }
  
  void sendBytes(int nBytesToSend, BYTE *buf) {
    while (nBytesToSend>0) {
      int nSentBytes = send(sockfd, buf, nBytesToSend, 0);
      if (nSentBytes==-1) {   
        perror("camclient: send"); exit(1);
      }
      nBytesToSend -= nSentBytes;
      buf += nSentBytes;
    }
  }

  void receiveBytes(int nBytesToReceive, BYTE *buf) {
    while (nBytesToReceive>0) {
      int nReceivedBytes = recv(sockfd, buf, nBytesToReceive, 0);
      if (nReceivedBytes==-1) { perror("recv"); exit(1); }
      nBytesToReceive -= nReceivedBytes;
      buf += nReceivedBytes;
    }
  }

  void sendUint(uint32_t m) {
    static uint32_t n;
    n=htonl(m);
    sendBytes(4,(BYTE*)&n);
  }

  void sendInt(int32_t m) {
    static int32_t n;
    n=htonl(m);
    sendBytes(4,(BYTE*)&n);
  }

  void sendChar(char ch) {
    static char n;
    n=ch;
    sendBytes(1,(BYTE*)&ch);
  }

  void sendVb(const vector<BYTE>& vb) {
    sendUint(vb.size());
    sendBytes(vb.size(),(BYTE*)vb.data());
  }
  
  void sendString(const string& st) {
    sendUint(st.length());
    sendBytes(st.length(),(BYTE*)st.data());
  }

  void receiveChar(char& ch) {
    receiveBytes(1,(BYTE*)&ch);
  }

  void receiveUint(uint32_t& m) {
    receiveBytes(4,(BYTE*)&m);
    m=ntohl(m);
  }

  void receiveInt(int32_t& m) {
    receiveBytes(4,(BYTE*)&m);
    m=ntohl(m);
  }

  void receiveVb(vector<BYTE>& vb) {
    uint32_t t;
    receiveUint(t);
    vb.resize(t);
    receiveBytes(vb.size(),(BYTE*)vb.data());
  }
  
  void receiveString(string& st) {
    uint32_t t;
    receiveUint(t);
    st.resize(t);
    receiveBytes(st.size(),(BYTE*)st.data());
  }
  
  void receiveImg(Mat_<COR>& img){
	vector<BYTE> vb;
	int sizevector;
	receiveVb(vb);
	img=imdecode(vb,1);  
	flip(img, img, -1);
  }

int decodeKey(int key)
{
	if( key >= 1114033 && key <= 1114041 && key != 1114036 && key != 1114038)
		return(key - 1114032);
	else
		return(-1);
}


};

int main(int argc, char *argv[]) {
  if (argc!=2) {
	  printf("client6 servidorIpAddr\n");
	  return 0;
  }
  if (argc!=2) erro("client7 servidorIpAddr\n");
  CLIENT client(argv[1]);
  Mat_<COR> img; 
  namedWindow("img", WINDOW_AUTOSIZE);
  int key = -1;
  while(1)
  { 
	
	key = waitKey(20);
	cout << client.decodeKey(key) << endl;
	client.sendInt(client.decodeKey(key));
	client.receiveImg(img);
	imshow("img",img);
	}

}
