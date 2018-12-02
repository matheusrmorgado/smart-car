//DesenhaSeta

#include <opencv2/opencv.hpp>
using namespace cv;

struct Mouse {
  bool clicou=true;
  int x,y;
};

void onMouse(int event, int x, int y, int flags, void* userdata)
{ Mouse* mouse=(Mouse*)userdata;
  if (event==EVENT_LBUTTONDOWN) {
    (*mouse).clicou=true; (*mouse).x=x; (*mouse).y=y;
  } 
}

class Botao {
  public:
    Point p,q;
    string st0,st1;
    bool primeiraVez;
    bool ligado;
    Botao(Point _p, Point _q, string _st0, string _st1="", bool _ligado=false);
    void testaDesenha(Mat_<Vec3b>& a, Mouse mouse);
};

Botao::Botao(Point _p, Point _q, string _st0, string _st1, bool _ligado)
{ p=_p; q=_q; st0=_st0; 
  if (_st1!="") st1=_st1; else st1=_st0;
  ligado=!_ligado; primeiraVez=true; 
}

void Botao::testaDesenha(Mat_<Vec3b>& a, Mouse mouse)
{ if (primeiraVez || (p.x<=mouse.x && mouse.x<=q.x && p.y<=mouse.y && mouse.y<=q.y)) {
    if (ligado) {
      ligado=false;
      rectangle(a, p,q, Scalar(160,160,160), CV_FILLED);
      line(a, p, Point(p.x,q.y), Scalar(255,255,255), 4);
      line(a, p, Point(q.x,p.y), Scalar(255,255,255), 4);
      line(a, q, Point(p.x,q.y), Scalar(0,0,0), 4);
      line(a, q, Point(q.x,p.y), Scalar(0,0,0), 4);
      putText(a, st0, Point(p.x+5,(p.y+q.y)/2), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), 2);
    } else {
      ligado=true;
      rectangle(a, p,q, Scalar(160,160,160), CV_FILLED);
      line(a, p, Point(p.x,q.y), Scalar(0,0,0), 4);
      line(a, p, Point(q.x,p.y), Scalar(0,0,0), 4);
      line(a, q, Point(p.x,q.y), Scalar(255,255,255), 4);
      line(a, q, Point(q.x,p.y), Scalar(255,255,255), 4);
      putText(a, st1, Point(p.x+5,(p.y+q.y)/2), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), 2);
    }
    primeiraVez=false;
  }
}

class Lampada {
    Point p,q;
  public:
    bool acesa;
    Lampada(Point _p, Point _q) { p=_p; q=_q;}
    void desenha(Mat_<Vec3b>& a);
};

void Lampada::desenha(Mat_<Vec3b>& a)
{ if (acesa) arrowedLine(a, p, q, Scalar(0,0,255), 2, 8, 0, 0.15);
  else       arrowedLine(a, p, q, Scalar(0,0,0), 2, 8, 0, 0.15);
}

int main()
{ printf("Sai do programa apertando uma tecla.\n");
  printf("Muda estado da lampada com click no botao.\n");

  Mat_<Vec3b> a(400,400,Vec3b(192,192,192));
  namedWindow("janela");
  Mouse mouse;
  setMouseCallback("janela", onMouse, &mouse);
  imshow("janela",a);
  Botao botao1(Point( 10, 10),Point(189,189),"BOTAO-0","BOTAO-1");
  Lampada lampada1(Point(300,30),Point(300,160));
  Botao botao2(Point( 10,210),Point(189,389),"BOTAO-0","BOTAO-1");
  Lampada lampada2(Point(300,360),Point(300,230));
  while (waitKey(30)<0) {
    if (mouse.clicou) {
      botao1.testaDesenha(a,mouse);
      lampada1.acesa=botao1.ligado;
      lampada1.desenha(a);
      botao2.testaDesenha(a,mouse);
      lampada2.acesa=botao2.ligado;
      lampada2.desenha(a);
      imshow("janela",a);
      mouse.clicou=false;
    }
  }
}