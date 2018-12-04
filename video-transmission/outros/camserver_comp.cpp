//server9.cpp
#include "project_hae.hpp"
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
    server.sendImgComp(img);
  }
}