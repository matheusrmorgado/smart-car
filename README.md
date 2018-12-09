# Carrinho Autônomo

## Objetivos

O projeto foi desenvolvido durante os meses de outubro e novembro na disciplina Laboratório de Sistemas Eletrônicos e teve como principal objetivo desenvolver um carrinho elétrico controlado por uma placa embarcada que roda sistema Linux, usando visão computacional e aprendizagem de máquina.

<p align="center">
  <img src=https://github.com/matheusrmorgado/smart-car/blob/master/project/carrinho.png height="477" width="614">
</p>
<p align="center">
  <i> Figura 1 - Vista frontal do carrinho </i>
</p>

## Descrição do desafio

Em linhas gerais, o projeto foi dividido em 6 etapas para facilitar o desenvolvimento e acompanhamento aula a aula pelos orientadores. No carrinho foi colocada uma placa Raspberry Pi (servidor) com uma câmera responsável por adquirir vídeo, comprimir e transmitir para um computador (cliente). Enquanto que o computador foi responsável por receber o vídeo, interpretar e comandar os motores do carrinho via comunicação TCP/IP.

## Estratégia para superar o desafio

### Transmissão de vídeo pela Raspberry e transmissão de comandos manuais de controle pelo computador

Na primeira etapa do projeto foi vital implementar a comunicação TCP/IP de forma eficiente. No início foram realizados alguns testes com o código desenvolvido pela dupla, mas as funções mostradas pelo professor em sala de aula tiveram um resultado mais satisfatório e por isso as funções mostradas em sala de aula foram implementadas.

Além disso, a dupla desenvolveu nessa fase a interface para controle de posição do carrinho pelo computador. Para isso, criamos uma classe chamada tecladoVirtual, sendo que para cada seta, criada pela função arrowedLine do OpenCV, existe um estado. Dessa forma, quando o usuário clica em uma seta, a variável estado é alterada e enviada para o Raspberry que interpreta e imprime na tela o número referente ao estado.

<p align="center">
  <img src=https://github.com/matheusrmorgado/smart-car/blob/master/examples/interface/interface.PNG height="300" width="300">
</p>
<p align="center">
  <i> Figura 2 - Interface para controle do carrinho </i>
</p>

### Controle remoto manual do carrinho com transmissão de vídeo

Para a segunda etapa do projeto foi aproveitado boa parte do código da primeira etapa, mas com a diferença de que o estado enviado pelo computador ao Raspberry agora ser responsável por controlar a direção e sentido do carrinho. Para possibilitar o controle dos motores foi utilizadas as bibliotecas wiringPi e softPwm.

### Localização da placa

Na terceira etapa do projeto, a dupla utilizou o método de template matching para identificação de placas com números. A função principal do programa fase3.cpp é a printSQR que funciona da seguinte forma: aumenta gradualmente o tamanho do quadrado utilizado como template (15, 20, 25, 30, 35, 40, 45, 50, 55) fazendo um resize; realiza um dcReject; para então fazer um matchTemplate entre a imagem original e o template, utilizando como método o CV_TM_CCORR para calcular correlação cruzada e detectar instâncias de alto contraste; faz um minMaxLoc com a imagem resultante da função matchTemplate; e por fim guarda o tamanho, o maxLoc e o tamanho do quadrado responsável pelo maior maxVal obtido com a função minMaxLoc.

Após essa iteração, utiliza-se o ponto maxLoc para desenhar um quadrado vermelho em volta da placa identificada e duas linhas para identificar o centro desse quadrado. Abaixo do quadrado é colocado o valor do maxVal obtido na iteração realizada anteriormente.

<p align="center">
  <img src=https://github.com/matheusrmorgado/smart-car/blob/master/project/fase3_template-matching/capturado1-quad450-fase3.png>
</p>
<p align="center">
	<i> Figura 3 - Identificação de placa </i>
</p>

Cabe também ressaltar que para implementar um “don’t care” para o conteúdo no centro da placa identificada, foi utilizado um template com ruído branco no interior. Dessa forma, a função templateMatching teve um resultado semelhante independente do número desenhado na placa. Além disso, foi utilizado processamento paralelo usando OpenMP, mas não foi observado uma melhora substancial no processamento do vídeo e aumento de FPS.

<p align="center">
  <img src=https://github.com/matheusrmorgado/smart-car/blob/master/project/fase3_template-matching/template-new.png height="250" width="250">
</p>
<p align="center">
	<i> Figura 4 - Imagem utilizada como template </i>
</p>

### Fazer o carrinho seguir automaticamente a placa

Para a quarta fase do projeto foi aproveitado totalmente o código da fase anterior para o desenvolvimento do programa cliente4.cpp, que funciona da seguinte forma: o computador recebe o vídeo da câmera do Raspberry Pi e divide a tela em 5 faixas verticais, permitindo que o carrinho vire para a esquerda ou para a direita dependendo da posição que a placa foi identificada no vídeo. 

Por exemplo, se a placa foi identificada no extremo direito do vídeo, o carrinho recebe o mesmo estado referente à seta para a direita da fase 2 do projeto, fazendo uma curva para a direita. Dessa forma, foi possível aproveitar parte do código que já tinha sido desenvolvido pela dupla em todas as fases anteriores.

### Reconhecimento de dígito manuscrito

Na quinta fase do projeto foram utilizadas técnicas de aprendizado de máquina para identificação do dígito escrito no interior da placa. Para isso, foi desenvolvido pela dupla um código que implementa o método do vizinho mais próximo aproximado e as classes CvKNearest e MNIST que foram responsáveis, respectivamente, pela identificação do número e pela leitura da base de dados de treino.

O código utiliza a função printSQR, desenvolvida na fase 3, para identificar a posição da placa, e aproveita a informação do tamanho do quadrado de melhor matching para definir a região em se encontra o desenho do número a ser identificado. Em seguida, a função find_nearest da classe CvKNearest é utilizada para identificar o valor do número, que em seguida é desenhado na imagem analisada.

<p align="center">
  <img src=https://github.com/matheusrmorgado/smart-car/blob/master/project/fase5_machine-learning/capturado1-quad100-fase5.png>
</p>
<p align="center">
	<i> Figura 5 - Identificação de número da placa </i>
</p>

Em relação à acurácia do algoritmo, o método CvKNearest foi bem parecido com o método flann, sendo que a taxa de erro nos testes realizados com os vídeos disponibilizados pela disciplina foi cerca de 3,6%. Assim como na fase 3, o processamento paralelo utilizando OpenMP não resultou em um aumento considerável nos FPS.

## Comentários finais

Apesar do bom funcionamento da fase 5 e o código desenvolvido ser base para a fase 6 de navegação autônoma com auxílio das placas, pela falta de tempo não foi possível implementar a última fase do projeto. Entretanto, a dupla considera que o resultado de todo o projeto foi bem satisfatório e os aprendizados adquiridos pela dupla durante o desenvolvimento foi de grande relevância para a formação como engenheiros.

## Referências

Site da disciplina: http://www.lps.usp.br/hae/psi3422/index.html

## Direitos autorais

O trabalho aqui publicado fez uso da biblioteca Cekeikon5 para processamento de imagens e visão computacional.

Mais informações sobre essa biblioteca pode ser encontrado em: http://www.lps.usp.br/~hae/software