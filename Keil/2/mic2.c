#include <REGX52.H>
#include <string.h>

#define TRUE 1
#define SERIAL_MODE P3_4	// 0 odbiór, 1 nadawanie
#define MAX_BUF_SIZE 20		// max rozmiar bufora
#define POWER_LEFT P2_0
#define POWER_RIGHT P2_1

/* -----------------ZMIENNE/STALE KOMUNIKACJI----------------- */

// Komendy nadajace
unsigned char code S_COMMAND1[] = "<3_display&*>";

// Komendy odbierane
unsigned char code R_COMMAND1[] = "<2_display12>";

// Bufor odbioru
unsigned char data rcvBuf[MAX_BUF_SIZE] = {0};	
unsigned char data sndBuf[MAX_BUF_SIZE] = {0};

// Licznik dla RcvBuf
unsigned char data rcvIndex;

/* -----------------ZMIENNE/STALE POZOSTALE----------------- */

// Tablica cyfr 0 - 9
unsigned char code NUMBERS[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};


// Zmienne dla wyswietlanych wartosci
unsigned char data counterD;
unsigned char data counterJ;
	
/* -----------------FUNKCJE POMOCNICZE----------------- */

void executeCommand();
void clearRcvBuf();
void display();
void send(unsigned char);
void Init();

/* -----------------PRZERWANIA----------------- */

void ISR_INT0(void) interrupt 0 {
	unsigned char i = 0;
	unsigned char commandLength = strlen(S_COMMAND1);
	for (i; i < commandLength; i++) {
		if (S_COMMAND1[i] == '&') {
			sndBuf[i] = counterD;
		} else if (S_COMMAND1[i] == '*') {
			sndBuf[i] = counterJ;
		} else {
			sndBuf[i] = S_COMMAND1[i];
		}
	}
	i = 0;
	for (i; i < commandLength; i++) {
		send(sndBuf[i]);
		sndBuf[i] = 0;
	}
}

// Przerwanie z portu szeregowego - odczyt danych
void ISR_SERIAL(void) interrupt 4 { 
	if (RI == 1) {
		if (SBUF == '<') {
			clearRcvBuf();
		}
		if (rcvIndex > -1 && rcvIndex < MAX_BUF_SIZE) {
			rcvBuf[rcvIndex++] = SBUF;
			if (SBUF == '>') {
				executeCommand();
			}
		}
	}
	RI = 0;
}	


/* -----------------PROGRAM GLOWNY----------------- */

void main(void) {

	Init();
	
	while(TRUE) {
		display();
	}
}

/* -----------------CIALA FUNKCJI POMOCNICZYCH----------------- */

// Funkcja sprawdzajaca ktora komenda zostala odebrana
void executeCommand(void) {
	
	// R_COMMAND1
	if ((!strncmp(rcvBuf, R_COMMAND1, 10)) && rcvBuf[12] == '>') {
		if (rcvBuf[10] == 0x0A) {
			counterJ = 0x00;
		} else {
			counterJ = rcvBuf[10];
		}
			
		if (rcvBuf[11] == 0x0A) {
			counterD = 0x00;
		} else {
			counterD = rcvBuf[11];
		}
	}
}

// Funkcja czysci zawartosc bufora odbiorcy
void clearRcvBuf(void) {
	rcvIndex = 0; 
	for (rcvIndex; rcvIndex < sizeof(rcvBuf); rcvIndex++) {
		rcvBuf[rcvIndex] = 0;
	}
	rcvIndex = 0;
}

// Funkcja wyswietlajaca zawartosc buforow na wyswietlaczu
void display(void) {
	unsigned char i = 0;
	for(i; i < 200; i++) {	
		POWER_LEFT = 1;
		POWER_RIGHT = 1;
		P0 = NUMBERS[counterD];
		POWER_LEFT = 0;
		POWER_LEFT = 1;	
		P0 = NUMBERS[counterJ];
		POWER_RIGHT = 0;
		POWER_RIGHT = 1;
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
	counterD = 0x00;
	counterJ = 0x00;
	POWER_LEFT = 0;
	POWER_RIGHT = 1;
}