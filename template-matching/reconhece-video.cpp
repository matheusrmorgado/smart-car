#include <cekeikon.h>

void printSQR( Mat_<COR>& img, Mat_<FLT> quadrado){				
	
	Mat_<FLT> imgFLT(img.size());
	Mat_<FLT> result;
	
	for (int ii = 0; ii  < img.rows; ii++)
		for (int jj = 0; jj  < img.cols; jj++)
			imgFLT(ii,jj) = (float)(img(ii,jj)[0])/256;
		
		
	double minVal; double maxVal;
	Point minLoc; Point maxLoc; Point matchLoc;

	int tamanho; float aux = 0.0;
	
	#pragma omp parallel for
	for ( int s = 15; s < 61; s = s + 6){
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

	
	if( aux > 0.23f){
		retang(img, matchLoc.y,matchLoc.x-1, matchLoc.y-1 + tamanho-1,matchLoc.x + tamanho-1, COR(255,0,0));
	}

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
	videoFile += "-detection.avi";
	VideoWriter vo(videoFile, CV_FOURCC('X','V','I','D'), fps, Size(nc,nl));

	//Create matrix
	Mat_<COR> src;
	Mat_<FLT> quadrado; 
	le( quadrado, "template-new.png" );

	//Frame analysis
	for (int i = 1; i <= frames; i++) 
	{
		
		vi >> src;
		
		if (i == 450) 
			imp( src, "quad450-detection.png" ); 

		printSQR(src, quadrado); 

		//Show results
		namedWindow("Template Matching Video", 0);
		imshow("Template Matching Video", src);
		if (waitKey(30) >= 0) 
			break;

		//Save image in video
		vo << src;
	}

	cout << "\n\t" + videoFile + " gerado com sucesso\n";
	destroyAllWindows();
}