#include <REGX52.H>
unsigned char code DecTo7Seg[] = {0x3F,0x06,0x5B,0x4F,0x66,
                                  0x6D,0x7D,0x07,0x7F,0x6F};
unsigned char xdata Jedn  _at_ 0xFD00;
unsigned char xdata Dzies _at_ 0xFE00;
volatile unsigned char  data Licznik = 0;

void ISR_INT0(void) interrupt 0
  {
    Licznik++;
    if (Licznik == 10) {Licznik = 0;}
  }

void ISR_Serial(void) interrupt 4
  {
    if (RI == 1)  // przysz³a ramka
      {
        Licznik = SBUF;
        Licznik = Licznik - '0';
        RI = 0;
      }
  }

void SerialInit(void)
  {
    SCON  = 0x50; // REN  = 1
    T2CON = 0x38; // TCLK = 1
                  // RCLK = 1
                  // TR2  = 0
                  // EXEN2= 1
    RCAP2L = TL2 = 0xDC;
    RCAP2H = TH2 = 0xFF;     
    TR2 = 1; 
  }

void Init(void)
  {
    ES  = 1;
    EX0 = 1;  // za³¹cz przerwanie od INT0
    IT0 = 1;  // aktywnosæ na zbocze opadaj¹ce H --> L
    EA  = 1;  // za³¹cz system przerwañ
  }

void main(void)
  {
    Init();
    SerialInit();
    for(;;)
      {
        Jedn  = DecTo7Seg[Licznik];
        Dzies = DecTo7Seg[0];
      }
  }