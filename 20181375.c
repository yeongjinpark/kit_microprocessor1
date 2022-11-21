

//시리얼과 연동하여 시리얼로 2를 입력하면 이떄까지 도트에 출력한 합을 출력하고
//8을 입력하면 곱을 출력,  4를 입력하면 최대값, 6을 입력하면 최소값을 출력하는 시스템
#include <avr/io.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <util/delay.h>

static int putchar0(char c, FILE *stream);
static FILE mystdout =FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);

#define UP 2
#define DOWN 8
#define LEFT 4
#define RIGHT 6

#define KEY_DDR DDRF
#define KEY_PORT PORTF
#define KEY_PIN PINF

#define DOT8_DATA_DDR   DDRA
#define DOT8_DATA_PORT   PORTA

#define DOT8_LINE_DDR   DDRC
#define DOT8_LINE_PORT   PORTC

volatile const unsigned char number[9][8]={
	{0xef,0xe7,0xeb,0xef,0xef,0xef,0xef,0x83},     // 1
	{0xff,0xe7,0xdb,0xdf,0xef,0xf7,0xc3,0xff},     // 2
	{0xff,0xe7,0xdb,0xdf,0xe7,0xdf,0xdb,0xe7},     // 3
	{0xff,0xef,0xf7,0xeb,0xc1,0xef,0xef,0xff},     // 4
	{0xff,0xc3,0xfb,0xe3,0xdf,0xdb,0xe7,0xff},     // 5
	{0xff,0xc7,0xfb,0xe3,0xdb,0xdb,0xe7,0xff},     // 6
	{0xff,0xc3,0xdb,0xdf,0xdf,0xdf,0xdf,0xff},     // 7
	{0xff,0xe7,0xdb,0xdb,0xe7,0xdb,0xdb,0xe7},     // 8
	{0xff,0xe7,0xdb,0xdb,0xc7,0xdf,0xdf,0xdf}		// 9
};

unsigned char key_scan(unsigned char a);
int key_convert(unsigned char in);

static int putchar0(char c, FILE *stream)
{

	if(c=='\n')
	putchar0('\r',stream);
	//while(!(UCSR0A & (1<<UDRE0)));
	while(!(UCSR0A & 0x20));
	UDR0=c;
	return 0;
}

char getchar0()
{
while(!(UCSR0A & (1<<RXC0)));
return(UDR0);
}

int main()
{
	int num[100];
	int m=0;
	int cnt=0, sum_res=0, mul_res=0,max=0, min=99;
	char c;

	
	DOT8_DATA_DDR=0xff;
	DOT8_LINE_DDR=0xff;

	volatile unsigned char in,in0,in1,in2,in3;

	UBRR0H=0;               //12번 비트가 의미를 가짐
	UBRR0L=8;               // 16Mhz, 115200baud

	UCSR0B=0x18;             //Receive(RX) 및 Transmit(TX) Enable
	UCSR0C=0x06;             //비동기, no parity, stop bit 1, 데이터 비트수 8-bit
	stdout=&mystdout;

	while(1)
	{
		c=getchar0();
		printf("%c\n",c);
		
		c=c-'0';
		
		//키패드 첫번째줄 스캔
		in0=key_scan(0x01);
		
		//키패드 첫번째줄 스캔
		in1=key_scan(0x02);
		
		//키패드 첫번째줄 스캔
		in2=key_scan(0x04);
		
		//키패드 첫번째줄 스캔
		in3=key_scan(0x08);
		
		
		in=(in3 | in2| in1| in0);  //4줄 입력 값 취합
		in=key_convert(in);   //키 입력값을 숫자로 변환

		num[cnt]=in;
		if(c>0 && c<10){
			switch(c){
				case UP:
				 for(m=0; m<cnt; m++){
					 sum_res +=num[m];
				 }
				break;
				case DOWN:
				 for(m=0; m<cnt; m++){
					 mul_res +=num[m];
				 }
				break;
				case LEFT:
				  for(m=0; m<cnt; m++){
					 if(max<num[m]){
						max =num[m];
					}
				  }
				break;
				
				case RIGHT:
					  for(m=0; m<cnt; m++){
						  if(min>num[m]){
							  min =num[m];
						  }
					  }
				break;
			}
	}
	cnt++;
	
}
}

unsigned char key_scan(unsigned char a)
{
	unsigned char in;

	KEY_DDR = 0xFF;    //all out
	KEY_PORT = a;      //키패드 줄 위치
	KEY_DDR = 0x0F;    //Port 7bit~4bit(In) 3bit~0bit(Out)
	_delay_us(50);

	// 키패드 입력 포트의 상위 4자리 값 추출
	in = (PINF & 0xF0);
	if(in != 0x00)
	{
		in |= a;
		return in;
	}
	return 0;
}

//키패드에 각 줄에 입력값이 있는지 스캔하는 함수
int key_convert(unsigned char in)
{
	int returnvalue;
	int j=0;

	switch(in)
	{
		case 0x11 :
		returnvalue = 1;
		for(j=0; j<8; j++){
			PORTA=number[0][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x21 :
		returnvalue = 2;
		for(j=0; j<8; j++){
			PORTA=number[1][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x41 :
		returnvalue = 3;
		for(j=0; j<8; j++){
			PORTA=number[2][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x12 :
		returnvalue = 4;
		for(j=0; j<8; j++){
			PORTA=number[3][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x22 :
		returnvalue = 5;
		for(j=0; j<8; j++){
			PORTA=number[4][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x42 :
		returnvalue = 6;
		for(j=0; j<8; j++){
			PORTA=number[5][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x14 :
		returnvalue = 7;
		for(j=0; j<8; j++){
			PORTA=number[6][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x24 :
		returnvalue = 8;
		for(j=0; j<8; j++){
			PORTA=number[7][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;

		case 0x44 :
		returnvalue = 9;
		for(j=0; j<8; j++){
			PORTA=number[8][j];
			PORTE= 1<< j;
			_delay_ms(1);
		}
		break;
		default : returnvalue = 0;  break;
	}
	return returnvalue;
}





