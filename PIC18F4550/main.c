

#include <xc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include<p18f4550.h>
#define _XTAL_FREQ 8000000  

#include "config.h"
#include "GLCD.h"
#include "rutines_GLCD.h"


const char * win ="YOU WIN\n";
const char * lose="YOU LOSE\n";
const char * restart="PRESS RESTART\n";
char x = 32;

char y = 4;

signed char dx = 1;

signed char dy = 1;

unsigned int barra = 32;

unsigned int bool = 0;

char pts_Y = 0;
char pts_R = 0;

void drawPunts(){
   if(pts_Y==0) putch2(3,6,'0');
   else writeNum(3,6,pts_Y);
   putch2(3,12,'-');
   if(pts_R==0) putch2(3,18,'0');
   else writeNum(3,18,pts_R);
}

int newdata;
void drawBorder(){
   //for(int i = 0; i<8; i++) writeByte(i,0,0xFF); //izquierda
   //for(int i = 0; i<8; i++) writeByte(i,127,0xFF); //derecha
   
   
   for(int i = 1; i<127; i++) writeByte(0,i,0x1); //arriba
   for(int i = 1; i<127; i++) writeByte(63,i,0x80); //abajo
   for(int i = 0; i<8; i++) writeByte(i,126,0x55); //derecha
   for(int i = 0; i<8; i++) writeByte(i,127,0xAA); //derecha
}
void drawBall(int x,int y){
     for(int ix = -1; ix<=1; ++ix)
	for(int iy = -1; iy<=1; ++iy)
	    SetDot(x+ix, y+iy);
}
void drawBarra(char x){
     for(int i = -8; i<8; ++i){
	    //SetDot(x+i, 1);
	    SetDot(x+i, 2);
     }
}
void clearBarra(int x){
     for(int i = -8; i<8; ++i){
	    //ClearDot(x+i, 1);
	    ClearDot(x+i, 2);
     }
}
int nextPos(){
     if(x+dx>61)
	dx = -1;
     else if (x+dx < 2)
	dx = 1;
     x+=dx;
     
     if(y+dy>126)
	return 0;
     else if (y+dy < 1) return -1;
     else if (y+dy < 4) 
	if(x>=(barra-9) && x<=(barra+8)) dy = 1;
     
     y+=dy;
     
     return 1;
}
void clearBall(int x,int y){
     for(int ix = -1; ix<=1; ++ix)
	for(int iy = -1; iy<=1; ++iy)
	    ClearDot(x+ix, y+iy);
}



void iniGLCD()
 {
	
	PORTD=0; 		   
	PORTB=0;  
	TRISD = 0x00;	
	TRISA = 0x00;
	LATA= 0x08;    
	TRISB = 0x00;
	GLCDinit();		   
	clearGLCD(0,7,0,127);    
	setStartLine(0);         

       
}

void playSound(){
   while(1){
      PORTBbits.RB7 = 1;
      //PORTA = 0xFF;
      __delay_ms(10);
      PORTBbits.RB7 = 0;
      //PORTA = 0;
      __delay_ms(10);
   }
   
}

void writeTxt(byte page, byte y, char * s) {
 int i=0;
 while (*s!='\n') { 
  putch2(page, y+i, *(s++));
  i++;
 }
}
	

void ini_holamon(){
   TRISCbits.RC6 = 0;
   TRISCbits.RC7 = 1;
   TXSTA = 0x26;
   /*TXSTAbits.SYNC=0;
   TXSTAbits.TXEN=1;
   TXSTAbits.SENDB=1;*/
   SPBRG = 0x08; // Porque un 8?
   RCSTA = 0x90;
   BAUDCON = 0x00;
   //inicialitzar en timer i les interrupcions corresponents.
   RCONbits.IPEN = 1;
    INTCON2bits.TMR0IP = 1;
   INTCON = 0xE0;
   IPR1= 0x30;
   //T0CON = 0xC5; //POSSIBLE CUAJADA
   //TMR0=0x00F7; 
   //TMR0=0xFFF7;
}

unsigned char getc_usart1 (void)
{
 while(!PIR1bits.RCIF);
return RCREG; // RCIF clears automatically
}

char * DATA_PACKET(int x, int y, int dirx, int diry){
   char buf[32];
   sprintf(buf,"%d,%d,%d,%d\n", x, y, dirx, diry/*,sizeof(buf)*/);
   return buf;
}

void SEND_CHAR(char su){
   while (! PIR1bits.TX1IF);
   TXREG = su;
}
void SEND_PACKET(char* packet){
   int i = 2;
   int ptr = 0;
   while(packet[ptr] != '\n'){
      drawBall(43,i*5);
      ++i;
      SEND_CHAR(packet[ptr++]);
   }
   SEND_CHAR(packet[ptr]);
}


void  main(void){
    
   char lect[256];
	 int aux = 0;
	 iniGLCD();
	 ini_holamon();
   char data;
   //char *packet;
   drawBorder();
	int ret;
      int ptr;
        while (1) {
	   ptr=0;
	  
	   drawBorder();
	    
	      while((ret=nextPos())>0){
	      drawBarra(barra);
	       drawBall(x,y);
	       //__delay_ms(10);
	       clearBall(x,y);
	      clearBarra(barra);
	      if (barra == 55) bool = 1;
	      if (barra == 9) bool = 0;
	       if (bool == 0) ++barra;
		  else --barra;
	    }
	    
	   if(ret==-1){
	     pts_R++;
	     if(pts_R==2){
	       while (ptr != 2) {
	       if(ptr == 0) data = x;
	       else data = 2;
	       SEND_CHAR(data);
	       ++ptr;
	    }
	    break;
	     }
	      while (ptr != 2) {
	       if(ptr == 0) data = x;
	       else data = 0;  
	       SEND_CHAR(data);
	       ++ptr;
	      }
	      ptr = 0;
	      drawPunts();
	      __delay_ms(1000);
	      clearGLCD(0,7,0,127);
	      drawBorder();
	      drawBarra(barra);
	      x = 32;
	      y = 4;
	      dx = 1;
	      dy = 1;
	      continue;
	   }
	    while (ptr != 2) {
	       if(ptr == 0) data = x;
	       else data = dx;
	       SEND_CHAR(data);
	       ++ptr;
	    }
	    ptr = 0;
	    while(!PIR1bits.RCIF);
	      while (ptr != 2) {
		 
		  if(PIR1bits.RCIF){
		     if(ptr == 0) x = RCREG;
		     else dx = RCREG;
			
			++ptr;
		  }
	      }
	      ptr = 0;
	      if(dx == 2){
		 pts_Y = 3;
		 break;
	      }
	      
	      if(dx == 0){
			pts_Y++;
	      drawPunts();
	      __delay_ms(1000);
	      clearGLCD(0,7,0,127);
	      drawBorder();
	      drawBarra(barra);
		 while(!PIR1bits.RCIF);
	      while (ptr != 2) {
		 
		  if(PIR1bits.RCIF){
		     if(ptr == 0) x = RCREG;
		     else dx = RCREG;
			
			++ptr;
		  }
	      }
			   }
	      x = 63-x;
	      y = 126;
	      dy = -1;
	      dx *=-1;
	    drawBorder();
	    
	   }
	   
	   clearGLCD(0,7,0,127);
	    while(1){
		if(pts_Y==3){
		  writeTxt(2,6,win);
		}
		else{
		  writeTxt(2,6,lose);
		}
		writeTxt(4,6,restart);
		__delay_ms(2000);
		clearGLCD(0,7,0,127);
	    }
	     
   }