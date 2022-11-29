 #include <Arduino.h>                        //
 #include <Wire.h>                           // Library I2C
 #include <EEPROM.h>                         // Library EEPROM
 #include <LiquidCrystal_I2C.h>              // Library LCD with I2C
 #include <ModbusRtu.h>                      // Library Modbus RTU
 #define  ID 1                               // define ID pada Modbus 1

//Modbus
 Modbus slave(1,0,2);                        // Interface settings
 boolean led;                                //
 int8_t state = 0;                           // Keadaan int8_t awal = 0
 uint16_t au16data[8];                       // Data array for Modbus network sharing

//LCD
 LiquidCrystal_I2C lcd(0x27, 16, 2);         // Set alamat LCD 16x2

// Insialisasi pin
 int button         = 2;                     // Pin  push button reset
 int uvtron         = 7;                     // Pin  uvtron
 int buzzer         = 31;                    // Pin  buzzer 
 int green          = 33;                    // Pin  LED hijau 
 int red            = 35;                    // Pin  LED merah
 int api            = HIGH;                  // HIGH berarti NO FLAME --aktif low
 int dataAddress    = 3;                     // inisialisasi dataAddress EEPROM = 0s
 int dataAddress2   = 4;                     // dataAddress for ASE2000
 int lastEEPROMData = 0;                     // inisialisasi data EEPROM dalam variabel lastEEPROMData = 0
 int condition      = 0;                     // Kondisi awal condition

//  EEPROMs
 void resetEEPROM(){                         // Fungsi void reset
   EEPROM.update(dataAddress, 0);            // Tulis 0 di dataAddress
   condition = 0;                            // Jalankan kondisi 0                     
//   digitalWrite(11, HIGH);
 }

 /* ========================================================================*/

void setup()
{
  Serial.begin(9600);                        // Serial begin 9600
  slave.begin (9600);                        // Start communication Modbus
  EEPROM.begin();                            // Mulai EEPROM
  Wire.begin();

  api = EEPROM.read(dataAddress);            // variabel api baca dataAddress
  condition = api;                           // Baca Condition pake EEPROM (varibel api)

  pinMode(button, INPUT);                    // Push Button sebagai INPUT
  pinMode(uvtron, INPUT);                    // UVTRon sebagai INPUT
  pinMode(buzzer, OUTPUT);                   // Buzzer sebagai OUTPUT
  pinMode(red, OUTPUT);                      // LED Merah sebagai OUTPUT
  pinMode(green, OUTPUT);                    // LED Hijau sebagai OUTPUT

  pinMode(3, INPUT);                         // Define input
  pinMode(4, INPUT);                         // Define input
  pinMode(5, INPUT);                         // Define input
  pinMode(6, INPUT);                         // Define input
  pinMode(7, INPUT);                         // Define input
  pinMode(8, INPUT);                         // Define input
  pinMode(9, INPUT);                         // Define input
  pinMode(10, INPUT);                        // Define input
  pinMode(11, OUTPUT);

  lcd.init();                                // Inisialisasi LCD
  lcd.backlight();                           // LCD backlight
  lcd.setCursor(0, 0);                       // Set letak kursor LCD
  lcd.print("DETEKSI API FCO");              // LCD print 'DETEKSI DETEKSI API FCO'
  lcd.setCursor(2, 1);                       // Set letak kursor LCD
  lcd.print("PLN UP2D JTY");                 // LCD print 'DETEKSI DETEKSI API FCO'
  delay(2000);                               // Delay 2 detik
  lcd.clear();

  pinMode(button, INPUT_PULLUP);             // button sebagai input pullup
  attachInterrupt(digitalPinToInterrupt(button), resetEEPROM, FALLING);   // interupt button for reset EEPROM 
}

void io_poll(){                                        // You cannot read just one bit from a holding register. Must read all 16 bits, and then test the individual bit you are interested in for true or false (1 or 0).               
    bitWrite( au16data[0], B00000000, digitalRead(3)); // 0000 0001 -- read pin 3
    bitWrite( au16data[0], B00000001, digitalRead(4)); // 0000 0010 -- read pin 4
    bitWrite( au16data[0], B00000010, digitalRead(5)); // 0000 0100 -- read pin 5
    bitWrite( au16data[0], B00000011, digitalRead(6)); // 0000 1000 -- read pin 6
    bitWrite( au16data[0], B00000100, condition);      // 0001 0000 --read variable 'condition'
    bitWrite( au16data[0], B00000101, digitalRead(8)); // 0010 0000 -- read pin 8
    bitWrite( au16data[0], B00000110, digitalRead(9)); // 0100 0000 -- read pin 9
    bitWrite( au16data[0], B00000111, digitalRead(10));// 1000 0000 -- read pin 10

    EEPROM.write (dataAddress2, condition);            // EEPROMwrite to dataAddress2 for using with ASE2000
    pinMode(button, INPUT_PULLUP);                     // Reset EEPROM dataAddress2
    attachInterrupt(digitalPinToInterrupt(button), resetEEPROM, FALLING); // interupt button for reset EEPROM with ASE2000 Modbus Simulator
  }


void loop()
{  
  api = digitalRead(uvtron);           // BUAT read alamt eeprom 3
  EEPROM.write(dataAddress, api);      // Write EEPROM dengan nilai dari variabel 'api'
  
  state = slave.poll(au16data,8);      // receive reset command from master
  if (state > 7) {
    digitalWrite(11, HIGH);
  }
  else{
    digitalWrite(11, LOW);   
  }
 
  io_poll();                           // Modbus

  if (api == HIGH){                    // Logika pembacaan api sensor UVTron
    EEPROM.update(dataAddress, 1);     // update dataAddress dengan nilai 1
    condition = 1;                     // Jalankan condition 1
  }

  if (condition == 0){                 // Kondisi stand by atau tidak ada api
       digitalWrite(buzzer, LOW);      // Buzzer mati
       digitalWrite(green, HIGH);      // LED hijau nyala --aktif low
       digitalWrite(red, LOW);         // LED merah mati
//       lcd.setCursor(0,00);             // Set letak kursor LCD
//       lcd.print(rtc.getDateStr());     // Prosedur pemanggilan tanggal 
//       lcd.setCursor(11,0);             // Set letak kursor LCD
//       lcd.print(rtc.getTimeStr());     // Prosedur pemanggilan tanggal
       lcd.setCursor(0, 1);            // Set letak kursor LCD
       lcd.print("Tidak Ada Api");     // LCD print teks
   } 

  if (condition == 1){                 // maka
       digitalWrite(buzzer, HIGH);     // Buzzer bunyi
       digitalWrite(green, LOW);       // LED hijau mati
       digitalWrite(red, HIGH);        // LED merah nyala
//       lcd.setCursor(0,00);              // Set letak kursor LCD
//       lcd.print(rtc.getDateStr());      // Prosedur pemanggilan tanggal 
//       lcd.setCursor(11,0);              // Set letak kursor LCD
//       lcd.print(rtc.getTimeStr());     // Prosedur pemanggilan tanggal
       lcd.setCursor(0, 1);              // Set letak kursor LCD
       lcd.print("Api Terdeteksi");      // LCD print teks
     }

}
