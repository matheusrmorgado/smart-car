#include "project.hpp"
#include <cekeikon.h>
#include <stdio.h>

using namespace cv;

int estado;
void onMouse(int event, int c, int l, int flags, void* userdata) {
    if (event==EVENT_LBUTTONDOWN) {
        if      (   0<=l && l<300 &&   0<=c && c<300 ) estado=1;
        else if (   0<=l && l<300 && 300<=c && c<600 ) estado=2;
        else if (   0<=l && l<300 && 600<=c && c<900 ) estado=3;
        else if ( 300<=l && l<600 &&   0<=c && c<300 ) estado=4;
        else if ( 300<=l && l<600 && 300<=c && c<600 ) estado=5;
        else if ( 300<=l && l<600 && 600<=c && c<800 ) estado=6;
        else if ( 600<=l && l<900 &&   0<=c && c<300 ) estado=7;
        else if ( 600<=l && l<900 && 300<=c && c<600 ) estado=8;
        else if ( 600<=l && l<900 && 600<=c && c<900 ) estado=9;

        else estado=0;
    }
    else if (event==EVENT_LBUTTONUP) {
        estado=0;
    }
}

class tecladoVirtual {
    Point p,q;
    int thickness, lineType, shift;
    double tipLength;
  public:
    tecladoVirtual(Point _p, Point _q) 
    { p=_p; q=_q; thickness= 3; lineType= 8; shift= 0; tipLength= 0.125;}
    void controle(Mat_<Vec3b>& imagem, bool acesa);
};

tecladoVirtual seta1(Point(290, 290), Point(20 , 20));
tecladoVirtual seta2(Point(450, 290), Point(450, 15));
tecladoVirtual seta3(Point(610, 290), Point(880, 20));
tecladoVirtual seta4(Point(290, 450), Point(15 , 450));
    // seta 5 nao existe
tecladoVirtual seta6(Point(610, 450), Point(885, 450));
tecladoVirtual seta7(Point(290, 610), Point(20, 880));
tecladoVirtual seta8(Point(450, 610), Point(450 , 885));
tecladoVirtual seta9(Point(610, 610), Point(880, 880));

void tecladoVirtual::controle(Mat_<Vec3b>& imagem, bool acesa)
{ if (acesa) arrowedLine(imagem, p, q, Scalar(0,0,255), thickness, lineType, shift, tipLength);
  else       arrowedLine(imagem, p, q, Scalar(255,255,255), thickness, lineType, shift, tipLength);
}

void setControles(Mat_<Vec3b>& imagem)
{
    if (estado == 1)
        seta1.controle(imagem, true);
    else if (estado == 2)
        seta2.controle(imagem, true);
    else if (estado == 3)
        seta3.controle(imagem, true);
    else if (estado == 4)
        seta4.controle(imagem, true);
    else if (estado == 6)
        seta6.controle(imagem, true);
    else if (estado == 7)
        seta7.controle(imagem, true);
    else if (estado == 8)
        seta8.controle(imagem, true);
    else if (estado == 9)
        seta9.controle(imagem, true);
    else {
        seta1.controle(imagem, false);
        seta2.controle(imagem, false);
        seta3.controle(imagem, false);
        seta4.controle(imagem, false);
        seta6.controle(imagem, false);
        seta7.controle(imagem, false);
        seta8.controle(imagem, false);
        seta9.controle(imagem, false);
    }
}

int main(int argc, char *argv[]) {
    if (argc!=2) erro("client6 servidorIpAddr\n");
    CLIENT client(argv[1]);
    vector<BYTE> vb;
    COR cinza(128, 128, 128);
    Mat_<COR> tela(900, 1800);
    Mat_<COR> camera_display(900, 900);
    Mat_<COR> teclado_display(900, 900, cinza);
    setControles(tela);
    namedWindow("tela", WINDOW_NORMAL);
    resizeWindow("tela",2*tela.cols,2*tela.rows);
    // namedWindow("botoes", WINDOW_NORMAL);
    // resizeWindow("botoes", 2*teclado_display.cols, 2*teclado_display.rows);
    // namedWindow("camera_display", WINDOW_NORMAL);
    // resizeWindow("camera_display", 2*camera_display.cols, 2*camera_display.rows);
    setMouseCallback("tela", onMouse);

    int ch=-1;
    while (ch<0) {
        // recebe img
        client.receiveImgComp(camera_display);
        flip(camera_display, camera_display, -1);
        resize(camera_display, camera_display, Size(900, 900));

        // cout << "camera_display.cols: " << camera_display.cols << endl;
        // cout << "camera_display.rows: " << camera_display.rows << endl;
        // cout << "teclado_display.cols: " << teclado_display.cols << endl;
        // cout << "teclado_display.rows: " << teclado_display.rows << endl;
        
        // concatena imagens
        cv::Mat matArray[] = {teclado_display, camera_display};
        cv::hconcat(matArray, 2, tela);
        setControles(tela);

        // envia estado
        client.sendInt(estado);        

        // mostra na janela do PC
        // imshow("camera_display", camera_display);
        // imshow("botoes", teclado_display);
        imshow("tela", tela);
        ch=waitKey(30);
    }
}
