#include "p33EP16GS506.h"

volatile unsigned int  WREG0;
volatile unsigned int  WREG1;
volatile unsigned int  WREG2;
volatile unsigned int  WREG3;
volatile unsigned int  WREG4;
volatile unsigned int  WREG5;
volatile unsigned int  WREG6;
volatile unsigned int  WREG7;
volatile unsigned int  WREG8;
volatile unsigned int  WREG9;
volatile unsigned int  WREG10;
volatile unsigned int  WREG11;
volatile unsigned int  WREG12;
volatile unsigned int  WREG13;
volatile unsigned int  WREG14;
volatile unsigned int  WREG15;
volatile unsigned int  SPLIM;
volatile unsigned int  ACCAL;
volatile unsigned int  ACCAH;
volatile unsigned char ACCAU;
volatile unsigned int  ACCBL;
volatile unsigned int  ACCBH;
volatile unsigned char ACCBU;
volatile unsigned int  PCL;
volatile unsigned char PCH;
volatile unsigned int  DSRPAG;
volatile DSRPAGBITS DSRPAGbits;
volatile unsigned int  DSWPAG;
volatile DSWPAGBITS DSWPAGbits;
volatile unsigned int  RCOUNT;
volatile unsigned int  DCOUNT;
volatile unsigned int  DOSTARTL;
volatile unsigned int  DOSTARTH;
volatile unsigned int  DOENDL;
volatile unsigned int  DOENDH;
volatile unsigned int  SR;
volatile SRBITS SRbits;
volatile unsigned int  CORCON;
volatile CORCONBITS CORCONbits;
volatile unsigned int  MODCON;
volatile MODCONBITS MODCONbits;
volatile unsigned int  XMODSRT;
volatile unsigned int  XMODEND;
volatile unsigned int  YMODSRT;
volatile unsigned int  YMODEND;
volatile unsigned int  XBREV;
volatile XBREVBITS XBREVbits;
volatile unsigned int  DISICNT;
volatile unsigned int  TBLPAG;
volatile TBLPAGBITS TBLPAGbits;
volatile unsigned int  CTXTSTAT;
volatile CTXTSTATBITS CTXTSTATbits;
volatile unsigned int  TMR1;
volatile unsigned int  PR1;
volatile unsigned int  T1CON;
volatile T1CONBITS T1CONbits;
volatile unsigned int  TMR2;
volatile unsigned int  TMR3HLD;
volatile unsigned int  TMR3;
volatile unsigned int  PR2;
volatile unsigned int  PR3;
volatile unsigned int  T2CON;
volatile T2CONBITS T2CONbits;
volatile unsigned int  T3CON;
volatile T3CONBITS T3CONbits;
volatile unsigned int  TMR4;
volatile unsigned int  TMR5HLD;
volatile unsigned int  TMR5;
volatile unsigned int  PR4;
volatile unsigned int  PR5;
volatile unsigned int  T4CON;
volatile T4CONBITS T4CONbits;
volatile unsigned int  T5CON;
volatile T5CONBITS T5CONbits;
volatile unsigned int  IC1CON1;
volatile IC1CON1BITS IC1CON1bits;
volatile unsigned int  IC1CON2;
volatile IC1CON2BITS IC1CON2bits;
volatile IC IC1;
volatile IC IC2;
volatile IC IC3;
volatile IC IC4;
volatile IC IC5;
volatile IC IC6;
volatile IC IC7;
volatile IC IC8;
volatile unsigned int  IC1BUF;
volatile unsigned int  IC1TMR;
volatile unsigned int  IC2CON1;
volatile IC2CON1BITS IC2CON1bits;
volatile unsigned int  IC2CON2;
volatile IC2CON2BITS IC2CON2bits;
volatile unsigned int  IC2BUF;
volatile unsigned int  IC2TMR;
volatile unsigned int  IC3CON1;
volatile IC3CON1BITS IC3CON1bits;
volatile unsigned int  IC3CON2;
volatile IC3CON2BITS IC3CON2bits;
volatile unsigned int  IC3BUF;
volatile unsigned int  IC3TMR;
volatile unsigned int  IC4CON1;
volatile IC4CON1BITS IC4CON1bits;
volatile unsigned int  IC4CON2;
volatile IC4CON2BITS IC4CON2bits;
volatile unsigned int  IC4BUF;
volatile unsigned int  IC4TMR;
volatile unsigned int  I2C1CON1;
volatile I2C1CON1BITS I2C1CON1bits;
volatile unsigned int  I2C1CONL;
volatile I2C1CONLBITS I2C1CONLbits;
volatile unsigned int  I2C1CON2;
volatile I2C1CON2BITS I2C1CON2bits;
volatile unsigned int  I2C1CONH;
volatile I2C1CONHBITS I2C1CONHbits;
volatile unsigned int  I2C1STAT;
volatile I2C1STATBITS I2C1STATbits;
volatile unsigned int  I2C1ADD;
volatile I2C1ADDBITS I2C1ADDbits;
volatile unsigned int  I2C1MSK;
volatile I2C1MSKBITS I2C1MSKbits;
volatile unsigned int  I2C1BRG;
volatile unsigned int  I2C1TRN;
volatile I2C1TRNBITS I2C1TRNbits;
volatile unsigned int  I2C1RCV;
volatile I2C1RCVBITS I2C1RCVbits;
volatile unsigned int  I2C2CON1;
volatile I2C2CON1BITS I2C2CON1bits;
volatile unsigned int  I2C2CONL;
volatile I2C2CONLBITS I2C2CONLbits;
volatile unsigned int  I2C2CON2;
volatile I2C2CON2BITS I2C2CON2bits;
volatile unsigned int  I2C2CONH;
volatile I2C2CONHBITS I2C2CONHbits;
volatile unsigned int  I2C2STAT;
volatile I2C2STATBITS I2C2STATbits;
volatile unsigned int  I2C2ADD;
volatile I2C2ADDBITS I2C2ADDbits;
volatile unsigned int  I2C2MSK;
volatile I2C2MSKBITS I2C2MSKbits;
volatile unsigned int  I2C2BRG;
volatile unsigned int  I2C2TRN;
volatile I2C2TRNBITS I2C2TRNbits;
volatile unsigned int  I2C2RCV;
volatile I2C2RCVBITS I2C2RCVbits;
volatile UART UART1;
volatile UART UART2;
volatile unsigned int  U1MODE;
volatile U1MODEBITS U1MODEbits;
volatile unsigned int  U1STA;
volatile U1STABITS U1STAbits;
volatile unsigned int  U1TXREG;
volatile unsigned int  U1RXREG;
volatile unsigned int  U1BRG;
volatile unsigned int  U2MODE;
volatile U2MODEBITS U2MODEbits;
volatile unsigned int  U2STA;
volatile U2STABITS U2STAbits;
volatile unsigned int  U2TXREG;
volatile unsigned int  U2RXREG;
volatile unsigned int  U2BRG;
volatile SPI SPI1;
volatile SPI SPI2;
volatile unsigned int  SPI1STAT;
volatile SPI1STATBITS SPI1STATbits;
volatile unsigned int  SPI1CON1;
volatile SPI1CON1BITS SPI1CON1bits;
volatile unsigned int  SPI1CON2;
volatile SPI1CON2BITS SPI1CON2bits;
volatile unsigned int  SPI1BUF;
volatile unsigned int  SPI2STAT;
volatile SPI2STATBITS SPI2STATbits;
volatile unsigned int  SPI2CON1;
volatile SPI2CON1BITS SPI2CON1bits;
volatile unsigned int  SPI2CON2;
volatile SPI2CON2BITS SPI2CON2bits;
volatile unsigned int  SPI2BUF;
volatile unsigned int  ADCON1L;
volatile ADCON1LBITS ADCON1Lbits;
volatile unsigned int  ADCON1H;
volatile ADCON1HBITS ADCON1Hbits;
volatile unsigned int  ADCON2L;
volatile ADCON2LBITS ADCON2Lbits;
volatile unsigned int  ADCON2H;
volatile ADCON2HBITS ADCON2Hbits;
volatile unsigned int  ADCON3L;
volatile ADCON3LBITS ADCON3Lbits;
volatile unsigned int  ADCON3H;
volatile ADCON3HBITS ADCON3Hbits;
volatile unsigned int  ADCON4L;
volatile ADCON4LBITS ADCON4Lbits;
volatile unsigned int  ADCON4H;
volatile ADCON4HBITS ADCON4Hbits;
volatile unsigned int  ADMOD0L;
volatile ADMOD0LBITS ADMOD0Lbits;
volatile unsigned int  ADMOD0H;
volatile ADMOD0HBITS ADMOD0Hbits;
volatile unsigned int  ADMOD1L;
volatile ADMOD1LBITS ADMOD1Lbits;
volatile unsigned int  ADIEL;
volatile ADIELBITS ADIELbits;
volatile unsigned int  ADIEH;
volatile ADIEHBITS ADIEHbits;
volatile unsigned int  ADSTATL;
volatile ADSTATLBITS ADSTATLbits;
volatile unsigned int  ADSTATH;
volatile ADSTATHBITS ADSTATHbits;
volatile unsigned int  ADCMP0ENL;
volatile ADCMP0ENLBITS ADCMP0ENLbits;
volatile unsigned int  ADCMP0ENH;
volatile ADCMP0ENHBITS ADCMP0ENHbits;
volatile unsigned int  ADCMP0LO;
volatile unsigned int  ADCMP0HI;
volatile unsigned int  ADCMP1ENL;
volatile ADCMP1ENLBITS ADCMP1ENLbits;
volatile unsigned int  ADCMP1ENH;
volatile ADCMP1ENHBITS ADCMP1ENHbits;
volatile unsigned int  ADCMP1LO;
volatile unsigned int  ADCMP1HI;
volatile unsigned int  ADFL0DAT;
volatile unsigned int  ADFL0CON;
volatile ADFL0CONBITS ADFL0CONbits;
volatile unsigned int  ADFL1DAT;
volatile unsigned int  ADFL1CON;
volatile ADFL1CONBITS ADFL1CONbits;
volatile unsigned int  ADTRIG0L;
volatile ADTRIG0LBITS ADTRIG0Lbits;
volatile unsigned int  ADTRIG0H;
volatile ADTRIG0HBITS ADTRIG0Hbits;
volatile unsigned int  ADTRIG1L;
volatile ADTRIG1LBITS ADTRIG1Lbits;
volatile unsigned int  ADTRIG1H;
volatile ADTRIG1HBITS ADTRIG1Hbits;
volatile unsigned int  ADTRIG2L;
volatile ADTRIG2LBITS ADTRIG2Lbits;
volatile unsigned int  ADTRIG2H;
volatile ADTRIG2HBITS ADTRIG2Hbits;
volatile unsigned int  ADTRIG3L;
volatile ADTRIG3LBITS ADTRIG3Lbits;
volatile unsigned int  ADTRIG3H;
volatile ADTRIG3HBITS ADTRIG3Hbits;
volatile unsigned int  ADTRIG4L;
volatile ADTRIG4LBITS ADTRIG4Lbits;
volatile unsigned int  ADTRIG4H;
volatile ADTRIG4HBITS ADTRIG4Hbits;
volatile unsigned int  ADTRIG5L;
volatile ADTRIG5LBITS ADTRIG5Lbits;
volatile unsigned int  ADCMP0CON;
volatile ADCMP0CONBITS ADCMP0CONbits;
volatile unsigned int  ADCMP1CON;
volatile ADCMP1CONBITS ADCMP1CONbits;
volatile unsigned int  ADLVLTRGL;
volatile ADLVLTRGLBITS ADLVLTRGLbits;
volatile unsigned int  ADLVLTRGH;
volatile ADLVLTRGHBITS ADLVLTRGHbits;
volatile unsigned int  ADCORE0L;
volatile ADCORE0LBITS ADCORE0Lbits;
volatile unsigned int  ADCORE0H;
volatile ADCORE0HBITS ADCORE0Hbits;
volatile unsigned int  ADCORE1L;
volatile ADCORE1LBITS ADCORE1Lbits;
volatile unsigned int  ADCORE1H;
volatile ADCORE1HBITS ADCORE1Hbits;
volatile unsigned int  ADCORE2L;
volatile ADCORE2LBITS ADCORE2Lbits;
volatile unsigned int  ADCORE2H;
volatile ADCORE2HBITS ADCORE2Hbits;
volatile unsigned int  ADCORE3L;
volatile ADCORE3LBITS ADCORE3Lbits;
volatile unsigned int  ADCORE3H;
volatile ADCORE3HBITS ADCORE3Hbits;
volatile unsigned int  ADEIEL;
volatile ADEIELBITS ADEIELbits;
volatile unsigned int  ADEIEH;
volatile ADEIEHBITS ADEIEHbits;
volatile unsigned int  ADEISTATL;
volatile ADEISTATLBITS ADEISTATLbits;
volatile unsigned int  ADEISTATH;
volatile ADEISTATHBITS ADEISTATHbits;
volatile unsigned int  ADCON5L;
volatile ADCON5LBITS ADCON5Lbits;
volatile unsigned int  ADCON5H;
volatile ADCON5HBITS ADCON5Hbits;
volatile unsigned int  ADCAL0L;
volatile ADCAL0LBITS ADCAL0Lbits;
volatile unsigned int  ADCAL0H;
volatile ADCAL0HBITS ADCAL0Hbits;
volatile unsigned int  ADCAL1H;
volatile ADCAL1HBITS ADCAL1Hbits;
volatile unsigned int  ADCBUF0;
volatile unsigned int  ADCBUF1;
volatile unsigned int  ADCBUF2;
volatile unsigned int  ADCBUF3;
volatile unsigned int  ADCBUF4;
volatile unsigned int  ADCBUF5;
volatile unsigned int  ADCBUF6;
volatile unsigned int  ADCBUF7;
volatile unsigned int  ADCBUF8;
volatile unsigned int  ADCBUF9;
volatile unsigned int  ADCBUF10;
volatile unsigned int  ADCBUF11;
volatile unsigned int  ADCBUF12;
volatile unsigned int  ADCBUF13;
volatile unsigned int  ADCBUF14;
volatile unsigned int  ADCBUF15;
volatile unsigned int  ADCBUF16;
volatile unsigned int  ADCBUF17;
volatile unsigned int  ADCBUF18;
volatile unsigned int  ADCBUF19;
volatile unsigned int  ADCBUF20;
volatile unsigned int  ADCBUF21;
volatile unsigned int  ISRCCON;
volatile ISRCCONBITS ISRCCONbits;
volatile unsigned int  PGA1CON;
volatile PGA1CONBITS PGA1CONbits;
volatile unsigned int  PGA1CAL;
volatile PGA1CALBITS PGA1CALbits;
volatile unsigned int  PGA2CON;
volatile PGA2CONBITS PGA2CONbits;
volatile unsigned int  PGA2CAL;
volatile PGA2CALBITS PGA2CALbits;
volatile unsigned int  CMP1CON;
volatile CMP1CONBITS CMP1CONbits;
volatile unsigned int  CMP1DAC;
volatile CMP1DACBITS CMP1DACbits;
volatile unsigned int  CMP2CON;
volatile CMP2CONBITS CMP2CONbits;
volatile unsigned int  CMP2DAC;
volatile CMP2DACBITS CMP2DACbits;
volatile unsigned int  CMP3CON;
volatile CMP3CONBITS CMP3CONbits;
volatile unsigned int  CMP3DAC;
volatile CMP3DACBITS CMP3DACbits;
volatile unsigned int  CMP4CON;
volatile CMP4CONBITS CMP4CONbits;
volatile unsigned int  CMP4DAC;
volatile CMP4DACBITS CMP4DACbits;
volatile unsigned int  RPOR0;
volatile RPOR0BITS RPOR0bits;
volatile unsigned int  RPOR1;
volatile RPOR1BITS RPOR1bits;
volatile unsigned int  RPOR2;
volatile RPOR2BITS RPOR2bits;
volatile unsigned int  RPOR3;
volatile RPOR3BITS RPOR3bits;
volatile unsigned int  RPOR4;
volatile RPOR4BITS RPOR4bits;
volatile unsigned int  RPOR5;
volatile RPOR5BITS RPOR5bits;
volatile unsigned int  RPOR6;
volatile RPOR6BITS RPOR6bits;
volatile unsigned int  RPOR7;
volatile RPOR7BITS RPOR7bits;
volatile unsigned int  RPOR8;
volatile RPOR8BITS RPOR8bits;
volatile unsigned int  RPOR9;
volatile RPOR9BITS RPOR9bits;
volatile unsigned int  RPOR10;
volatile RPOR10BITS RPOR10bits;
volatile unsigned int  RPOR11;
volatile RPOR11BITS RPOR11bits;
volatile unsigned int  RPOR12;
volatile RPOR12BITS RPOR12bits;
volatile unsigned int  RPOR13;
volatile RPOR13BITS RPOR13bits;
volatile unsigned int  RPOR14;
volatile RPOR14BITS RPOR14bits;
volatile unsigned int  RPOR15;
volatile RPOR15BITS RPOR15bits;
volatile unsigned int  RPOR16;
volatile RPOR16BITS RPOR16bits;
volatile unsigned int  RPOR17;
volatile RPOR17BITS RPOR17bits;
volatile unsigned int  RPOR18;
volatile RPOR18BITS RPOR18bits;
volatile unsigned int  RPINR0;
volatile RPINR0BITS RPINR0bits;
volatile unsigned int  RPINR1;
volatile RPINR1BITS RPINR1bits;
volatile unsigned int  RPINR2;
volatile RPINR2BITS RPINR2bits;
volatile unsigned int  RPINR3;
volatile RPINR3BITS RPINR3bits;
volatile unsigned int  RPINR7;
volatile RPINR7BITS RPINR7bits;
volatile unsigned int  RPINR8;
volatile RPINR8BITS RPINR8bits;
volatile unsigned int  RPINR11;
volatile RPINR11BITS RPINR11bits;
volatile unsigned int  RPINR12;
volatile RPINR12BITS RPINR12bits;
volatile unsigned int  RPINR13;
volatile RPINR13BITS RPINR13bits;
volatile unsigned int  RPINR18;
volatile RPINR18BITS RPINR18bits;
volatile unsigned int  RPINR19;
volatile RPINR19BITS RPINR19bits;
volatile unsigned int  RPINR20;
volatile RPINR20BITS RPINR20bits;
volatile unsigned int  RPINR21;
volatile RPINR21BITS RPINR21bits;
volatile unsigned int  RPINR22;
volatile RPINR22BITS RPINR22bits;
volatile unsigned int  RPINR23;
volatile RPINR23BITS RPINR23bits;
volatile unsigned int  RPINR37;
volatile RPINR37BITS RPINR37bits;
volatile unsigned int  RPINR38;
volatile RPINR38BITS RPINR38bits;
volatile unsigned int  RPINR42;
volatile RPINR42BITS RPINR42bits;
volatile unsigned int  RPINR43;
volatile RPINR43BITS RPINR43bits;
volatile unsigned int  NVMCON;
volatile NVMCONBITS NVMCONbits;
volatile unsigned int  NVMADR;
volatile NVMADRBITS NVMADRbits;
volatile unsigned int  NVMADRL;
volatile NVMADRLBITS NVMADRLbits;
volatile unsigned int  NVMADRH;
volatile NVMADRHBITS NVMADRHbits;
volatile unsigned int  NVMADRU;
volatile NVMADRUBITS NVMADRUbits;
volatile unsigned int  NVMKEY;
volatile unsigned int  NVMSRCADRL;
volatile NVMSRCADRLBITS NVMSRCADRLbits;
volatile unsigned int  NVMSRCADRH;
volatile NVMSRCADRHBITS NVMSRCADRHbits;
volatile unsigned int  RCON;
volatile RCONBITS RCONbits;
volatile unsigned int  OSCCON;
volatile OSCCONBITS OSCCONbits;
volatile unsigned char OSCCONL;
volatile unsigned char OSCCONH;
volatile unsigned int  CLKDIV;
volatile CLKDIVBITS CLKDIVbits;
volatile unsigned int  PLLFBD;
volatile PLLFBDBITS PLLFBDbits;
volatile unsigned int  OSCTUN;
volatile OSCTUNBITS OSCTUNbits;
volatile unsigned int  LFSR;
volatile LFSRBITS LFSRbits;
volatile unsigned int  REFOCON;
volatile REFOCONBITS REFOCONbits;
volatile unsigned int  ACLKCON;
volatile ACLKCONBITS ACLKCONbits;
volatile unsigned int  PMD1;
volatile PMD1BITS PMD1bits;
volatile unsigned int  PMD2;
volatile PMD2BITS PMD2bits;
volatile unsigned int  PMD3;
volatile PMD3BITS PMD3bits;
volatile unsigned int  PMD4;
volatile PMD4BITS PMD4bits;
volatile unsigned int  PMD6;
volatile PMD6BITS PMD6bits;
volatile unsigned int  PMD7;
volatile PMD7BITS PMD7bits;
volatile unsigned int  PMD8;
volatile PMD8BITS PMD8bits;
volatile unsigned int  IFS0;
volatile IFS0BITS IFS0bits;
volatile unsigned int  IFS1;
volatile IFS1BITS IFS1bits;
volatile unsigned int  IFS2;
volatile IFS2BITS IFS2bits;
volatile unsigned int  IFS3;
volatile IFS3BITS IFS3bits;
volatile unsigned int  IFS4;
volatile IFS4BITS IFS4bits;
volatile unsigned int  IFS5;
volatile IFS5BITS IFS5bits;
volatile unsigned int  IFS6;
volatile IFS6BITS IFS6bits;
volatile unsigned int  IFS7;
volatile IFS7BITS IFS7bits;
volatile unsigned int  IFS8;
volatile IFS8BITS IFS8bits;
volatile unsigned int  IFS9;
volatile IFS9BITS IFS9bits;
volatile unsigned int  IFS10;
volatile IFS10BITS IFS10bits;
volatile unsigned int  IFS11;
volatile IFS11BITS IFS11bits;
volatile unsigned int  IEC0;
volatile IEC0BITS IEC0bits;
volatile unsigned int  IEC1;
volatile IEC1BITS IEC1bits;
volatile unsigned int  IEC2;
volatile IEC2BITS IEC2bits;
volatile unsigned int  IEC3;
volatile IEC3BITS IEC3bits;
volatile unsigned int  IEC4;
volatile IEC4BITS IEC4bits;
volatile unsigned int  IEC5;
volatile IEC5BITS IEC5bits;
volatile unsigned int  IEC6;
volatile IEC6BITS IEC6bits;
volatile unsigned int  IEC7;
volatile IEC7BITS IEC7bits;
volatile unsigned int  IEC8;
volatile IEC8BITS IEC8bits;
volatile unsigned int  IEC9;
volatile IEC9BITS IEC9bits;
volatile unsigned int  IEC10;
volatile IEC10BITS IEC10bits;
volatile unsigned int  IEC11;
volatile IEC11BITS IEC11bits;
volatile unsigned int  IPC0;
volatile IPC0BITS IPC0bits;
volatile unsigned int  IPC1;
volatile IPC1BITS IPC1bits;
volatile unsigned int  IPC2;
volatile IPC2BITS IPC2bits;
volatile unsigned int  IPC3;
volatile IPC3BITS IPC3bits;
volatile unsigned int  IPC4;
volatile IPC4BITS IPC4bits;
volatile unsigned int  IPC5;
volatile IPC5BITS IPC5bits;
volatile unsigned int  IPC6;
volatile IPC6BITS IPC6bits;
volatile unsigned int  IPC7;
volatile IPC7BITS IPC7bits;
volatile unsigned int  IPC8;
volatile IPC8BITS IPC8bits;
volatile unsigned int  IPC9;
volatile IPC9BITS IPC9bits;
volatile unsigned int  IPC12;
volatile IPC12BITS IPC12bits;
volatile unsigned int  IPC13;
volatile IPC13BITS IPC13bits;
volatile unsigned int  IPC14;
volatile IPC14BITS IPC14bits;
volatile unsigned int  IPC16;
volatile IPC16BITS IPC16bits;
volatile unsigned int  IPC18;
volatile IPC18BITS IPC18bits;
volatile unsigned int  IPC23;
volatile IPC23BITS IPC23bits;
volatile unsigned int  IPC24;
volatile IPC24BITS IPC24bits;
volatile unsigned int  IPC25;
volatile IPC25BITS IPC25bits;
volatile unsigned int  IPC26;
volatile IPC26BITS IPC26bits;
volatile unsigned int  IPC27;
volatile IPC27BITS IPC27bits;
volatile unsigned int  IPC28;
volatile IPC28BITS IPC28bits;
volatile unsigned int  IPC29;
volatile IPC29BITS IPC29bits;
volatile unsigned int  IPC35;
volatile IPC35BITS IPC35bits;
volatile unsigned int  IPC37;
volatile IPC37BITS IPC37bits;
volatile unsigned int  IPC38;
volatile IPC38BITS IPC38bits;
volatile unsigned int  IPC39;
volatile IPC39BITS IPC39bits;
volatile unsigned int  IPC40;
volatile IPC40BITS IPC40bits;
volatile unsigned int  IPC41;
volatile IPC41BITS IPC41bits;
volatile unsigned int  IPC43;
volatile IPC43BITS IPC43bits;
volatile unsigned int  IPC44;
volatile IPC44BITS IPC44bits;
volatile unsigned int  IPC45;
volatile IPC45BITS IPC45bits;
volatile unsigned int  INTCON1;
volatile INTCON1BITS INTCON1bits;
volatile unsigned int  INTCON2;
volatile INTCON2BITS INTCON2bits;
volatile unsigned int  INTCON3;
volatile INTCON3BITS INTCON3bits;
volatile unsigned int  INTCON4;
volatile INTCON4BITS INTCON4bits;
volatile unsigned int  INTTREG;
volatile INTTREGBITS INTTREGbits;
volatile unsigned int  OC1CON1;
volatile OC1CON1BITS OC1CON1bits;
volatile unsigned int  OC1CON2;
volatile OC1CON2BITS OC1CON2bits;
volatile OC OC1;
volatile OC OC2;
volatile OC OC3;
volatile OC OC4;
volatile OC OC5;
volatile OC OC6;
volatile OC OC7;
volatile OC OC8;
volatile unsigned int  OC1RS;
volatile unsigned int  OC1R;
volatile unsigned int  OC1TMR;
volatile unsigned int  OC2CON1;
volatile OC2CON1BITS OC2CON1bits;
volatile unsigned int  OC2CON2;
volatile OC2CON2BITS OC2CON2bits;
volatile unsigned int  OC2RS;
volatile unsigned int  OC2R;
volatile unsigned int  OC2TMR;
volatile unsigned int  OC3CON1;
volatile OC3CON1BITS OC3CON1bits;
volatile unsigned int  OC3CON2;
volatile OC3CON2BITS OC3CON2bits;
volatile unsigned int  OC3RS;
volatile unsigned int  OC3R;
volatile unsigned int  OC3TMR;
volatile unsigned int  OC4CON1;
volatile OC4CON1BITS OC4CON1bits;
volatile unsigned int  OC4CON2;
volatile OC4CON2BITS OC4CON2bits;
volatile unsigned int  OC4RS;
volatile unsigned int  OC4R;
volatile unsigned int  OC4TMR;
volatile unsigned int  PTCON;
volatile PTCONBITS PTCONbits;
volatile unsigned int  PTCON2;
volatile PTCON2BITS PTCON2bits;
volatile unsigned int  PTPER;
volatile unsigned int  SEVTCMP;
volatile SEVTCMPBITS SEVTCMPbits;
volatile unsigned int  MDC;
volatile unsigned int  STCON;
volatile STCONBITS STCONbits;
volatile unsigned int  STCON2;
volatile STCON2BITS STCON2bits;
volatile unsigned int  STPER;
volatile unsigned int  SSEVTCMP;
volatile SSEVTCMPBITS SSEVTCMPbits;
volatile unsigned int  CHOP;
volatile CHOPBITS CHOPbits;
volatile unsigned int  PWMKEY;
volatile unsigned int  PWMCON1;
volatile PWMCON1BITS PWMCON1bits;
volatile unsigned int  IOCON1;
volatile IOCON1BITS IOCON1bits;
volatile unsigned int  FCLCON1;
volatile FCLCON1BITS FCLCON1bits;
volatile unsigned int  PDC1;
volatile unsigned int  PHASE1;
volatile unsigned int  DTR1;
volatile unsigned int  ALTDTR1;
volatile unsigned int  SDC1;
volatile unsigned int  SPHASE1;
volatile unsigned int  TRIG1;
volatile TRIG1BITS TRIG1bits;
volatile unsigned int  TRGCON1;
volatile TRGCON1BITS TRGCON1bits;
volatile unsigned int  STRIG1;
volatile STRIG1BITS STRIG1bits;
volatile unsigned int  PWMCAP1;
volatile PWMCAP1BITS PWMCAP1bits;
volatile unsigned int  LEBCON1;
volatile LEBCON1BITS LEBCON1bits;
volatile unsigned int  LEBDLY1;
volatile LEBDLY1BITS LEBDLY1bits;
volatile unsigned int  AUXCON1;
volatile AUXCON1BITS AUXCON1bits;
volatile unsigned int  PWMCON2;
volatile PWMCON2BITS PWMCON2bits;
volatile unsigned int  IOCON2;
volatile IOCON2BITS IOCON2bits;
volatile unsigned int  FCLCON2;
volatile FCLCON2BITS FCLCON2bits;
volatile unsigned int  PDC2;
volatile unsigned int  PHASE2;
volatile unsigned int  DTR2;
volatile unsigned int  ALTDTR2;
volatile unsigned int  SDC2;
volatile unsigned int  SPHASE2;
volatile unsigned int  TRIG2;
volatile TRIG2BITS TRIG2bits;
volatile unsigned int  TRGCON2;
volatile TRGCON2BITS TRGCON2bits;
volatile unsigned int  STRIG2;
volatile STRIG2BITS STRIG2bits;
volatile unsigned int  PWMCAP2;
volatile PWMCAP2BITS PWMCAP2bits;
volatile unsigned int  LEBCON2;
volatile LEBCON2BITS LEBCON2bits;
volatile unsigned int  LEBDLY2;
volatile LEBDLY2BITS LEBDLY2bits;
volatile unsigned int  AUXCON2;
volatile AUXCON2BITS AUXCON2bits;
volatile unsigned int  PWMCON3;
volatile PWMCON3BITS PWMCON3bits;
volatile unsigned int  IOCON3;
volatile IOCON3BITS IOCON3bits;
volatile unsigned int  FCLCON3;
volatile FCLCON3BITS FCLCON3bits;
volatile unsigned int  PDC3;
volatile unsigned int  PHASE3;
volatile unsigned int  DTR3;
volatile unsigned int  ALTDTR3;
volatile unsigned int  SDC3;
volatile unsigned int  SPHASE3;
volatile unsigned int  TRIG3;
volatile TRIG3BITS TRIG3bits;
volatile unsigned int  TRGCON3;
volatile TRGCON3BITS TRGCON3bits;
volatile unsigned int  STRIG3;
volatile STRIG3BITS STRIG3bits;
volatile unsigned int  PWMCAP3;
volatile PWMCAP3BITS PWMCAP3bits;
volatile unsigned int  LEBCON3;
volatile LEBCON3BITS LEBCON3bits;
volatile unsigned int  LEBDLY3;
volatile LEBDLY3BITS LEBDLY3bits;
volatile unsigned int  AUXCON3;
volatile AUXCON3BITS AUXCON3bits;
volatile unsigned int  PWMCON4;
volatile PWMCON4BITS PWMCON4bits;
volatile unsigned int  IOCON4;
volatile IOCON4BITS IOCON4bits;
volatile unsigned int  FCLCON4;
volatile FCLCON4BITS FCLCON4bits;
volatile unsigned int  PDC4;
volatile unsigned int  PHASE4;
volatile unsigned int  DTR4;
volatile unsigned int  ALTDTR4;
volatile unsigned int  SDC4;
volatile unsigned int  SPHASE4;
volatile unsigned int  TRIG4;
volatile TRIG4BITS TRIG4bits;
volatile unsigned int  TRGCON4;
volatile TRGCON4BITS TRGCON4bits;
volatile unsigned int  STRIG4;
volatile STRIG4BITS STRIG4bits;
volatile unsigned int  PWMCAP4;
volatile PWMCAP4BITS PWMCAP4bits;
volatile unsigned int  LEBCON4;
volatile LEBCON4BITS LEBCON4bits;
volatile unsigned int  LEBDLY4;
volatile LEBDLY4BITS LEBDLY4bits;
volatile unsigned int  AUXCON4;
volatile AUXCON4BITS AUXCON4bits;
volatile unsigned int  PWMCON5;
volatile PWMCON5BITS PWMCON5bits;
volatile unsigned int  IOCON5;
volatile IOCON5BITS IOCON5bits;
volatile unsigned int  FCLCON5;
volatile FCLCON5BITS FCLCON5bits;
volatile unsigned int  PDC5;
volatile unsigned int  PHASE5;
volatile unsigned int  DTR5;
volatile unsigned int  ALTDTR5;
volatile unsigned int  SDC5;
volatile unsigned int  SPHASE5;
volatile unsigned int  TRIG5;
volatile TRIG5BITS TRIG5bits;
volatile unsigned int  TRGCON5;
volatile TRGCON5BITS TRGCON5bits;
volatile unsigned int  STRIG5;
volatile STRIG5BITS STRIG5bits;
volatile unsigned int  PWMCAP5;
volatile PWMCAP5BITS PWMCAP5bits;
volatile unsigned int  LEBCON5;
volatile LEBCON5BITS LEBCON5bits;
volatile unsigned int  LEBDLY5;
volatile LEBDLY5BITS LEBDLY5bits;
volatile unsigned int  AUXCON5;
volatile AUXCON5BITS AUXCON5bits;
volatile unsigned int  TRISA;
volatile TRISABITS TRISAbits;
volatile unsigned int  PORTA;
volatile PORTABITS PORTAbits;
volatile unsigned int  LATA;
volatile LATABITS LATAbits;
volatile unsigned int  ODCA;
volatile ODCABITS ODCAbits;
volatile unsigned int  CNENA;
volatile CNENABITS CNENAbits;
volatile unsigned int  CNPUA;
volatile CNPUABITS CNPUAbits;
volatile unsigned int  CNPDA;
volatile CNPDABITS CNPDAbits;
volatile unsigned int  ANSELA;
volatile ANSELABITS ANSELAbits;
volatile unsigned int  TRISB;
volatile TRISBBITS TRISBbits;
volatile unsigned int  PORTB;
volatile PORTBBITS PORTBbits;
volatile unsigned int  LATB;
volatile LATBBITS LATBbits;
volatile unsigned int  ODCB;
volatile ODCBBITS ODCBbits;
volatile unsigned int  CNENB;
volatile CNENBBITS CNENBbits;
volatile unsigned int  CNPUB;
volatile CNPUBBITS CNPUBbits;
volatile unsigned int  CNPDB;
volatile CNPDBBITS CNPDBbits;
volatile unsigned int  ANSELB;
volatile ANSELBBITS ANSELBbits;
volatile unsigned int  TRISC;
volatile TRISCBITS TRISCbits;
volatile unsigned int  PORTC;
volatile PORTCBITS PORTCbits;
volatile unsigned int  LATC;
volatile LATCBITS LATCbits;
volatile unsigned int  ODCC;
volatile ODCCBITS ODCCbits;
volatile unsigned int  CNENC;
volatile CNENCBITS CNENCbits;
volatile unsigned int  CNPUC;
volatile CNPUCBITS CNPUCbits;
volatile unsigned int  CNPDC;
volatile CNPDCBITS CNPDCbits;
volatile unsigned int  ANSELC;
volatile ANSELCBITS ANSELCbits;
volatile unsigned int  TRISD;
volatile TRISDBITS TRISDbits;
volatile unsigned int  PORTD;
volatile PORTDBITS PORTDbits;
volatile unsigned int  LATD;
volatile LATDBITS LATDbits;
volatile unsigned int  ODCD;
volatile ODCDBITS ODCDbits;
volatile unsigned int  CNEND;
volatile CNENDBITS CNENDbits;
volatile unsigned int  CNPUD;
volatile CNPUDBITS CNPUDbits;
volatile unsigned int  CNPDD;
volatile CNPDDBITS CNPDDbits;
volatile unsigned int  ANSELD;
volatile ANSELDBITS ANSELDbits;
volatile unsigned int  _APPO;
volatile unsigned int  _APPIN;
volatile unsigned int  APPS;
volatile APPSBITS APPSbits;
volatile unsigned int  STROUTL;
volatile unsigned int  STROUTH;
volatile unsigned int  STROVCNT;
volatile unsigned int  JDATAH;
volatile unsigned int  JDATAL;
unsigned int _FSEC;
unsigned int _FBSLIM;
unsigned int _FSIGN;
unsigned int _FOSCSEL;
unsigned int _FOSC;
unsigned int _FWDT;
unsigned int _FPOR;
unsigned int _FICD;
unsigned int _FDEVOPT;
unsigned int _FALTREG;

