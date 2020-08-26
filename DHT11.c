#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdio.h>

#define DHT11_PORT 		PORTA
#define DHT11_PORTPIN 	PINA0
#define DHT11_DDR 		DDRA
#define DHT11_DDRPIN 	DDRA0
#define DHT11_PINPORT 	PINA
#define DHT11_PIN 		PINA0

#define DHT11_INPUT_MODE() DHT11_DDR&=~(1<<DHT11_DDRPIN)
#define DHT11_OUTPUT_MODE() DHT11_DDR|=(1<<DHT11_DDRPIN)
#define DHT11_LOW() DHT11_PORT&=~(1<<DHT11_PORTPIN)
#define DHT11_HIGH() DHT11_PORT|=(1<<DHT11_PORTPIN)

#define DHTLIB_OK                               0
#define DHTLIB_ERROR_CHECKSUM                    -1
#define DHTLIB_ERROR_TIMEOUT                    -2

int DHT11_READ_TH(int *temp,int *humidity){
	unsigned char bytes[5];
	unsigned char sum;
	unsigned int bitcount=8;
	unsigned int loopcount=0;
	unsigned int byteid=0;
	int i=0;
	// EMPTY BUFFER
	for (i=0; i< 5; i++) bytes[i] = 0;
	// REQUEST SAMPLE
	DHT11_OUTPUT_MODE();
	DHT11_LOW();
	_delay_ms(20);
	DHT11_HIGH();
	_delay_us(2);
	DHT11_INPUT_MODE();
	DHT11_LOW();
	loopcount=0;
	while(!(DHT11_PINPORT&(1<<DHT11_PIN))){ //pin is low
		_delay_us(40);
		loopcount++;
	}
	if (loopcount>80) return DHTLIB_ERROR_TIMEOUT;
	loopcount=0;
	while((DHT11_PINPORT&(1<<DHT11_PIN))){ //pin is high
		_delay_us(1);
		loopcount++;
	}
	if (loopcount>80) return DHTLIB_ERROR_TIMEOUT;
	for(i=0;i<40;i++){
		loopcount=0;
		while(!(DHT11_PINPORT&(1<<DHT11_PIN))){}
		while((DHT11_PINPORT&(1<<DHT11_PIN)) && loopcount<100){loopcount++;_delay_us(1);}
		if(loopcount>15)
		bytes[byteid] |= (1 << bitcount);
		if (bitcount == 0)   // next byte?
		{
			bitcount = 7;    // restart at MSB
			byteid++;      // next byte!
		}
		else bitcount--;
	}
	*humidity    = bytes[0];
	*temp = bytes[2];
	sum = (bytes[0] + bytes[1]+ bytes[2]+ bytes[3])&0xff;
	if (bytes[4] != sum) return DHTLIB_ERROR_CHECKSUM;
	return DHTLIB_OK;
}