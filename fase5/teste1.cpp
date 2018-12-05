#include <cekeikon.h>


cv::Rect printSQR( Mat_<COR>& img, Mat_<FLT> quadrado){				
	
	Mat_<FLT> imgFLT(img.size());
	Mat_<FLT> result;
	
	for (int ii = 0; ii  < img.rows; ii++)
		for (int jj = 0; jj  < img.cols; jj++)
			imgFLT(ii,jj) = (float)(img(ii,jj)[0])/256;
		
		
	double minVal; double maxVal;
	Point minLoc; Point maxLoc; Point matchLoc;

	int tamanho; float aux = 0.0;
	
	for ( int s = 25; s < 80; s = s+5){
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

	cv::Rect rect;
	if( aux > 0.4f){
		retang(img, matchLoc.y,matchLoc.x-1, matchLoc.y-1 + tamanho-1,matchLoc.x + tamanho-1, COR(255,0,0));
		rect = Rect(matchLoc.y + tamanho*0.3,matchLoc.x  + tamanho*0.3 ,tamanho*0.4,tamanho*0.4);
	}
	else
		rect = Rect(0,0,0,0);
	 
	return rect;
	
}

flann::Index train(MNIST& mnist){
	// BIIIIIIIIIRRRRRL
	int t1=centseg();
	printf("training \n\n");
	flann::Index ind(mnist.ax,flann::KDTreeIndexParams(4));
	
	int t2=centseg();
	for (int l=0; l<mnist.nq; l++) {
		vector<int> indices(1);
		vector<float> dists(1);
		ind.knnSearch(mnist.qx.row(l),indices,dists,1);
		mnist.qp(l)=mnist.ay(indices[0]);
	}
	int t3=centseg();
	int tt=t2-t1;
	printf("Tempo de treinamento: %d.%02d\n",tt/100,tt%100);
	int tp=t3-t2;
	printf("Tempo de predicao: %d.%02d\n",tp/100,tp%100);
	xdebug1("contando erros e gerando imagem de erros");
	printf("Erros=%10.2f\n",100.0*mnist.contaErros()/mnist.nq);
	Mat_<GRY> e=mnist.geraSaidaErros(10);
	imp(e,"flann.png");
	
	return ind;
}

int printNumber(flann::Index& ind, MNIST& mnist, Mat_<COR>& img, Rect rec){
	
	if (rec.width < 20)
		return(-1);
	
	Mat_<GRY> insideSQR(rec.width,rec.height);
	
	for (int i=0; i<rec.width; i++){
		for (int j=0; j<rec.height; j++){
			insideSQR(i,j) = img(i+rec.x,j+rec.y)[0]/3
							+ img(i+rec.x,j+rec.y)[1]/3 
							+ img(i+rec.x,j+rec.y)[2]/3;
			if (insideSQR(i,j) > 140)
				insideSQR(i,j) = 255;
			else
				insideSQR(i,j) = insideSQR(i,j)/1.5;
		}
	}
	//mostra (insideSQR);
	
	Mat_<GRY> resizedInsideSQR(24,24);
	cv::resize(insideSQR, resizedInsideSQR, cv::Size(24,24));
	//mostra (resizedInsideSQR);
	
	Mat_<FLT> matToFind(1,24*24); 
	for(int i=0; i<24*24; i++){
		matToFind(i) = (float)(resizedInsideSQR(i))/255.0;
	}
		
	vector<int> indices(1);
	vector<float> dists(1);
	ind.knnSearch(matToFind,indices,dists,1);
	
	cv::putText(img, std::to_string((int)(mnist.ay(indices[0]))), 
	Point(rec.x, rec.y +10),FONT_HERSHEY_SCRIPT_SIMPLEX, 0.5, Scalar::all(255),1,8);
		
	return mnist.ay(indices[0]);
	
}
	
int main( int argc, char** argv )
{
	//begin
	bool debug = false;
	if( argc != 3) erro(" wrong argument number \n " );
	
	char* vidName = argv[1];

	if (!existeArq(vidName)) erro("There is nothing with that name in that folder or whatever \n");
	  VideoCapture vi(vidName);
	if (!vi.isOpened()) erro("Opening error \n ");

	//Larn how to count
	MNIST mnist(24, true, false);
	mnist.le("./mnist");
	flann::Index ind = train(mnist);
	
	//Crete matrix
	Mat_<COR> img(vi.get(CV_CAP_PROP_FRAME_HEIGHT ),vi.get(CV_CAP_PROP_FRAME_WIDTH ));
	Mat_<FLT> quadrado; 
	le( quadrado, "quadradoTop.png" );
	
	//Creating the output file
	int ex = static_cast<int>(vi.get(CV_CAP_PROP_FOURCC));
	Size size(vi.get(CV_CAP_PROP_FRAME_WIDTH), vi.get(CV_CAP_PROP_FRAME_HEIGHT)); 
	VideoWriter writer(argv[2], ex, vi.get(CV_CAP_PROP_FPS), size, true);	

	int time;
	int * cValues;
	double percentage = 0.0;
	int number = -1;
	
	//Picking frame
	while (vi.grab()) {
		
		vi.retrieve(img);
		time = vi.get(CV_CAP_PROP_POS_MSEC);
		
		cv::Rect rect;
		rect = printSQR(img,quadrado);

		number = printNumber(ind,mnist,img,rect);
		
		writer.write(img);
		
		}
}