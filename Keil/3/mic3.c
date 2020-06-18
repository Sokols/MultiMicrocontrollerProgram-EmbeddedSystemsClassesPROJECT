#include <REGX52.H>
#include <string.h>

#define TRUE 1
#define SERIAL_MODE P3_4	// 0 odbiór, 1 nadawanie
#define MAX_BUF_SIZE 20		// max rozmiar bufora
#define X 500							// parametry dla funkcji delay
#define Y 500

/* -----------------ZMIENNE/STALE KOMUNIKACJI----------------- */

// Komendy nadajace
unsigned char code B_COMMAND1[] = "<2_display12>";

// Komendy odbierane
unsigned char code R_COMMAND1[] = "";

// Bufor odbioru
unsigned char data RcvBuf[MAX_BUF_SIZE] = {0};	

// Licznik dla RcvBuf
unsigned char data rcvIndex;

/* -----------------ZMIENNE/STALE POZOSTALE----------------- */


		
/* -----------------FUNKCJE POMOCNICZE----------------- */
		
extern void delay();
extern void send(unsigned char);
extern void Init();

/* -----------------PRZERWANIA----------------- */

// Przerwanie z portu szeregowego - odczyt danych
void ISR_SERIAL(void) interrupt 4 { 
	if (RI == 1) { 	
		P1_0 = 0; 	    
  }
	RI = 0;
}	

/* -----------------PROGRAM GLOWNY----------------- */

void main(void) {

	Init();
	
	while(TRUE) {
		;
	}
}

/* -----------------CIALA FUNKCJI POMOCNICZYCH----------------- */
		
// Funkcja opozniajaca odczytywanie wcisnietych wartosci
void delay(void) {
	int x = 0, y = 0;
	for (x; x < X; x++) {
		for (y; y < Y; y++) {;}
	}
}


// Funkcja aktualizujaca bufory po wykryciu wcisniecia przycisku
void handlePressing(void) {

}

// Funkcja wysylajaca dany znak przez magistrale
void send(unsigned char value) {
	SERIAL_MODE = 1; 							// tryb nadawania
	ES = 0;												// wylaczenie nasluchu odbierania danych
	TI = 0;												
	SBUF = value;  								// zaladuj znak do bufora
	while(TI == 0); 
	TI = 0;
	ES = 1;												// ponowne wlaczenie nasluchu odbierania danych
	SERIAL_MODE = 0;					    // tryb odbioru
}


// Funkcja inicjujaca
void Init(void) {
	
	// USTAWIENIA OGOLNE
	SERIAL_MODE = 0;					// przejdz w tryb odbioru
	rcvIndex = 0;							// licznik bufora odbioru na 0
	SCON = 0x50; 							// konfiguracja portu szeregowego
	RCAP2L = TL2 = 0xDC;  		// konfiguracja predkosci nadawania i odbioru 
	RCAP2H = TH2 = 0xFF; 			// danych przez port szeregowy
	T2CON = 0x30; 						// konfiguracja Timera 2 
	RI = TI = 0;							// gaszenie flag odbiornika i nadajnika
	ES  = 1;									// wlacz przerwania z portu szeregowego
	EA  = 1; 									// wlacz maske przerwan
	TR2 = 1; 									// timer 2 => ON taktowanie portu szeregowego
	
	// USTAWIENIA POZOSTALE

}