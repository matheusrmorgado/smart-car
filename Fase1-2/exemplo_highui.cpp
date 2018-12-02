//highgui.cpp
#include <cekeikon.h>
#include <opencv2/opencv.hpp>

int estado=0; //0=nao_apertado, 1=apertou_botao_1 2=apertou_botao_2
void on_mouse(int event, int c, int l, int flags, void* userdata) {
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

int main() {
    COR cinza(128,128,128);
    COR vermelho(0,0,255);
    Mat_<COR> imagem(900,1800,cinza);
    namedWindow("janela",WINDOW_NORMAL);
    resizeWindow("janela",2*imagem.cols,2*imagem.rows);
    setMouseCallback("janela", on_mouse);
    imshow("janela",imagem);

    while (waitKey(1)!=27) { // ESC=27 sai do programa
        imagem.setTo(cinza);
        cv::arrowedLine(imagem, Point(290, 290), Point(10 , 10), Scalar(255, 255, 255), 2, 8, 0, 0.15); // seta 1
        cv::arrowedLine(imagem, Point(450, 290), Point(450, 10), Scalar(255, 255, 255), 2, 8, 0, 0.15);  // seta 2
        cv::arrowedLine(imagem, Point(610, 290), Point(890, 10), Scalar(255, 255, 255), 2, 8, 0, 0.15);  // seta 3
        cv::arrowedLine(imagem, Point(290, 450), Point(10 , 450), Scalar(255, 255, 255), 2, 8, 0, 0.15);  // seta 4
        // seta 5 nao existe
        cv::arrowedLine(imagem, Point(610, 450), Point(890, 450), Scalar(255, 255, 255), 2, 8, 0, 0.15);  // seta 6
        cv::arrowedLine(imagem, Point(290, 610), Point(10, 890), Scalar(255, 255, 255), 2, 8, 0, 0.15);  // seta 7
        cv::arrowedLine(imagem, Point(450, 610), Point(450 , 890), Scalar(255, 255, 255), 2, 8, 0, 0.15);  // seta 8
        cv::arrowedLine(imagem, Point(610, 610), Point(890, 890), Scalar(255, 255, 255), 2, 8, 0, 0.15);  // seta 9


        if (estado==1) {
            cv::arrowedLine(imagem, Point(290, 290), Point(10 , 10), Scalar(255, 0, 255), 2, 8, 0, 0.15); // seta 1

        }
    
        else if (estado==2) {
            cv::arrowedLine(imagem, Point(450, 290), Point(450, 10), Scalar(255, 0, 255), 2, 8, 0, 0.15);  // seta 2
        }

        else if (estado==3) {
            cv::arrowedLine(imagem, Point(610, 290), Point(890, 10), Scalar(255, 0, 255), 2, 8, 0, 0.15);  // seta 3
        }
    
        else if (estado==4) {
            
        }

        else if (estado==5) {
            
        }
    
        else if (estado==6) {
            
        }

        else if (estado==7) {
            
        }
    
        else if (estado==8) {
            for (int l=600; l<900; l++)
                for (int c=300; c<600; c++)
                    imagem(l,c)=vermelho;
        }

        else if (estado==9) {
            for (int l=600; l<900; l++)
                for (int c=600; c<900; c++)
                    imagem(l,c)=vermelho;
        }

        imshow("janela",imagem);
    }
}