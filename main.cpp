//Wersja w której klatkê pobieramy z pliku
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <zmq.hpp>
#include <string>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <vector>					
#include <fstream>
#include <math.h>

using namespace std;
using namespace cv;

#define ZAPIS_IN "VideoIN_09.avi" 
#define ZAPIS_OUT "VideoOUT_09.avi"

#define WINDOW "Main Counter"
//#define NAZWA_KLATKI "img.png"

ofstream plik("plik.txt");
//#define NAZWA_FILMIKU "agh_src30_hrc0.avi"
#define NAZWA_FILMIKU "new.mp4"

#define N 3												//(1) co ktora klatka bedzie brana do obliczen;
//#define POMNIEJSZ 2.625										//o ile ma przeskalowaæ obraz, wszystkie parametry s¹ dobrane do 1920x1080 
#define POMNIEJSZ 1.0
#define POMNIEJSZ_NAPISY 2.625											//potrzebne jest przeskalowanie ich tym parametrem do oko³o 1080/POMNIEJSZ = aktualny wymiar klatki

//Znajdz kontur
#define MIN 2100
#define MAX 640*120
#define IL_MAX_OBIEKTY 1000

#define WYSOKOSC_MAX 310					//Maksymalna wysokoœc cz³owieka jak¹ zak³adamy
#define SZEROKOSC_MAX 120					//Maksymalna szerokoœc cz³owieka jak¹ zak³adamy
#define ODL_NA_WYS 480/2
#define ODL_NA_SZE 640/64
#define POWIERZCHNIA_CZL WYSOKOSC_MAX*SZEROKOSC_MAX
#define WYP_MAX POWIERZCHNIA_CZL/32		//(320)					//WYSOKOSC_MAX*SZEROKOSC_MAX/32 //Najmniejsze wype³nienie konturu jakie bedzie brane pod uwagê jako samotne
#define POWXY_MIN ((WYSOKOSC_MAX+WYSOKOSC_MAX/2)*(SZEROKOSC_MAX+SZEROKOSC_MAX/2))/2.5//(16320)			//=(WYSOKOSC_MAX+WYSOKOSC_MAX/2)*(SZEROKOSC_MAX+SZEROKOSC_MAX/2)/1,5
#define POL_CZLOW POWXY_MIN-WYP_MAX
#define WYSW_MIN POWIERZCHNIA_CZL/6
#define ZWIEKSZ_POL_CZLOW 0.26
//#define WYSOKOSC_MAX 440/POMNIEJSZ			//Maksymalna wysokoœc cz³owieka jak¹ zak³adamy
//#define SZEROKOSC_MAX 280/POMNIEJSZ
#define R_X 1280 //w labie - 1920, laptop - 1280 
#define R_Y 800  //w labie - 1080, laptop - 800

#define NOWE_TLO 18





BackgroundSubtractorMOG2 bg_model;//(100, 3, 0.3, 5);

int rows, cols; //Rozmiary klatki
int IloscKonturow=0, IloscKonturow2 = 0, LiczbaOsob = 0, dod=0;
int NumberOfObjects=0;	//number of detected objects (previous number of )
Mat Mklatka, img, binarny;
//Zmienne przechowuj¹ce poszczególne obrazy z pliku wyjœciowego które sa nastêpnie ³¹czone w jeden o nazwie KlatkaWY
Mat Obraz1, Obraz2, Obraz3, Obraz4, KlatkaWY;
Mat frame;
Mat Zapamietana_Maska;
int uaktualnienie_tla = 0;
 struct obiekt
 {
	 int x0;
	 int y0;
	 int xk;
	 int yk;
	 int wielkosc;
	 int powxy;
	 Mat binarny;
	 bool polaczony;
	 bool wyswietl;
	 int i;
 };

 obiekt Obiekty [IL_MAX_OBIEKTY];

void AnalizujObraz (int NrK);
  void BgFb (int NrK);
  void ObrazBinarny (int NrK);
  int ZnajdzKontury (int NrK);
	void Tworzenieobiektu (int Iloscobiektow, int a1, int a2, int b1, int b2, int wielkosc, int i );		//Funkcja odpowiedzialna za stworzenie obiketu
  void Kontury (int NrK, int IlK);
	void WpiszNoweDane(int pomoc1, int pomoc2);
  void Rysuj(int NrK, int IlK);
void Rysuj_Na_Kazdej(int NrK, int IlK);
void Napisy(int NrK, int IlK);

//void Connect2Server();
void SendInfo(int n);


int AvgMean(Mat tab3_1, Mat tab3_2);

int main()
{
   // namedWindow(WINDOW, CV_WINDOW_NORMAL);
   // 	resizeWindow(WINDOW, R_X, R_Y-50);
   // 	moveWindow(WINDOW, 0, 0);

//***************INPUT***************
//*** From camera	
//	VideoCapture cap(0); // open the default camera
//	if(!cap.isOpened())  // check if we succeeded
//	  return -1;
//***

//*** From remote camera
	VideoCapture cap;
	if (!cap.open("http://149.156.203.23:8003/mjpg/video.mjpg"))		// or ports: 8001-8004
	{
		cout << "Could not open the video stream from camera" << endl;
		return -1;
	}
//***

	char* outFile = "/root/code/mjpg/out.mjpg";
	//Connect2Server();

//*** Camera input	
	cap >> frame;
	Mklatka = frame;
//***

//*** From video file
	// CvCapture* film = cvCreateFileCapture( NAZWA_FILMIKU );
	// IplImage* Iklatka;

	// Iklatka = cvQueryFrame( film );
	// Mklatka = Iklatka;
//***
//***************INPUT***************

	VideoWriter outputVideo2(ZAPIS_IN, CV_FOURCC('D','I','V','X'), 30, Mklatka.size(), true);
	outputVideo2<<Mklatka;
	//Mklatka = imread(NAZWA_KLATKI);								//Na pocz¹tku pobieramy klatkê "próbn¹" z której zostaj¹ pobrane rozmiary, aby stworzyæ t³o o odpowiednim rozmiarze
																//Za³o¿enie: ¯eby rozmiar pliku wyjœciowego dopasowywa³ siê do plików wejœciowych
																//Pobranie klatki z pliku 
	rows = Mklatka.rows;
	cols = Mklatka.cols;
	Mat MTlo(2*rows+60, 2*cols+60, 16, Scalar(132,2,8));		//Tworzymy t³o które zmieœci na sobie 4 klatki o odpowiednim rozmiarze, Mklatka.type() = 16, Scalar(132,2,8) to ten niebieski kolor itd.
	VideoWriter outputVideo(ZAPIS_OUT, CV_FOURCC('D','I','V','X'), 30, MTlo.size(), true);
	Mat gener(rows, cols, 16, Scalar(132,2,8));					//Musimy czymœ wype³niæ te Obrazy na pocz¹tek, poniewa¿ jak bêdziemy próbowali na MTlo na³o¿yæ pusty obraz to program siê wywali.  
	Obraz1 = gener.clone(); 
	Obraz2 = gener.clone(); 
	Obraz3 = gener.clone(); 
	Obraz4 = gener.clone();

	int numer_klatki = 0;										//Zliczamy która klatka
	while(1)
	{
		plik<<"--------------- "<<numer_klatki<<" ---------------"<<endl;
		cap >> frame;
		Mklatka = frame;
		//outputVideo2<<Mklatka;
		// Iklatka = cvQueryFrame( film );
		// if(!Iklatka) break;
		// Mklatka = Iklatka;
		//Mklatka = imread(NAZWA_KLATKI);							//Pobieramy klatkê z pliku
		if(numer_klatki%N == 0)
		{
			AnalizujObraz(numer_klatki);
		}
		char c = cvWaitKey(1);
		if( c == 27 ) break;						 	//Jesli nacisniemy ESC zakoñczymy program;		
		if( c == 32 ) cvWaitKey(0);						//Jesli nacisniemy Spacje zapusejemy program;
		

		KlatkaWY = MTlo.clone();
		Rysuj_Na_Kazdej(numer_klatki, IloscKonturow);
		Obraz1.copyTo(KlatkaWY.colRange(20,cols+20).rowRange(20,rows+20));
		Obraz2.copyTo(KlatkaWY.colRange(40+cols,2*cols+40).rowRange(20,rows+20));
		Obraz3.copyTo(KlatkaWY.colRange(20,cols+20).rowRange(40+rows,2*rows+40));
		Obraz4.copyTo(KlatkaWY.colRange(40+cols,2*cols+40).rowRange(40+rows,2*rows+40));
		Napisy(numer_klatki, IloscKonturow);
		//imshow(WINDOW, KlatkaWY);
		//cvWaitKey(0);
		//imwrite("OUT.png", KlatkaWY);
		outputVideo<<KlatkaWY;
		VideoWriter outStream(outFile, CV_FOURCC('M','J','P','G'), 15, MTlo.size(), true);
		outStream<<KlatkaWY;
		numer_klatki++;
	}
	system ("pause");
	return 0;
}

void AnalizujObraz(int NrK)
{
	BgFb(NrK);									//Odzielenie t³a
	ObrazBinarny(NrK);							//Czysty obraz binarny
	IloscKonturow = ZnajdzKontury(NrK);			//Zapisuje wszystkie znalezione kontury do tablicy Obiekty
	Kontury(NrK, IloscKonturow);
	Rysuj(NrK, IloscKonturow);
}

void BgFb(int NrK)
{
	Mat fgmask, fgimg;
	img = Mklatka.clone();
	if( fgimg.empty() )
	fgimg.create(img.size(), img.type());
	bool update_bg_model = true;
	bg_model(img, fgmask, update_bg_model ? -1 : 0);
	fgimg = Scalar::all(0);
	img.copyTo(fgimg, fgmask);
	Mat bgimg;
	//bg_model.set("nmixtures",3);
	//bg_model.set("detectShadows", true);
	bg_model.getBackgroundImage(bgimg);
	if(!bgimg.empty())
	Obraz4 = bgimg.clone();
	Obraz3 = fgimg.clone();
	binarny = fgmask;
	
	 if(uaktualnienie_tla + 5 < NrK)
	 {
	 	bool WARUNEK = false;
	 	int roznica = AvgMean(bgimg, img);
		cout<<roznica<<endl;
		if(roznica > NOWE_TLO) WARUNEK = true;
	 	if(WARUNEK)
	 	{
			
	 		BackgroundSubtractorMOG2 bg_model2;
			bg_model = bg_model2;
			cout<<"NOWE TLO"<<endl;
			//cvWaitKey(0);
			WARUNEK = false;
			uaktualnienie_tla = NrK;
	 	}
	  }

}


void ObrazBinarny(int NrK)
{
	Mat elementd(2,2,CV_8U,cv::Scalar(1));
	Mat elemente(2,2,CV_8U,cv::Scalar(1));

	threshold( binarny, binarny, 50,255,0);

	dilate(binarny, binarny, elementd);
	erode(binarny, binarny, elemente);
	dilate(binarny, binarny, elementd);
	erode(binarny, binarny, elemente);
	dilate(binarny, binarny, elementd);
	erode(binarny, binarny, elemente);
	dilate(binarny, binarny, elementd);
	erode(binarny, binarny, elemente);
}

int ZnajdzKontury(int NrK)
{
	vector<std::vector<cv::Point> > contours;
	vector<Point> contours_poly; 
	Rect boundRect; 

    findContours(binarny,contours,CV_RETR_EXTERNAL ,CV_CHAIN_APPROX_NONE);
	int nrkonturu = 0;
	plik<<"contours.size(): "<<contours.size()<<endl;
	for( int i = 0; i< contours.size(); i++ ) 
	{
		int wielkosc = abs(contourArea(Mat(contours[i])));
		if ( wielkosc > MIN && wielkosc < MAX ) 
		{
			drawContours(img,contours,i,cv::Scalar(0,0,255),2);
			approxPolyDP( Mat(contours[i]), contours_poly, 3, true ); 
			boundRect = boundingRect( Mat(contours_poly) ); 
			fillConvexPoly(img, contours_poly, contours_poly.size() ); 
			rectangle( img, boundRect.tl(), boundRect.br(), Scalar(255, 0, 125), 1, 8, 0 ); 
			
			int a1=boundRect.x, a2=boundRect.width, b1=boundRect.y, b2=boundRect.height, a=a1 + a2/2, b=b1 + b2/2;
			int Radius = 8;
			circle(img, Point(a,b-b2/4), Radius, Scalar(0,0,255),1,8,0);
			circle(img, Point(a,b-b2/4), Radius-2, Scalar(0,0,255),1,8,0);
			line( img, Point(a - Radius - 2 ,b-b2/4), Point(a + Radius + 2 ,b-b2/4), Scalar(0, 0, 255), 1, 8, 0); 
			line( img, Point(a ,b-b2/4  - Radius - 2), Point(a ,b-b2/4 + Radius + 2), Scalar(0, 0, 255), 1, 8, 0); 
			string spow;
			stringstream outpow;
			outpow<<"W: "<<wielkosc<<" hei: "<<b2<<" wid: "<<a2;
			
			spow = outpow.str();
			putText(img, spow, Point(a1-40,b1-5-40), FONT_HERSHEY_COMPLEX_SMALL, 4/POMNIEJSZ_NAPISY, cvScalar(0,125,255),1, CV_AA);

			string s1;
			stringstream out1;
			out1 <<"[ "<<a<<" , "<<b<<" ]";
			s1 = out1.str();
			putText(img, s1, Point(a1+a2+3,b1+b2/4),  FONT_HERSHEY_COMPLEX_SMALL, 2/POMNIEJSZ_NAPISY, cvScalar(0,255,255),1, CV_AA);

			Tworzenieobiektu(nrkonturu, a1, a2, b1, b2, wielkosc, i );
			
			nrkonturu++;
		}
	}
	return nrkonturu;
}

//Zapamiêtujemy ka¿dy kontur
void Tworzenieobiektu(int Iloscobiektow, int a1, int a2, int b1, int b2, int wielkosc, int i )
{
	if(Iloscobiektow<IL_MAX_OBIEKTY)
	{
		Obiekty[Iloscobiektow].x0 = a1; 
		Obiekty[Iloscobiektow].y0 = b1;
		Obiekty[Iloscobiektow].xk = a1+a2;
		Obiekty[Iloscobiektow].yk = b1+b2;
		Obiekty[Iloscobiektow].powxy = a2*b2;
		Obiekty[Iloscobiektow].wielkosc = wielkosc;
		Obiekty[Iloscobiektow].binarny = binarny.rowRange(b1-1,b1+b2-1).colRange(a1-1,a1+a2-1).clone();
		Obiekty[Iloscobiektow].polaczony = false;
		Obiekty[Iloscobiektow].i = i;
	}
}

void Kontury (int NrK, int IlK)  //NrK - numer klatki, ILK - Iloœc konturów z obrazu binarnego
{	
	dod=0; int mmm=0;
	for (int i = 0; i < IlK; i++)
	{		
		plik<<i<<". Wielkosc: "<<Obiekty[i].wielkosc<<" powxy/wielk: "<<Obiekty[i].powxy/Obiekty[i].wielkosc<<"   hei:   "<<Obiekty[i].yk-Obiekty[i].y0<<"   wid:   "<<Obiekty[i].xk-Obiekty[i].x0<<"  [ "<<(Obiekty[i].x0+(Obiekty[i].xk-Obiekty[i].x0)/2)<<" , "<<(Obiekty[i].y0+(Obiekty[i].yk-Obiekty[i].y0)/2)<<" ]   powxy:"<< Obiekty[i].powxy <<endl;
		if(Obiekty[i].powxy<POWIERZCHNIA_CZL) 		
		{
			plik<<i<<". powxy: "<<Obiekty[i].powxy<<" < POWIERZCHNIA_CZL: " <<POWIERZCHNIA_CZL<<endl;
			// Musimy sprawdziæ czy jakiœ cz³owiek nie jest rozpoznawany jako dwie osoby (czyli czy nie rozdzieli³ siê jego kontur na dwie czêœci)
			// Bierzemy po kolei kontury i sprawdzamy z nastêpnymi czy ich œrodki nie znajduj¹ siê blisko siebie
			for( int j = i; j < IlK; j++)
			{
				CvPoint Asri = Point(Obiekty[i].x0 + (Obiekty[i].xk - Obiekty[i].x0)/2, Obiekty[i].y0 + (Obiekty[i].yk - Obiekty[i].y0)/2);
				CvPoint Bsrj = Point(Obiekty[j].x0 + (Obiekty[j].xk - Obiekty[j].x0)/2, Obiekty[j].y0 + (Obiekty[j].yk - Obiekty[j].y0)/2);
				if( abs(Asri.y - Bsrj.y) < ODL_NA_WYS && abs(Asri.x - Bsrj.x) < ODL_NA_SZE && i!=j )
				{
					plik<<i<<". Zostaje z³¹czony z "<<j<<"gdy¿ dl_y: "<<abs(Asri.y - Bsrj.y)<<" < "<<ODL_NA_WYS<<"  i dl_x: "<<abs(Asri.x - Bsrj.x)<<" < "<<ODL_NA_SZE<< endl;
					int pomoc1 = i, pomoc2 = j;
					if(Asri.y > Bsrj.y)
					{
						 pomoc1 = j;
						 pomoc2 = i;
					}
					//Pomoc1 jest nad Pomoc2
					WpiszNoweDane(pomoc1, pomoc2);
				}
				else Obiekty[i].wyswietl = true;
			}
		}
		else if( Obiekty[i].powxy < POWIERZCHNIA_CZL/4 || Obiekty[i].wielkosc<(WYP_MAX/(POMNIEJSZ*POMNIEJSZ)) || Obiekty[i].wielkosc<2000)
		{
			plik<<i<<". nie bede wyœwietlony bo powxy<POWIERZCHNIA_CZL/4 ("<<POWIERZCHNIA_CZL/4<<") lub wielkosc < WYP_MAX  ("<<WYP_MAX<<")"<<endl;
			Obiekty[i].wyswietl = false;
		}
		plik<<i<<". powierzchnie wieksz¹ ni¿ "<<POWXY_MIN<<" ??? "<<endl;
		if(Obiekty[i].powxy>POWXY_MIN/(POMNIEJSZ*POMNIEJSZ))
		{
			plik<<i<<". Mam powierzchnie wieksz¹ ni¿ POWXY_MIN"<<POWXY_MIN<<endl;
			//Mamy prawdopodobnie wiêcej ni¿ jedngeo cz³owieka lub jakiœ wiêkszy obiekt np.: samochód itd...
			string spow;
			stringstream outpow;
			outpow<<"("<<Obiekty[i].x0<<" ,"<<Obiekty[i].y0<<" )";
			spow = outpow.str();
			putText(img, spow, Point(Obiekty[i].x0,Obiekty[i].y0), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0,255,255),1, CV_AA);
			int licz;
			int j = 0, max_wiel = POL_CZLOW/(POMNIEJSZ*POMNIEJSZ), maxPW = 3;
			mmm=0;
			while(1)
			{
				plik<<i<<". etap 1 max_wiel = POL_CZLOW = "<<max_wiel<<" ??? "<<endl;
				//wymyœlamy algorytm na to jak przewidzieæ ile osob znajduje sie w danym obszarze
				if(Obiekty[i].wielkosc<=max_wiel && Obiekty[i].powxy/Obiekty[i].wielkosc<maxPW) {dod = j;  break;}
				max_wiel +=max_wiel*ZWIEKSZ_POL_CZLOW;
				plik<<i<<". etap 2 max_wiel "<<max_wiel<<" ??? "<<endl;
				if((Obiekty[i].wielkosc<=max_wiel) && Obiekty[i].powxy/Obiekty[i].wielkosc <= (maxPW-1)) {dod = j;  break;}
				j++; mmm++;
				maxPW++;
			}
			plik<<"Kontur nr "<<i<<". Zliczany jako "<<mmm<<" osob"<<endl; 
		}
	}
}

void WpiszNoweDane(int pomoc1, int pomoc2)
{
	 int x0 = Obiekty[pomoc1].x0;
	 int y0 = Obiekty[pomoc1].y0;
	 int xk = Obiekty[pomoc1].xk;
	 int yk = Obiekty[pomoc2].yk;
	 int wielkosc = Obiekty[pomoc1].wielkosc + Obiekty[pomoc2].wielkosc;
	 int powxy =  (yk - y0)*(xk - x0);
	if((yk - y0)< WYSOKOSC_MAX || (xk-x0)<SZEROKOSC_MAX)
	{
		Obiekty[pomoc1].x0 = x0; 
		Obiekty[pomoc1].y0 = y0;
		Obiekty[pomoc1].xk = xk;
		Obiekty[pomoc1].yk = yk;
		Obiekty[pomoc1].powxy = powxy;
		Obiekty[pomoc1].wielkosc = wielkosc;
		Obiekty[pomoc1].polaczony = true;
		Obiekty[pomoc1].wyswietl = true;
		Obiekty[pomoc2].x0 = x0; 
		Obiekty[pomoc2].y0 = y0;
		Obiekty[pomoc2].xk = xk;
		Obiekty[pomoc2].yk = yk;
		Obiekty[pomoc2].powxy = powxy;
		Obiekty[pomoc2].wielkosc = wielkosc;
		Obiekty[pomoc2].polaczony = true;
		Obiekty[pomoc2].wyswietl = true;
	}
}

void Rysuj(int NrK, int IlK)
{
	//cout << "Number: " << IlK << endl;
	SendInfo(IlK-NumberOfObjects);
	NumberOfObjects = IlK;
	Mat Pusty = Mklatka - Mklatka;
	for (int i = 0; i < IlK; i++)
	{
		if(Obiekty[i].wyswietl && Obiekty[i].wielkosc>(100*16/(POMNIEJSZ*POMNIEJSZ))){
		rectangle( img, Point(Obiekty[i].x0, Obiekty[i].y0), Point(Obiekty[i].xk, Obiekty[i].yk), Scalar(0, 0, 255), 2, 8, 0 );
		int Radius = 18/POMNIEJSZ_NAPISY;
		int a = Obiekty[i].x0 + (Obiekty[i].xk - Obiekty[i].x0)/2;
		int b = Obiekty[i].y0 + (Obiekty[i].yk - Obiekty[i].y0)/2;
		int b2 = Obiekty[i].yk - Obiekty[i].y0;
		int dr = 25/POMNIEJSZ_NAPISY; float gr = 10/POMNIEJSZ_NAPISY; //d³ugoœæ rameczki, grubosc
		line( Mklatka, Point(Obiekty[i].x0,Obiekty[i].y0), Point(Obiekty[i].x0+dr,Obiekty[i].y0), Scalar(30, 170, 230), gr, 8, 0); 
		line( Mklatka, Point(Obiekty[i].x0,Obiekty[i].y0), Point(Obiekty[i].x0,Obiekty[i].y0+dr), Scalar(30, 170, 230), gr, 8, 0); 

		line( Mklatka, Point(Obiekty[i].xk,Obiekty[i].y0), Point(Obiekty[i].xk-dr,Obiekty[i].y0), Scalar(30, 170, 230), gr, 8, 0); 
		line( Mklatka, Point(Obiekty[i].xk,Obiekty[i].y0), Point(Obiekty[i].xk,Obiekty[i].y0+dr), Scalar(30, 170, 230), gr, 8, 0); 

		line( Mklatka, Point(Obiekty[i].x0,Obiekty[i].yk), Point(Obiekty[i].x0+dr,Obiekty[i].yk), Scalar(30, 170, 230), gr, 8, 0); 
		line( Mklatka, Point(Obiekty[i].x0,Obiekty[i].yk), Point(Obiekty[i].x0,Obiekty[i].yk-dr), Scalar(30, 170, 230), gr, 8, 0); 

		line( Mklatka, Point(Obiekty[i].xk,Obiekty[i].yk), Point(Obiekty[i].xk,Obiekty[i].yk-dr), Scalar(30, 170, 230), gr, 8, 0); 
		line( Mklatka, Point(Obiekty[i].xk,Obiekty[i].yk), Point(Obiekty[i].xk-dr,Obiekty[i].yk), Scalar(30, 170, 230), gr, 8, 0); 

		circle(Pusty, Point(a,b-b2/4), Radius-2, Scalar(255,255,255),-1,8,0);
		}
		else
		{
			rectangle( Mklatka, Point(Obiekty[i].x0, Obiekty[i].y0), Point(Obiekty[i].xk, Obiekty[i].yk), Scalar(30, 230, 170), 1, 8, 0 );
		}
		string spow;
			stringstream outpow;
			outpow<<"("<<Obiekty[i].x0<<" ,"<<Obiekty[i].y0<<" )";
			spow = outpow.str();
			putText(img, spow, Point(Obiekty[i].x0,Obiekty[i].y0), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(0,255,255),1, CV_AA);
	}
	vector<std::vector<cv::Point> > contours;
	int Liczba = 0;
	cvtColor(Pusty, Pusty, COLOR_RGB2GRAY);
	findContours(Pusty,contours,CV_RETR_EXTERNAL ,CV_CHAIN_APPROX_NONE);
	for( int i = 0; i< contours.size(); i++ ) 
	{
		Liczba++;
	}
	LiczbaOsob = Liczba + dod;
	Obraz2 = img.clone();
	Obraz1 = Mklatka.clone();
}


void Rysuj_Na_Kazdej(int NrK, int IlK)
{
	for (int i = 0; i < IlK; i++)
	{
		if(Obiekty[i].wyswietl && Obiekty[i].wielkosc>(WYSW_MIN/(POMNIEJSZ*POMNIEJSZ)))
		{
			int a = Obiekty[i].x0 + (Obiekty[i].xk - Obiekty[i].x0)/2;
			int b = Obiekty[i].y0 + (Obiekty[i].yk - Obiekty[i].y0)/2;
			int b2 = Obiekty[i].yk - Obiekty[i].y0;
			int dr = 25/POMNIEJSZ_NAPISY; float gr = 10/POMNIEJSZ_NAPISY; //d³ugoœæ rameczki, grubosc
			line( Obraz1, Point(Obiekty[i].x0,Obiekty[i].y0), Point(Obiekty[i].x0+dr,Obiekty[i].y0), Scalar(30, 170, 230), gr, 8, 0); 
			line( Obraz1, Point(Obiekty[i].x0,Obiekty[i].y0), Point(Obiekty[i].x0,Obiekty[i].y0+dr), Scalar(30, 170, 230), gr, 8, 0); 

			line( Obraz1, Point(Obiekty[i].xk,Obiekty[i].y0), Point(Obiekty[i].xk-dr,Obiekty[i].y0), Scalar(30, 170, 230), gr, 8, 0); 
			line( Obraz1, Point(Obiekty[i].xk,Obiekty[i].y0), Point(Obiekty[i].xk,Obiekty[i].y0+dr), Scalar(30, 170, 230), gr, 8, 0); 

			line( Obraz1, Point(Obiekty[i].x0,Obiekty[i].yk), Point(Obiekty[i].x0+dr,Obiekty[i].yk), Scalar(30, 170, 230), gr, 8, 0); 
			line( Obraz1, Point(Obiekty[i].x0,Obiekty[i].yk), Point(Obiekty[i].x0,Obiekty[i].yk-dr), Scalar(30, 170, 230), gr, 8, 0); 

			line( Obraz1, Point(Obiekty[i].xk,Obiekty[i].yk), Point(Obiekty[i].xk,Obiekty[i].yk-dr), Scalar(30, 170, 230), gr, 8, 0); 
			line( Obraz1, Point(Obiekty[i].xk,Obiekty[i].yk), Point(Obiekty[i].xk-dr,Obiekty[i].yk), Scalar(30, 170, 230), gr, 8, 0); 
		}
		else
		{
			rectangle( Obraz1, Point(Obiekty[i].x0, Obiekty[i].y0), Point(Obiekty[i].xk, Obiekty[i].yk), Scalar(30, 230, 170), 1, 8, 0 );
		}
	}

}

void Napisy(int NrK, int IlK)
{
	string spow;
	stringstream outpow;
	outpow<<"X: "<<LiczbaOsob;
	spow = outpow.str();

	putText(KlatkaWY, spow, Point(15+ cols/2 - 240, 15 ), FONT_HERSHEY_COMPLEX_SMALL, 2.5/POMNIEJSZ_NAPISY, cvScalar(0,255,255),1, CV_AA);
	string spow2;
	stringstream outpow2;
	outpow2<<"Nr klatki: "<<NrK/N<<"/("<<NrK<<") ";
	spow2 = outpow2.str();
	putText(KlatkaWY, spow2, Point(15+ cols - 240, 15 ), FONT_HERSHEY_COMPLEX_SMALL, 2/POMNIEJSZ_NAPISY, cvScalar(0,255,255),1, CV_AA);
}

int AvgMean (Mat tab3_1, Mat tab3_2)
{
    Scalar bg =  mean(tab3_1), kl = mean(tab3_2);
    return abs(abs(bg[0] - kl[0])+abs(bg[1] - kl[1])+abs(bg[2] - kl[2]))/3.0;
}
/*
void Connect2Server()	// not used
{
	//  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to hello world server…" << std::endl;
    socket.connect ("tcp://localhost:5555");	// localhost or server IP
}
*/
void SendInfo(int n)
{
	if (n == 0) return;
	int event = (n > 0) ? 1 : 2;
	char texts[2][35] = {"Person entered a restricted area", "Person left a restricted area"};
	//  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    //std::cout << "Connecting to hello world server…" << std::endl;
    socket.connect ("tcp://localhost:5555");	// localhost or server IP
	
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [128];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer, 32, "%d %b %G %H:%M:%S", timeinfo);
	
	zmq::message_t message (128);
    //memcpy (message.data(), "Hello !!! :)", 15);
	//snprintf ((char *) message.data(), 15, "%s", "Hello !!! :)");
	snprintf ((char *) message.data(), 128, "%s 1 %d info \"%s\" confidence=0.8 zoom=3 rotation=0", buffer, event, texts[event-1]);
	socket.send (message);
}
