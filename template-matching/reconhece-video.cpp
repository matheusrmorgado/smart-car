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
	//begin
	if( argc != 3) erro(" wrong argument number \n " );
	
	char* vidName = argv[1];

	if (!existeArq(vidName)) erro("There is nothing with that name in that folder or whatever \n");
	  VideoCapture vi(vidName);
	if (!vi.isOpened()) erro("Opening error \n ");

	
	//Create matrix
	Mat_<COR> img(vi.get(CV_CAP_PROP_FRAME_HEIGHT ), vi.get(CV_CAP_PROP_FRAME_WIDTH ));
	Mat_<FLT> quadrado; 
	le( quadrado, "template-new.png" );
	
	//Creating the output file
	int ex = static_cast<int>(vi.get(CV_CAP_PROP_FOURCC));
	Size size(vi.get(CV_CAP_PROP_FRAME_WIDTH), vi.get(CV_CAP_PROP_FRAME_HEIGHT)); 
	VideoWriter writer(argv[2], ex, vi.get(CV_CAP_PROP_FPS), size, true);	

	//Picking frame
	while (vi.grab()) {
		
		vi.retrieve(img);

		printSQR(img,quadrado); 
		
		writer.write(img);
		
		printf("\r Processing %u s ", (int)vi.get(CV_CAP_PROP_POS_MSEC)/1000);
		
		}
}