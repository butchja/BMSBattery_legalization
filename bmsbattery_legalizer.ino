// ****************************************************************************************
// *                                                                                      *
// * Legalization Firmware for STM32F103C8 Arduino Nano Style Boards                      *
// * for BMSBattery S-LCD3 and SN12SNN 72V Sine Motor Driver*                             *
// * * might be compatible to others                                                      *
// *                                                                                      *
// * Allows limiting to maximum legal speeds of 25km/h                                    *
// * Reduces the thumb throttle to 6km/h (traction aid only)                              *
// * Output power is limited to device's minimum I/2                                      *
// * A logical condition needs to be met to unlock legal limitations                      *
// * that might be used for an off-road mode                                              *
// *                                                                                      *
// * TODO:                                                                                *
// * - adding RFID support RC-522 SPI based                                               *
// * - reducing power output furthermore or limiting displayed power                      *
// *                                                                                      *
// * Creator                                                                              *
// *  butchja@arcor.de                                                                    *
// *                                                                                      *
// ****************************************************************************************

#define CRC_POLYNOM       26 // exclusive OR after bytes has been xor'ed could be everything, default is 2
#define MAX_PACKET_SIZE   23

unsigned long time;
uint8_t buf[MAX_PACKET_SIZE]; // buffer for S-LCD3 to S12SN communication protocol
uint8_t bufi = 0;      // index for S-LCD3 to S12SN buffer

// the setup function runs once when you press reset or power the board
void setup() {
  Serial1.begin(9600); // 5V tolerant, PA10: RX  ;  PA9:  TX   -   Motor Controller side
  Serial2.begin(9600); // 3V3 Debug/Tracing
  Serial3.begin(9600); // 5V tolerant, PB11: RX  ;  PB10: TX   -   LCD side  
}

void loop() {
  // let stuff from controller to LCD pass through
  if (Serial1.available()) {
    Serial3.write(Serial1.read());
  }

  // get data from LCD
  if (Serial3.available()) {
    while (Serial3.available()) {
      buf[bufi++] = Serial3.read();
    }
  }
  else {
    // we should have a complete packet here
    if (packet_is_valid()) 
    {
      if (packet_needs_mod()) modify_packet();
      send_packet();
      bufi = 0;
    } else {
      // we need to reset bufi in case of an error (e.g. wrong CRC)
      if (bufi >= MAX_PACKET_SIZE)
        bufi = 0;
    }
  }
}

uint8_t packet_needs_mod() {
  // check if the packet is of interest
  if (!((buf[1]&0x80)==0x80)) return 1; 
  else return 0;  
}

void modify_packet() { // change bytes here
  uint8_t speed;
  
  //set param C5 to minimum current of I/2
  buf[7]=buf[7]&0xf0;
  buf[7]=buf[7]|0x03;

  //set param C4 thumb throttle to 6km/h 
  buf[8]=buf[8]&0x1f; 
  buf[8]=buf[8]|0x20;

  speed=(buf[2]>>3)|(0x2&buf[4])+10; // rearrange speed configuration bits for clarity
  
  //set max speed to 27km/h
  /*if (speed>27){
    // set 27km/h 0b010001
    buf[2]=buf[2]&0b00000111; //delete speed bit 0...4  0b 0000 0RRR   R:retain
    buf[2]=buf[2]|0b10001000; //set speed bit 0...4     0b 0000 1RRR
    buf[4]=buf[4]&0b11011111; //clear speed bit 5       0b RR0R RRRR
  }*/
  // set max speed to 25km/h 0b001111
  if (speed>25){      
    buf[2]=buf[2]&0x07; //delete speed bit 0...4
    buf[2]=buf[2]|0x70; //set speed bit 0...4 
    buf[4]=buf[4]&0xdf; //clear speed bit 5*/
  }
  
  //generate new CRC after anything has changed
  buf[5]=gen_crc();
}



uint8_t packet_is_valid() {
  // we check the packetsize here
  if (bufi == 13 || bufi == MAX_PACKET_SIZE)
  {
    if (buf[5]==gen_crc()) return 1; // check CRC
  }
  return 0;
}

void send_packet() {
  for (int i = 0; i < bufi; i++)
    Serial1.write(buf[i]);
}

uint8_t gen_crc(){
  uint8_t crc=0;
  crc=buf[0]^buf[1]^buf[2]^buf[3]^buf[4]^buf[6]^buf[7]^buf[8]^buf[9]^buf[10]^buf[11]^buf[12];
  crc=crc^crc_polynom; 
  return crc;
}
