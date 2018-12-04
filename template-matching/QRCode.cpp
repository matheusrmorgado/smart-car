#include <cekeikon.h>

double minVal; double maxVal;
Point minLoc; Point maxLoc; Point matchLoc;

Mat_<COR> qrCode;
Mat_<FLT> image;
Mat_<FLT> templ; 
Mat_<FLT> result;

int achaTamanho( Mat_<FLT> image, Mat_<FLT> templ, Mat_<FLT> result ){
	
	int tamanho; float aux = 0.0;
	
	for ( int s = 30; s < 120; s++){
		resize( templ, templ, Size(s,s), 0, 0, INTER_AREA );
		templ = somaAbsDois( dcReject( templ ) );
		matchTemplate( image, templ, result, CV_TM_CCORR );
		minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc );	
		if ( maxVal > aux){
			aux = maxVal;
			tamanho = s;
		}
	}
	return tamanho;
}

Mat_<FLT> pinta( Mat_<FLT> a, int ls, int cs ) {
	
	Mat_<FLT> b = a.clone();
	queue<int> q;
	q.push( ls ); q.push( cs );
	
	for( int i = 0; i < 150; i++ ) {
		int l = q.front(); q.pop();
		int c = q.front(); q.pop();
		b(l,c) = 0;
		q.push( l-1 ); q.push( c ); //N
		q.push( l+1 ); q.push( c ); //S
		q.push( l ); q.push( c+1 ); //E
		q.push( l ); q.push( c-1 ); //W
	}
	return b;
}

void localizaQR( Mat_<COR> qrCode, Mat_<FLT> templ, Mat_<FLT> result ){
	
	minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc );
	matchLoc = maxLoc;

	line( qrCode, Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 + 0.12*templ.rows ), 
		Point( matchLoc.x + templ.cols/2 , matchLoc.y + templ.rows/2 - 0.12*templ.rows ), Scalar(0,0,255), 1, 8);
	line( qrCode, Point( matchLoc.x + templ.cols/2 + 0.12*templ.rows , matchLoc.y + templ.rows/2 ), 
		Point( matchLoc.x + templ.cols/2 - 0.12*templ.rows , matchLoc.y + templ.rows/2 ), Scalar(0,0,255), 1, 8);
	rectangle( qrCode, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar(0,0,255), 1.5, 8);

	string str = to_string(maxVal);
	putTxt( qrCode, matchLoc.y + templ.rows/2 + 35, matchLoc.x + templ.cols/2 + 2, 
		str, COR(255,0,0) , 1, true, COR(0,0,0));
		
	result = pinta( result, matchLoc.y, matchLoc.x );
	cout << " MatchLoc: " << maxVal << endl;
	
}

int main( int argc, char* argv[] ){				
	
	le( qrCode, argv[1] );
	le( image, argv[1] );
	le( templ, "padrao.png" );

	int result_cols =  image.cols - templ.cols + 1;
	int result_rows = image.rows - templ.rows + 1;
	result.create( result_rows, result_cols );
	
	int s = achaTamanho(image,templ,result);

	resize( templ, templ, Size(s,s), 0, 0, INTER_AREA );
	templ = somaAbsDois( dcReject( templ ) );
	matchTemplate( image, templ, result, CV_TM_CCORR );
	
	//mostra( result );

	localizaQR( qrCode, templ, result );
	
	mostra( qrCode );
	imp( qrCode, argv[2] );
	cout << "\n Imagem " << argv[2] << " gerada com sucesso!" << endl;
}