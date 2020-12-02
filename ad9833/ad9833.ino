/*
 * AD9833 Waveform Generator
 *
   | Arduino | D10 - SS (Slave Select)          --- FSYNC |        |
   |   UNO   | D11 - MOSI (Master Out Slave In) --- SCLK  | AD9833 |
   |         | D13 - SCK (Serial Clock)         --- SDATA |        |
   |         | A0  - ADC (10-bit)               --- VOUT  |        |
 *
*/

/*--- Preprocessor ---*/
#include <SPI.h> // Import SPI library

/*--- Preprocessor ---*/
#define outPin A0               // ADC pin

#define SINE_WAVE        0x2000 // 8192_DEC, 0010 0000 0000 0000, Sine Wave
#define TRIANGULAR_WAVE  0x2002 // 8194_DEC, 0010 0000 0000 0010, MODE = 1, Triangular Wave
#define SQUARE_WAVE      0x2020 // 8224_DEC, 0010 0000 0010 0000, OPBITEN = 1, Square Wave (MSB/2)
#define HALF_SQUARE_WAVE 0x2008 // 8232_DEC, 0010 0000 0010 1000, OPBITEN = 1, DIV2 = 1, Half Square Wave (MSB)

#define N 800                   // Measurment sampling number for smoothing

/*--- Constants ---*/
const unsigned long baudSpeed = 115200;           // Sets the data rate in bits per second (baud) for serial data transmission

const byte vIn = 5;                               // Supply voltage from Arduino
const byte resBits = 10;                          // Resolution of ADC (10 bits)
const float vConv = vIn / (pow(2, resBits) - 1);  // Voltage of ADC level (2^bits)

/*--- Global Variables ---*/
float vOut = 0.0; // Output of the function generator

/*--- Function Prototype ---*/
void WriteAD9833(uint16_t );
void setup(void);
void loop(void);

/*--- Functions Definition ---*/
void WriteAD9833(uint16_t Data) {
    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV2, MSBFIRST, SPI_MODE2));
    digitalWrite(SS, LOW);
    delayMicroseconds(1);
    SPI.transfer16(Data);
    digitalWrite(SS, HIGH);
    SPI.endTransaction();
}

/*--- Initialization ---*/
void setup(void) {
  Serial.begin(baudSpeed);	// Initializes serial port
	SPI.begin();							// Initializes SPI

	// AD9833 Initialization
	WriteAD9833(0x2100); // 0010 0001 0000 0000, Reset + DB28
	WriteAD9833(0x50C7); // 0101 0000 1100 0111, Freq0 LSB (4295)
	WriteAD9833(0x4000); // 0100 0000 0000 0000, Freq0 MSB (0)
	WriteAD9833(0xC000); // 1100 0000 0000 0000, Phase0 (0)
	WriteAD9833(0x2000); // 0010 0000 0000 0000, Exit Reset
}

/*--- Running ---*/
void loop(void) {
	WriteAD9833(TRIANGULAR_WAVE);

  for (unsigned int i = 0; i < N; ++i) {    // Get samples for smooth the value
    vOut = vOut + analogRead(outPin);
    delay(1);                               // delay in between reads for stability
  }
	vOut = (vOut * vConv) / N;                // ADC of voltage meter output voltage

	Serial.println(vOut, 4);                  // Print the results to the Serial Monitor
}
