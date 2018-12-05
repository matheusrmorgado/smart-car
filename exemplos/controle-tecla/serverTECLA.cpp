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
#include <cekraspicam.h>
#include <softPwm.h>
#include <wiringPi.h>


class SERVER {
  const string PORT="3490";  // the port users will be connecting to
  const int BACKLOG=1;   // how many pending connections queue will hold

  int listensock, sockfd;  // listen on sock_fd, new connection on sockfd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  // get sockaddr, IPv4 or IPv6:
  static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
public:
  SERVER() {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(NULL, PORT.c_str(), &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); exit(1);
    }
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((listensock = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) {
        perror("server: socket"); continue;
      }
      if (setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &yes,
          sizeof(int)) == -1) {
        perror("setsockopt"); exit(1);
      }
      if (bind(listensock, p->ai_addr, p->ai_addrlen) == -1) {
        close(listensock); perror("server: bind"); continue;
      }
      break;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (p == NULL)  {
      fprintf(stderr, "server: failed to bind\n"); exit(1);
    }
    if (listen(listensock, BACKLOG) == -1) {
      perror("listen"); exit(1);
    }
  }

  ~SERVER() {
    close(listensock);
    close(sockfd);
    this->giraDireita(0);
    this->giraEsquerda(0);
  }

  void waitConnection() {
    while (true) {
      sin_size = sizeof their_addr;
      sockfd = accept(listensock, (struct sockaddr *)&their_addr, &sin_size);
      if (sockfd == -1) {
        perror("accept"); continue;
      } else break;
    }
    inet_ntop(their_addr.ss_family,
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s);
    printf("server: got connection from %s\n", s);
  }

  void sendBytes(int nBytesToSend, BYTE *buf) {
    while (nBytesToSend>0) {
      int nSentBytes = send(sockfd, buf, nBytesToSend, 0);
      if (nSentBytes==-1) {
        perror("camserver: send"); exit(1);
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
  
  void sendImg(const Mat_<COR>& img){
	vector<BYTE> vb;
    vector<int> param{CV_IMWRITE_JPEG_QUALITY,80};
    imencode(".jpg",img,vb,param);
    sendVb(vb);
	}
	void giraEsquerda(int direction){
		if (direction > 0){
		softPwmWrite(0, 0);
		softPwmWrite(1, direction);
		}
		else if( direction < 0){
		softPwmWrite(0, direction);
		softPwmWrite(1, 0);
		}
		else if ( direction == 0){
		softPwmWrite(0, 0);
		softPwmWrite(1, 0);
		}	
	}
	void giraDireita(int direction){
		if (direction > 0){
		softPwmWrite(2, direction);
		softPwmWrite(3, 0);
		}
		else if( direction < 0){
		softPwmWrite(2, 0);
		softPwmWrite(3, direction);
		}
		else if ( direction == 0){
		softPwmWrite(2, 0);
		softPwmWrite(3, 0);
		}	
	}
	
void printSQR( Mat_<COR>& img, Mat_<FLT> quadrado){				
	
	Mat_<FLT> imgFLT(img.size());
	Mat_<FLT> result;
	
	#pragma omp parallel for
	for (int ii = 0; ii  < img.rows; ii++)
		for (int jj = 0; jj  < img.cols; jj++)
			imgFLT(ii,jj) = (float)(img(ii,jj)[0])/256;
		
		
	double minVal; double maxVal;
	Point minLoc; Point maxLoc; Point matchLoc;

	int tamanho; float aux = 0.0;
	
	#pragma omp parallel for
	for ( int s = 15; s < 61; s = s + 6){
		resize( quadrado, quadrado, Size(s,s), 0, 0, INTER_AREA );
		quadrado = somaAbsDois( dcReject( quadrado ) );
		matchTemplate( imgFLT, quadrado, result, CV_TM_CCORR );
		minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc );	
		if ( maxVal > aux){
			aux = maxVal;
			tamanho = s;
			matchLoc = maxLoc;
		}
	}

	
	if( aux > 0.23f){
		retang(img, matchLoc.y,matchLoc.x-1, matchLoc.y-1 + tamanho-1,matchLoc.x + tamanho-1, COR(255,0,0));
		if (tamanho < 56){
			int proportion = ((img.cols/2) - (matchLoc.x + tamanho/2))/img.cols;
			giraDireita(50-proportion);
			cout << "DIREITA " << 50-proportion << endl;
			giraEsquerda(50+proportion);
			cout << "ESQUERDA " << 50+proportion << endl;
		}
		else{
			giraDireita(0);
			giraEsquerda(0);
		}
	}
	else{
		giraDireita(0);
		giraEsquerda(0);
	}
}
	
};

int main(void) {
	wiringPiSetup();
  if (softPwmCreate(0, 0, 100)) erro("erro");
  if (softPwmCreate(1, 0, 100)) erro("erro");
  if (softPwmCreate(2, 0, 100)) erro("erro");
  if (softPwmCreate(3, 0, 100)) erro("erro");
  
  SERVER server;
  server.waitConnection();
   raspicam::RaspiCam_Cv Camera;
    Mat_<COR> image;
    Camera.set( CV_CAP_PROP_FORMAT, CV_32F );
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH,  320);
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
	      Mat_<FLT> quadrado; 
    le( quadrado, "quadradoTop.png" );
	int key = -1;

    while(1){
    Camera.grab();
    Camera.retrieve (image);
    //server.printSQR(image, quadrado);
    
    server.receiveInt(key);
    cout << key << endl;
		switch ( key){
			
			case -1:
			server.giraDireita(0);
			server.giraEsquerda(0);
			break;
			case 1:
			server.giraDireita(-60);
			server.giraEsquerda(0);
			break;
			case 2:
			server.giraDireita(-60);
			server.giraEsquerda(-60);
			break;
			case 3:
			server.giraDireita(0);
			server.giraEsquerda(-60);
			break;	
			case 5:
			server.giraDireita(0);
			server.giraEsquerda(0);
			break;
			case 7:
			server.giraDireita(0);
			server.giraEsquerda(60);	
			break;
			case 8:
			server.giraDireita(60);
			server.giraEsquerda(60);	
			break;
			case 9:
			server.giraDireita(60);
			server.giraEsquerda(0);
			break;
		}
		
    server.sendImg(image);
	}	
}
