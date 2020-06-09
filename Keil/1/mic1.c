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

// Tablica sprawdzen napiec na liniach klawiatury
unsigned char code CHECK_LINE[4] = {0xEF, 0xDF, 0xBF, 0x7F};

// Tablica numerow
unsigned char code NUMBERS[4][3] = {
		0x10, 0x20, 0x30, 
		0x40, 0x50, 0x60,
		0x70, 0x80, 0x90,
		0xA0, 0x00, 0xC0};
		
// Tablica buforow - dla obu wyswietlaczy
unsigned char data buf[2];

// Licznik dla CHECK_LINE
unsigned char data tabIndex;

// Flaga dla diody
unsigned char data diodeFlag;
		
/* -----------------FUNKCJE POMOCNICZE----------------- */
		
extern void delay();
extern void setDiode();
extern void handlePressing();
extern void send(unsigned char);
extern void updateDisplay();
extern void Init();

/* -----------------PRZERWANIA----------------- */

// Przerwanie z portu szeregowego - odczyt danych
void ISR_SERIAL(void) interrupt 4 { 
	// program nie obsluguje zadnych komend
}	

/* -----------------PROGRAM GLOWNY----------------- */

void main(void) {

	Init();
	
	while(TRUE) {
		P2 = CHECK_LINE[tabIndex];
		
		if (P2 != CHECK_LINE[tabIndex]) {
			setDiode();
			handlePressing();
			updateDisplay();
		}
		if (++tabIndex == 4) { tabIndex = 0; }
		delay();
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

// Funkcja gaszaca/zapalajaca diode
void setDiode(void) {
	diodeFlag = !diodeFlag;
	P0_0 = diodeFlag;
}

// Funkcja aktualizujaca bufory po wykryciu wcisniecia przycisku
void handlePressing(void) {
	
	// Klawisz * - nic nie rob
	if (P2_3 == 0 && P2_7 == 0) {	;; }
	
	// Klawisz # - wyzeruj bufory
	else if (P2_1 == 0 && P2_7 == 0) {
		buf[0] = 0x00;
		buf[1] = 0x00;
	} 
	
	// Klawisze cyfr - przypisz do buforow
	else {
		buf[1] = buf[0];				
		// przypisanie cyfry z 1. kolumny
		if (P2_3 == 0) { buf[0] = NUMBERS[tabIndex][0]; }
		// przypisanie cyfry z 2. kolumny
		else if (P2_2 == 0) { buf[0] = NUMBERS[tabIndex][1]; }
		// przypisanie cyfry z 3. kolumny
		else { buf[0] = NUMBERS[tabIndex][2]; }
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

// Funkcja wysylajaca aktualne bufory do 2. mikrokontrolera
void updateDisplay(void) {
	unsigned char i = 0;
	unsigned char commandLength = strlen(B_COMMAND1);
	for (i; i < commandLength; i++) {
		if (i == (commandLength - 3)) {
			send(buf[0]);
		} else if (i == (commandLength - 2)) {
			send(buf[1]);
		} else {
			send(B_COMMAND1[i]);
		}
	}
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
	buf[0] = 0x00;
	buf[1] = 0x00;
	diodeFlag = 1;
	tabIndex = 0;
}