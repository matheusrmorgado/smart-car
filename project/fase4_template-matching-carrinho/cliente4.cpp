#include "project.hpp"
#include <cekeikon.h>
#include <stdio.h>

using namespace cv;

int estado;
void onMouse(int event, int c, int l, int flags, void* userdata)
{
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

class tecladoVirtual
{
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


void printSQR( Mat_<COR>& img, Mat_<FLT> quadrado)
{             
    
    Mat_<FLT> imgFLT(img.size());
    Mat_<FLT> result;
    
    #pragma omp parallel for
    for (int ii = 0; ii  < img.rows; ii++)
        for (int jj = 0; jj  < img.cols; jj++)
            imgFLT(ii,jj) = (float)(img(ii,jj)[0])/256;

    double minVal; double maxVal;
    Point minLoc; Point maxLoc; Point matchLoc;

    int tamanho; float aux = 0.0;
    
    for (int s = 15; s < 60; s=s+5){
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

    resize( quadrado, quadrado, Size(tamanho,tamanho), 0, 0, INTER_AREA );
    quadrado = somaAbsDois( dcReject( quadrado ) );
    matchTemplate( imgFLT, quadrado, result, CV_TM_CCORR );
    minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc );

    if(maxVal > 0.23f){
        line( img, Point( matchLoc.x + quadrado.cols/2 , matchLoc.y + quadrado.rows/2 + 0.12*quadrado.rows ), 
            Point( matchLoc.x + quadrado.cols/2 , matchLoc.y + quadrado.rows/2 - 0.12*quadrado.rows ), Scalar(0,0,255), 1, 8);
        line( img, Point( matchLoc.x + quadrado.cols/2 + 0.12*quadrado.rows , matchLoc.y + quadrado.rows/2 ), 
            Point( matchLoc.x + quadrado.cols/2 - 0.12*quadrado.rows , matchLoc.y + quadrado.rows/2 ), Scalar(0,0,255), 1, 8);
        rectangle( img, matchLoc, Point( matchLoc.x + quadrado.cols , matchLoc.y + quadrado.rows ), Scalar(0,0,255), 1.5, 8);

        string str = to_string(maxVal);
        putTxt( img, matchLoc.y + quadrado.rows/2 + 35, matchLoc.x + quadrado.cols/2 + 2, 
            str, COR(255,0,0) , 1, true, COR(0,0,0));

        if (tamanho > 20 && tamanho < 50) { 
            int proportion = round(((matchLoc.x)*100)/(img.cols));
            if (proportion < 20) {
                estado = 4;
            }
            else if (proportion < 40) {
                estado = 1;
            }
            else if (proportion < 60) {
                estado = 2;
            }
            else if (proportion < 80) {
                estado = 3;   
            }
            else {
                estado = 6;
            }
        }
        else {
            estado = 0;
        }
    }
    else {
        estado = 0;
    }
}

int main(int argc, char *argv[])
{
    // Read user input
    if (argc != 2) 
        erro("\n-- Erro -- \nNumero de argumentos invalido\nExemplo de entrada: client6 servidorIpAddr\n");

    CLIENT client(argv[1]);
    vector<BYTE> vb;
    Mat_<COR> camera_display(900, 900);
    namedWindow("camera_display", WINDOW_NORMAL);
    resizeWindow("camera_display",2*camera_display.cols,2*camera_display.rows);
    setMouseCallback("camera_display", onMouse);

    // Template a ser encontrado
    Mat_<FLT> quadrado; 
    le(quadrado, "template-new.png");

    int ch=-1;
    while (ch<0)
    {
        // recebe img
        client.receiveImgComp(camera_display);
        flip(camera_display, camera_display, -1);
        printSQR(camera_display, quadrado);
        resize(camera_display, camera_display, Size(900, 900));
        
        // envia estado
        client.sendInt(estado);

        imshow("camera_display", camera_display);
        ch=waitKey(30);
    }
}