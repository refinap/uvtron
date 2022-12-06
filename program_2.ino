 #include <Arduino.h>                       //
 #include <Wire.h>                          // Library I2C
 #include <EEPROM.h>                        // Library EEPROM
 #include <LiquidCrystal_I2C.h>             // Library LCD with I2C
 #include <ModbusRtu.h>                     // Library Modbus RTU
 #define  ID 1                              // define ID pada Modbus 1
 #include <TimerOne.h>                      // Library iternal interrupt

//Modbus
 Modbus slave(1,0,2);                       // Interface settings
 boolean led;                               //
 int8_t state = 0;                          // Keadaan int8_t awal = 0
 uint16_t au16data[8];                      // Data array for Modbus network sharing

//LCD
 LiquidCrystal_I2C lcd(0x27, 16, 2);        // Set alamat LCD 16x2

// Insialisasi pin
 int button         = 2;                    // Pin  push button reset
 int uvtron         = 7;                    // Pin  uvtron
 int buzzer         = 31;                   // Pin  buzzer 
 int green          = 33;                   // Pin  LED hijau 
 int red            = 35;                   // Pin  LED merah
 int api            = HIGH;                 // HIGH berarti NO FLAME --aktif low
 int dataAddress    = 3;                    // inisialisasi dataAddress EEPROM = 0s
 int dataAddress2   = 4;                    // dataAddress2 pada alamat 4
 int lastEEPROMData = 0;                    // inisialisasi data EEPROM dalam variabel lastEEPROMData = 0
 int condition      = 0;

//  EEPROMs
   void resetEEPROM(){                      // Fungsi void reset
   EEPROM.update(dataAddress, 0);           // Tulis 0 di dataAddress
   condition = 0;                           // Jalankan kondisi 0                     
//   digitalWrite(11, HIGH);
 }

    void timer(){                            // Fungsi internal interrupt
      EEPROM.update(dataAddress, 0);         // Tulis 0 di dataAddress
      condition = 0;                         // Jalankan kondisi 0 
    }

 /* ========================================================================*/

void setup()
{
  Serial.begin(9600);                        // Serial begin 9600
  slave.begin (9600);                        // Start communication Modbus
  EEPROM.begin();                            // Mulai EEPROM
  Wire.begin();                              // Mulai I2C

  api = EEPROM.read(dataAddress);            // lastEEPROMData baca dataAddress
  condition = api;                           // Baca Condition pake EEPROM

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
  lcd.clear();                               // LCD clear

  pinMode(button, INPUT_PULLUP);             // Button input_pullup
  attachInterrupt(digitalPinToInterrupt(button), resetEEPROM, FALLING);

  Timer1.initialize();                      // Internal interrupt
}

void io_poll(){ 
    bitWrite( au16data[0], B00000000, digitalRead(3)); // You cannot read just one bit from a holding register.
    bitWrite( au16data[0], B00000001, digitalRead(4)); // Must read all 16 bits, and then test the individual bit you are interested in for true or false (1 or 0).
    bitWrite( au16data[0], B00000010, digitalRead(5));
    bitWrite( au16data[0], B00000011, digitalRead(6));
    bitWrite( au16data[0], B00000100, condition );     // baca variabel 'condition'
    bitWrite( au16data[0], B00000101, digitalRead(8));
    bitWrite( au16data[0], B00000110, digitalRead(9));
    bitWrite( au16data[0], B00000111, digitalRead(10));

    EEPROM.write (dataAddress2, condition);            // tulis condition pada dataAddress2 
    pinMode(button, INPUT_PULLUP);                     // Button input_pullup
    attachInterrupt(digitalPinToInterrupt(button), resetEEPROM, FALLING); 
  }

void loop(){      
  api = digitalRead(uvtron);        // BUAT read alamt eeprom 3
  state = slave.poll(au16data,8);   // receive reset command from master
 
  if (state > 7) {                 // Receive from master -- for Force Single Coil
  digitalWrite(11, HIGH);
   Timer1.attachInterrupt(timer);
      //attachInterrupt(digitalPinToInterrupt(button), resetEEPROM, RISING); 
  }
  else {
    digitalWrite(11, LOW);   
    //attachInterrupt(digitalPinToInterrupt(button), resetEEPROM, FALLING); 
  }


  io_poll();                             // Modbus

  if (api == HIGH){                      // Logika pembacaan api sensor UVTron
    EEPROM.update(dataAddress, 1);       // Tulis '1' pada dataAddress
    condition = 1;                       // Jalankan condition 1
  }

  if (condition == 0){                   // Kondisi stand by atau tidak ada api
       digitalWrite(buzzer, LOW);        // Buzzer mati
       digitalWrite(green, HIGH);        // LED hijau nyala --aktif low
       digitalWrite(red, LOW);           // LED merah mati
//       lcd.setCursor(0,00);
//       lcd.print(rtc.getDateStr());     //prosedur pemanggilan tanggal 
//       lcd.setCursor(11,0);
//       lcd.print(rtc.getTimeStr());
       lcd.setCursor(0, 1);              // Set letak kursor LCD
       lcd.print("Tidak Ada Api");       // tulis LCD
   } 

  if (condition == 1){                   // maka
       digitalWrite(buzzer, HIGH);       // Buzzer bunyi
       digitalWrite(green, LOW);         // LED hijau mati
       digitalWrite(red, HIGH);          // LED merah nyala
//       lcd.setCursor(0,00);
//       lcd.print(rtc.getDateStr());      // Prosedur pemanggilan tanggal 
//       lcd.setCursor(11,0);
//       lcd.print(rtc.getTimeStr());
       lcd.setCursor(0, 1);               // Set letak kursor LCD
       lcd.print("Api Terdeteksi");       // tulis LCD
     }
   
}
