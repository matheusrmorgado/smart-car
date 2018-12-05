# include "project.hpp"
# include <cekeikon.h>
# include <cekraspicam.h>
# include <stdio.h>
# include <wiringPi.h>
# include <softPwm.h>


void motor_direita_frente(int vel);
void motor_direita_freio();
void motor_direita_re(int vel);
void motor_esquerda_frente(int vel);
void motor_esquerda_freio();
void motor_esquerda_re(int vel);
void ponto_morto();

void motor_direita_frente(int vel) {
    softPwmWrite(0, 0);
    softPwmWrite(1, vel);
}

void motor_direita_freio() {
    softPwmWrite(0, 100);
    softPwmWrite(1, 100);
}

void motor_direita_re(int vel) {
    softPwmWrite(0, vel);
    softPwmWrite(1, 0);
}

void motor_esquerda_frente(int vel) {
    softPwmWrite(25, 0);
    softPwmWrite(28, vel);
}

void motor_esquerda_freio() {
    softPwmWrite(25, 100);
    softPwmWrite(28, 100);
}

void motor_esquerda_re(int vel) {
    softPwmWrite(25, vel);
    softPwmWrite(28, 0);
}

void ponto_morto() {
    softPwmWrite(0, 0); softPwmWrite(1, 0);
    softPwmWrite(25, 0); softPwmWrite(28, 0);
}


int main(void) {
    SERVER server;
    server.waitConnection();
    CEKRASPICAM cam(240, 320);
    wiringPiSetup();
    if (softPwmCreate(0, 0, 100)) erro("erro");
    if (softPwmCreate(1, 0, 100)) erro("erro");
    if (softPwmCreate(25, 0, 100)) erro("erro");
    if (softPwmCreate(28, 0, 100)) erro("erro");

    Mat_<COR> img_camera;
    bool deve_enviar = true;
    int estado = 0;
    int novo_estado = 0;
    int rapido = 90;
    int devagar = 40;

    while(1) {
        // captura imagem
        cam >> img_camera;
        cv::resize(img_camera, img_camera, Size(320, 240));
        // enviar img
        server.sendImgComp(img_camera);

        // receber novo estado
        server.receiveInt(novo_estado);
        estado = novo_estado;

        // Determinar ação com base no estado do botão
        if (estado == 0) {
            motor_esquerda_freio();
            motor_direita_freio();
        }
        else if (estado == 1) {
            motor_esquerda_frente(devagar);
            motor_direita_frente(rapido);
        }
        else if (estado == 2) {
            motor_esquerda_frente(rapido);
            motor_direita_frente(rapido);   
        }
        else if (estado == 3) {
            motor_esquerda_frente(rapido);
            motor_direita_frente(devagar);
        }
        else if (estado == 4) {
            motor_esquerda_re(devagar);
            motor_direita_frente(devagar);
        }
        // estado 5 == parado
        else if (estado == 6) {
            motor_esquerda_frente(devagar);
            motor_direita_re(devagar);   
        }
        else if (estado == 7) {
            motor_esquerda_re(devagar);
            motor_direita_re(rapido);
        }
        else if (estado == 8) {
            motor_esquerda_re(devagar);
            motor_direita_re(devagar);
        }
        else if (estado == 9) {
            motor_esquerda_re(rapido);
            motor_direita_re(devagar);
        }
    }
}