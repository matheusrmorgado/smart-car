//client9.cpp
//testa sendVb e receiveVb
#include "project.hpp"
#include <cekeikon.h>
// #include <cekraspicam.h>

int main(int argc, char *argv[]) {
  if (argc!=2) erro("client6 servidorIpAddr\n");
  CLIENT client(argv[1]);
  vector<BYTE> vb;
  Mat_<COR> img;

  int ch=-1;
  while (ch<0) {
    // recebe img
    client.receiveImg(img);
    imshow("janela",img);
    ch=waitKey(30);
    if (testaVb(vb,111)) {
      printf("Recebi corretamente %lu bytes %u\n",vb.size(),111);
    }
    else {
      printf("Erro na recepcao de %lu bytes %u\n",vb.size(),111);
    }
    vb.assign(100000,222);
    client.sendVb(vb);
    client.receiveVb(vb);
    if (testaVb(vb,1)) {
      printf("Recebi corretamente %lu bytes %u\n",vb.size(),1);
    }
    else {
      printf("Erro na recepcao de %lu bytes %u\n",vb.size(),1);
    }
  }
}