//server9.cpp
#include "project.hpp"
#include <cekeikon.h>
#include <cekraspicam.h>

int main(void) {
  SERVER server;
  server.waitConnection();
  vector<BYTE> vb;
  CEKRASPICAM cam;
  Mat_<COR> img;
  
  while(1) {
    // capturar img
    cam >> img;
    // enviar img
    server.sendImg(img);

    // server.receiveVb(vb);
    // // checar se parte abaixo é essencial
    // if (testaVb(vb,222)) {
    //   printf("Recebi corretamente %lu bytes %u\n",vb.size(),222);
    // }
    // else {
    //   printf("Erro na recepcao de %lu bytes %u\n",vb.size(),222);
    // }
    // vb.assign(100000,2);
    // server.sendVb(vb);  
  }
}