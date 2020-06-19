#include <REGX52.H>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define SERIAL_MODE P3_4	// 0 odbiór, 1 nadawanie
#define MAX_BUF_SIZE 20		// max rozmiar bufora
#define X 5000							// parametry dla funkcji delay
#define Y 5000

/* -----------------ZMIENNE/STALE KOMUNIKACJI----------------- */

// Komendy nadajace
unsigned char code S_COMMAND1[] = "";

// Komendy odbierane
unsigned char code R_COMMAND1[] = "<3_display12>";

// Bufor odbioru
unsigned char data rcvBuf[MAX_BUF_SIZE] = {0};	

// Licznik dla RcvBuf
unsigned char data rcvIndex;

/* -----------------ZMIENNE/STALE POZOSTALE----------------- */

unsigned char code less[] =					  "     Mniej!     ";
unsigned char code more[] =						"     Wiecej!    ";
unsigned char code correct[] =				"     Brawo!     ";
unsigned char code blank[] = 					"                ";

unsigned char data Var1, Var2, Var3;
unsigned char data drawCounter;
unsigned char data target;
unsigned char data type;
		
/* -----------------FUNKCJE POMOCNICZE----------------- */
		
void executeCommand();
void checkNewType();
void updateDisplay(unsigned char, unsigned char);
void clearRcvBuf();
unsigned char generateTarget();
void delay();
void send(unsigned char);
void Init();

extern void LcdInit();
extern void LcdWelcome();
extern void Lcd_DisplayRow(char, char*);

/* -----------------PRZERWANIA----------------- */

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
	
	LcdInit();
	Init();
	LcdWelcome();
	
	while(TRUE) {
		;
	}
}

/* -----------------CIALA FUNKCJI POMOCNICZYCH----------------- */

// Funkcja sprawdzajaca ktora komenda zostala odebrana
void executeCommand(void) {
	
	// R_COMMAND1
	if ((!strncmp(rcvBuf, R_COMMAND1, 10)) && rcvBuf[12] == '>') {
		if (rcvBuf[10] == 0x0A) {
			type = 0x00;
		} else {
			// dodaj dziesiatki
			type = rcvBuf[10] * 10;
		}
			
		if (rcvBuf[11] == 0x0A) {
			// do nothing - nie dodawaj 0
		} else {
			// dodaj jednosci
			type += rcvBuf[11];
		}
		
		checkNewType();
	}
}

// Funkcja sprawdza nowy typ uzytkownika
void checkNewType(void) {
	if (type > target) {
		Lcd_DisplayRow(4, less);
		drawCounter++;
		updateDisplay(drawCounter, type);
	} else if (type < target) {
		Lcd_DisplayRow(4, more);
		drawCounter++;
		updateDisplay(drawCounter, type);
	} else {
		Lcd_DisplayRow(4, correct);
		drawCounter++;
		updateDisplay(drawCounter, type);
		delay();
		drawCounter = 0;
		target = generateTarget();
		type = 0;
		updateDisplay(drawCounter, type);
		Lcd_DisplayRow(4, blank);
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

// Funkcja zwracajaca nowa wylosowana liczbe
unsigned char generateTarget(void) {
	return rand() % 100;
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
	RCAP2L = TL2 = 0xDC;  		// konfiguracja predkosci nadawania i odbioru 
	RCAP2H = TH2 = 0xFF; 			// danych przez port szeregowy
	T2CON = 0x30; 						// konfiguracja Timera 2 
	RI = TI = 0;							// gaszenie flag odbiornika i nadajnika
	ES  = 1;									// wlacz przerwania z portu szeregowego
	EA  = 1; 									// wlacz maske przerwan
	TR2 = 1; 									// timer 2 => ON taktowanie portu szeregowego
	
	// USTAWIENIA POZOSTALE
	drawCounter = 0;
	target = generateTarget();
	type = 0;
}