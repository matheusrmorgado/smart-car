#include "project.hpp"
#include <cekeikon.h>

int main(int argc, char *argv[]) {
  if (argc!=2) erro("client6 servidorIpAddr\n");
  CLIENT client(argv[1]);
  vector<BYTE> vb;
  Mat_<COR> img;
  int sinc = 1;

  int ch=-1;
  while (ch<0) {
    // recebe img
    client.receiveImgComp(img);
    client.sendInt(sinc);
    sinc = -sinc;
    imshow("janela",img);
    ch=waitKey(30);
  }
}
