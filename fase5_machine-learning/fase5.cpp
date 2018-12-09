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

	#pragma omp parallel for
	for ( int s = 25; s < 60; s = s+5){
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

	cv::Rect rect;
	if( maxVal > 0.4f){
		rectangle( img, matchLoc, Point( matchLoc.x + quadrado.cols , matchLoc.y + quadrado.rows ), Scalar(0,0,255), 1.5, 8);
		rect = Rect(matchLoc.y + tamanho*0.3 ,matchLoc.x  + tamanho*0.3, tamanho*0.4, tamanho*0.4);
	}
	else
		rect = Rect(0,0,0,0);
	 
	return rect;
	
}

int printNumber(CvKNearest& ind, MNIST& mnist, Mat_<COR>& img, Rect rec){
	
	if (rec.width < 15)
		return(-1);
	
	Mat_<GRY> insideSQR(rec.width,rec.height);

	#pragma omp parallel for	
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
	
	Mat_<GRY> insideBox = mnist.bbox(insideSQR);
	
	
	Mat_<FLT> matToFind(1,28*28); 
	for(int i=0; i<28; i++)
		for(int j=0; j<28; j++)
			matToFind(i*28 + j) = (float)(insideBox(i,j))/255.0;
 
	Mat_<FLT> result(1,1);
	
	ind.find_nearest(matToFind, 1, &result);

	cv::putText(img, std::to_string((int)result(0,0)), 
		Point(rec.y, rec.x + rec.width), FONT_HERSHEY_SCRIPT_SIMPLEX, 0.5, Scalar::all(255), 1, 8);
		
	return result(0,0);
	
}
	
int main( int argc, char** argv )
{
	// Read user input
	if (argc != 2) 
		erro("\n-- Erro -- \nNumero de argumentos invalido\nExemplo de entrada: reconhece-video video.avi\n");
	
	// Read input video
	string videoFile = argv[1];

	if (!existeArq(videoFile)) 
		erro("\n-- Erro -- \nNao existe video com o nome " + videoFile + " \n");
	
	VideoCapture vi(videoFile);

	float fps = vi.get(CV_CAP_PROP_FPS);
	int nc = vi.get(CV_CAP_PROP_FRAME_WIDTH);
	int nl = vi.get(CV_CAP_PROP_FRAME_HEIGHT);
	int frames = vi.get(CV_CAP_PROP_FRAME_COUNT);

	// Create output video
	videoFile.erase(videoFile.end()-4, videoFile.end());
	string videoName = videoFile;
	videoFile += "-fase5.avi";
	VideoWriter vo(videoFile, CV_FOURCC('X','V','I','D'), fps, Size(nc,nl));

	// Create matrix
	Mat_<COR> src(nl,nc);
	Mat_<FLT> quadrado; 
	le( quadrado, "template-new.png" );
	
	// Machine Learning
	MNIST mnist(28, true, true);
	mnist.le("./mnist");
	CvKNearest ind(mnist.ax, mnist.ay, Mat(), false, 1);

	int number = -1;
	printf("\nAguarde... O video esta sendo processado\n");

	// Frame analysis
	for (int i = 1; i <= frames; i++) 
	{		
		//Save frame in image
		vi >> src;
		if (i == 100) imp( src, videoName + "-quad100.png" ); 
		
		cv::Rect rect;
		rect = printSQR(src, quadrado);
		number = printNumber(ind, mnist, src, rect);

		if (i == 100) imp( src, videoName + "-quad100-fase5.png" ); 
		//Show results
		namedWindow("Template Matching - Machine Learning Video", 0);
		imshow("Template Matching - Machine Learning Video", src);
		if (waitKey(30) >= 0) 
			break;

		//Save image in output video
		vo << src;
	}

	cout << "\n\t" + videoFile + " gerado com sucesso\n";
}