#include <time.h>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <vector>

#define ROWS 5						/* szerokoœæ litery w px */
#define COLUMNS 7					/* wysokosc litery w px */
#define N_BYTES ROWS * COLUMNS				/* rozmiar wektora wejœcia tworzonego ze wszystki px litery */
#define N_LETTERS 20					/* liczba liter ucz¹cych */
#define N_TEST 6					/* liczba liter testuj¹cych */

using namespace std;

struct Letter
{
	char letter;					// char przechowywujacy dana litere w postaci ASCII
	int byte_series[N_BYTES];			// seria znakow w postaci 1 i 0 przedstawiajacych dana litere w formacie graficznym 5x7
	int y;						// y=1 jezeli litera jest duza, y=0 jezeli litera jest mala

	Letter();
};

struct Adaline
{
	std::vector<Letter> input_vector;		// wektor liter uczacych
	std::vector<double> weight_vector;		// wektor wag
	const double learning_rate;			// wspolczynnik nauki
	int output;					// obliczone wyjscie
	double sums[N_BYTES];				// tablica sum px * waga

	Adaline();
};

struct Tester
{
	std::vector<Letter> input_vector;		// wektor liter testujacych

	Tester();
};

void loadBytes(const char*, std::vector<Letter>&);
void randWeights(std::vector<double>&);
void writeUp(std::vector<Letter>);
std::vector<double> correctWeights(std::vector<double>, Adaline*);
double countSum(Adaline*, int);
double func(double);
double countMSE(Adaline*);
double countMAPE(Adaline*);
void testuj(Adaline*, Tester*);

//===================================================================================================== MAIN
int main(int argc, char **argv)
{
	int i, epoka = 0;
	double MSE, MAPE;

	Adaline *ada = new Adaline();
	loadBytes("input.txt", ada->input_vector);
	randWeights(ada->weight_vector);

	do {
		printf("epoka %d: \n", epoka);
		//	for each training sample
		//	CALCULATE THE OUTPUT VALUE
		for (i = 0; i<N_BYTES; i++) {
			ada->sums[i] = countSum(ada, i);
		}

		MSE = countMSE(ada);
		MAPE = countMAPE(ada);

		// 	COUNT ERRORS
		printf("MSE:%.2f\n", MSE);
		printf("MAPE:%.2f percent\n", MAPE);

		//	UPDATE THE WEIGHTS
		ada->weight_vector = correctWeights(ada->weight_vector, ada);
		epoka++;
	} while (epoka<20);

	Tester *test = new Tester();
	loadBytes("input_test.txt", test->input_vector);

	printf("\nLiczba potrzebnych epok: %d\n", epoka);
	testuj(ada, test);
	system("pause");

	return 0;
}
//===================================================================================================== MAIN

Letter::Letter() { letter = 'a'; for (int i = 0; i<N_BYTES; i++)byte_series[i] = 0; }	// zarezerwowanie 35 miejsc dla bajtow dla pojedynczej litery
Adaline::Adaline() :learning_rate(0.01) {
	Letter l;
	for (int i = 0; i<N_LETTERS; i++) {
		this->input_vector.push_back(l);		// Zarezerwowanie miejsca dla 20 liter w wektorze
	}
	for (int i = 0; i<N_BYTES; i++) {
		this->weight_vector.push_back(0);		// zarezerwowanie 35 miejsc dla wag
	}
}
Tester::Tester() {
	Letter l;
	for (int i = 0; i<N_TEST; i++) {
		this->input_vector.push_back(l);		// zarezerwowanie 5 miejsc dla danych testowych
	}
}



void loadBytes(const char* fn, std::vector<Letter>& iv)
{
	std::fstream file;					// Plik z którego pobieramy dane ucz¹ce
	std::vector<Letter>::iterator it;			// Iterator do przechodzenia po wektorze liter
	char b;							// Pomocnicza zmienna przy odczycie z pliku
	file.open(fn);						// Otwieranie pliku o nazwie fn
	it = iv.begin();					// Ustawienie iteratora na pocz¹tek

	do							// Dopóki nie odczytamy wszystkich wartosæi z pliku
	{
		file >> (*it).letter;				// Wczytujemy z pliku oczekiwane litery
		for (int i = 0; i<N_BYTES; i++)
		{
			file >> b;
			if (b == '1') (*it).byte_series[i] = 1;
			else (*it).byte_series[i] = 0;		// Wczytujemy z pliku ci¹gi bajtów oznaczaj¹ce dane ucz¹ce - litery
		}

		file >> (*it).y;				// Je¿eli y = 1 litera jest du¿a, je¿eli y = 0 litera jest ma³a 
		it++;
	} while (it != iv.end());

	file.close();
}

void writeUp(std::vector<Letter> iv)
{
	std::vector<Letter>::iterator it;
	it = iv.begin();
	for (; it != iv.end(); ++it)
	{
		printf("%c:", (*it).letter);
		for (int i = 0; i<N_BYTES; i++) {													// wypisanie liter w postaci graficznej tablicy 5x7
			if (i%ROWS == 0) printf("\n");
			printf("%d", (*it).byte_series[i]);
		}
		printf("\n");
	}
}

void randWeights(std::vector<double>& iv) {
	srand(time(NULL));
	std::vector<double>::iterator it;

	it = iv.begin();
	for (; it != iv.end(); ++it) {
		(*it) = (double)rand() / (double)RAND_MAX;											// losowanie wag
	}
}

std::vector<double> correctWeights(std::vector<double> wv, Adaline *ada) {
	std::vector<double> TEMPORARY = wv;
	std::vector<double>::iterator wt;
	wt = TEMPORARY.begin();
	for (int i = 0; i<N_LETTERS; i++, ++wt) {
		printf("\nbefore %f -> ", (*wt));
		double deltaW = 0;
		for (int j = 0; j<N_BYTES; j++) {
			deltaW += ada->learning_rate*(ada->input_vector[i].y - func(ada->sums[j])) * ada->input_vector[i].byte_series[j];	//liczenie deltaW
		}
		(*wt) += deltaW;														//zaktualizowanie wag o deltaW
		printf("updated w %f", (*wt));
	}

	return TEMPORARY;
}

double countSum(Adaline *a, int letter) {
	double sum = 0;
	for (int j = 0; j<N_LETTERS; j++) {
		for (int i = 0; i<N_BYTES; i++) {
			sum += a->input_vector[j].byte_series[i] * a->weight_vector[i]; 	// Liczenie sumy ci¹gu bajtów razy odpowiadaj¹ce wagi dla danej litery
		}
	}
	return sum;
}

double countMAPE(Adaline *ada) {
	double tmp = 0;
	for (int i = 0; i<N_LETTERS; i++) {
		for (int j = 0; j<N_BYTES; j++) {
			tmp += (fabs((func(ada->sums[j]) - ada->input_vector[i].y)) / func(ada->sums[j])); 	// wartosc bezwzgledna 
		}
	}
	tmp /= (double)N_BYTES;											// podzielenie przez liczbe px
	return tmp;
}

double countMSE(Adaline *ada) {
	double tmp = 0;
	for (int i = 0; i<N_LETTERS; i++) {
		for (int j = 0; j<N_BYTES; j++) {
			tmp += pow(func(ada->sums[j]) - ada->input_vector[i].y, 2);	// suma kwadratow
		}
		tmp /= (double)N_BYTES;								// podzielenie przez liczbe px
	}
	return tmp;
}

double func(double sum) {								// Funkcja aktywacji, wspolczynnik beta = 0.5 
	return 1 / (1 + exp(-0.5*sum));
}

void testuj(Adaline *ada, Tester* t)							// funkcja testuj¹ca
{
	printf("\n\n**TESTOWANIE**");
	double tmp = 0;
	for (int i = 0; i<N_TEST; i++) {
		for (int j = 0; j<N_BYTES; j++) {
			tmp += t->input_vector[i].byte_series[j] * ada->weight_vector[j];		// liczenie sumy dla jednej litery {wektor wejsciowy bajtow}x{wagi im odpowiadajace}
		}
		printf("\nlitera %c:%d\n", t->input_vector[i].letter, (tmp >= 1.0) ? 1 : 0);			// 1 - litera duza, 0 - litera mala
		tmp = 0;										// wyzerowanie sumy, przejscie do nastepnej litery
	}
}