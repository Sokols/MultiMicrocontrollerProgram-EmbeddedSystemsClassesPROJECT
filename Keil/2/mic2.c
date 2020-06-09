#include <REGX52.H>
#include <string.h>

#define TRUE 1
#define SERIAL_MODE P3_4	// 0 odbiór, 1 nadawanie
#define MAX_BUF_SIZE 20		// max rozmiar bufora
#define X 200							// parametry dla funkcji delay
#define Y 200
#define POWER_LEFT P2_0
#define POWER_RIGHT P2_1

/* -----------------ZMIENNE/STALE KOMUNIKACJI----------------- */

// Komendy nadajace
unsigned char code B_COMMAND1[] = "";

// Komendy odbierane
unsigned char code R_COMMAND1[] = "<2_display12>";

// Bufor odbioru
unsigned char data rcvBuf[MAX_BUF_SIZE] = {0};	

// Licznik dla RcvBuf
unsigned char data rcvIndex;

/* -----------------ZMIENNE/STALE POZOSTALE----------------- */

// Tablica buforow - dla obu wyswietlaczy
unsigned char data buf[2];

// Tablica cyfr 0 - 9
unsigned char code NUMBERS[] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF};

unsigned char xdata right _at_ 0xFD00;
unsigned char xdata left _at_ 0xFE00;
	
/* -----------------FUNKCJE POMOCNICZE----------------- */

extern void executeCommand();
extern void clearRcvBuf();
extern void delay();
extern void send(unsigned char);
extern void Init();

/* -----------------PRZERWANIA----------------- */

void ISR_INT0(void) interrupt 0
  {
		rcvIndex++;
  }

// Przerwanie z portu szeregowego - odczyt danych
void ISR_SERIAL(void) interrupt 4 { 
	
	if (RI == 1) {
		/*
		if (SBUF == '<') {
			clearRcvBuf();
		}
		if (rcvIndex > -1 && rcvIndex < MAX_BUF_SIZE) {
			rcvBuf[rcvIndex++] = SBUF;
			if (SBUF == '>') {
				executeCommand();
			}
		}
	*/
	}
	RI = 0;
}	

/* -----------------PROGRAM GLOWNY----------------- */

void main(void) {

	Init();
	
	while(TRUE) {
    left = NUMBERS[buf[rcvIndex]];
		right = NUMBERS[buf[1]];
	}
}

/* -----------------CIALA FUNKCJI POMOCNICZYCH----------------- */

// Funkcja sprawdzajaca ktora komenda zostala odebrana
void executeCommand(void) {
	
	unsigned char i = 0;
	unsigned char bufLength = 0;
	unsigned char correctFlag = 1;
	
	// R_COMMAND1
	bufLength = strlen(rcvBuf);
	while (i < bufLength && correctFlag == 1) {
		if (i == (bufLength - 3)) {
			buf[0] = rcvBuf[i];
		} else if (i == (bufLength - 2)) {
			buf[1] = rcvBuf[i];
		} else if (R_COMMAND1[i] == rcvBuf[i]) {
			// do nothing
		} else {
			correctFlag = 0;
		}
		i++;
	}
	buf[0] = 1 % 5;
	buf[1] = 1 % 10;
}

// Funkcja czysci zawartosc bufora odbiorcy
void clearRcvBuf(void) {
	rcvIndex = 0; 
	for (rcvIndex; rcvIndex < sizeof(rcvBuf); rcvIndex++) {
		rcvBuf[rcvIndex] = '0';
	}
	rcvIndex = 0;
}
		
// Funkcja opozniajaca odczytywanie wcisnietych wartosci
void delay(void) {
	int x = 0, y = 0;
	for (x; x < X; x++) {
		for (y; y < Y; y++) {;}
	}
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
	T2CON = 0x30; 						// konfiguracja Timera 2 
	RCAP2L = TL2 = 0xDC;  		// konfiguracja predkosci nadawania i odbioru 
	RCAP2H = TH2 = 0xFF; 			// danych przez port szeregowy
	TR2 = 1; 									// timer 2 => ON taktowanie portu szeregowego
	RI = TI = 0;							// gaszenie flag odbiornika i nadajnika
	ES  = 1;									// wlacz przerwania z portu szeregowego
	EX0 = 1;  								// zalacz przerwanie od INT0
  IT0 = 1;  								// aktywnosc na zbocze opadajace H --> L
	EA  = 1; 									// wlacz maske przerwan
	
	// USTAWIENIA POZOSTALE
	buf[0] = 1;
	buf[1] = 2;
}