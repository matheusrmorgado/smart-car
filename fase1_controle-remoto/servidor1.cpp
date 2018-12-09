# include "project.hpp"
# include <cekeikon.h>
# include <cekraspicam.h>
# include <stdio.h>

void desenha_estado(Mat_<COR>& img_camera, int numero)
{

    string str = to_string(numero);
    putTxt( img_camera, 50, 50, str, COR(255,0,0), 1, true, COR(0,0,0));

}

int main(void)
{
    SERVER server;
    server.waitConnection();
    CEKRASPICAM cam(240, 320);

    Mat_<COR> img_camera;
    int estado = 0;
    int novo_estado = 0;

    while(1)
    {
        // recebe novo estado
        server.receiveInt(novo_estado);
        estado = novo_estado;

        // captura imagem
        cam >> img_camera;
        cv::resize(img_camera, img_camera, Size(320, 240));

        // Determinar ação com base no estado do botão
        desenha_estado(img_camera, estado);

        // enviar img
        server.sendImgComp(img_camera);
    }
}