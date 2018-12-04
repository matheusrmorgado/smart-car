#include "project.hpp"
#include <cekeikon.h>
#include <cekraspicam.h>

int main(void) {
  SERVER server;
  server.waitConnection();
  int ack;
  CEKRASPICAM cam;
  Mat_<COR> img;
  bool deve_enviar = true;
  int sinc = 1;
  
  while(1) {
    if (deve_enviar) {
      deve_enviar = false;
      // capturar img
      cam >> img;
      // enviar img
      server.sendImgComp(img);
    }
    server.receiveInt(ack);
    if (ack == sinc) {
      deve_enviar = true;
      sinc = -sinc;
    }
  }
}
