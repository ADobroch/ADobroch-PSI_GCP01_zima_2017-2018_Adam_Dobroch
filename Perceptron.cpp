#include <iostream>
#include <vector>
#include<cstdlib>
#include<time.h>
#include<fstream>
#include<cmath>
#include<Windows.h>


using namespace std;

#define LEARNING_RATE	 0.4
#define MAX_ITERATION	 100


float randomNumber()
{
	return (float)rand() / (float)RAND_MAX;
}

double fRand(double fMin, double fMax)
{
	
	double f = (double)rand() / RAND_MAX;
	//Sleep(300);
	return fMin + f * (fMax - fMin);
//	return f;
}


int mno¿eniePrzezWagi(float wagi[],float x, float y)
{
	float suma = x* wagi[0] + y*wagi[1]+ wagi[2];
	return (suma >= 0) ? 1 : -1;

}




int main()
{
	

	//fstream plik;
	float x[100], y[100], wagi[3], bladLokalny, bladGlobalny;
	int outputTest[100], output;


/*	plik.open("nazwa_pliku.txt", std::ios::in | std::ios::out);
	if (plik.good() == true)
	{
		
		plik.close();
	}
	*/
	for (int i = 0; i < 50; i++)
	{
		x[i] = fRand(5, 10);
		y[i] = fRand(4, 8);
		outputTest[i] = 1;
		//cout << x[i] << " " << y[i] << endl;
		
		
	}
	for (int i = 50; i < 100; i++)
	{
		x[i] = fRand(-1, 3);
		y[i] = fRand(-4, 2);
		outputTest[i] = -1;

	}


	wagi[0] = 0.3;
	
	wagi[1] =0.3 ;

	wagi[2] = 0.3;
	
	float min = 1.0;
	int iteracja = 0;

	do {

		iteracja++;
		bladGlobalny = 0;
		for (int i = 0; i < 100; i++)
		{
			output = mno¿eniePrzezWagi(wagi,x[i],y[i]);

			bladLokalny = outputTest[i] - output;
			
			wagi[0] += LEARNING_RATE* bladLokalny*x[i];
			wagi[1] += LEARNING_RATE *bladLokalny*y[i];
			wagi[2] += LEARNING_RATE* bladLokalny;
		//	cout << wagi[0] << " " << wagi[1] << endl;
			bladGlobalny += (bladLokalny*bladLokalny);
		}
		if (sqrt(bladGlobalny / 100) < min) min = sqrt(bladGlobalny / 100);
		cout << "Iteracja: " << iteracja << "\n Blad globalny: \n " << sqrt(bladGlobalny/100) << endl;
		
		//cout << wagi[0] << " " << wagi[1] << endl;
	} while (bladGlobalny != 0 && iteracja<=MAX_ITERATION);
	
	cout << min << endl;
	system("pause");
}



