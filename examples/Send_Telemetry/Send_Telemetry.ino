#include "crsf_esp32.h"   // Einbinden der CRSF-Bibliothek

#define LED_BUILTIN 8
unsigned long lastSend = 0;

// Erzeugen eines CRSF-Objekts
CRSF crsf;

void setup() {
    Serial.begin(115200);                      // Debug-Serial starten

    pinMode(LED_BUILTIN, OUTPUT);              // Onboard-LED als Ausgang setzen
    digitalWrite(LED_BUILTIN, HIGH);            // LED ausschalten

    // Initialisierung der seriellen Schnittstelle für CRSF
    crsf.init_crsf(&Serial1, 20, 21);          // RX=20, TX=21 für ESP32-C3

}

void loop() {

    unsigned long now = millis();

    if (now - lastSend >= 300) {   // alle 300 ms
        lastSend = now;

        crsf.send_tele_GPS(524213670, 133560120, 250, 18000, 120, 12); // latitude 524213670 = 52°25'16.9"N; longitude 133560120 = 13°21'21.6"E, uint16_t groundspeed in km/h 250 = 25,0km/h, heading in ° 18000 = 180,00°, altitude in m 120m, satellites 12)
        
        //crsf.send_tele_Battery_Sensor(102, 77, 333, 22);    // voltage in V 102 = 10,2V ;current A 77 = 7,7A ; capacity_used in mA 333mA; remaining in % 22%

 		//crsf.send_tele_Vario(122);  // Vertical speed in m/s 122 = 1,22 m/s

        //crsf.send_tele_Baro_Altitude(1230, 144);  //High in m 1230 = 123,0m ; v_speed in cm/s 144 cm/s

        //crsf.send_tele_Airspeed(1456); // Airspeed in km/h  1456 = 145,6 km/h
         
        //crsf.send_tele_RPM(0, {15000, 14980, 15020, 12345}); // ID 0; RPM (revolutions per minute) 15000rpm

        //crsf.send_tele_Temp(1, {32, 34, 33}); //ID 1; Temp in°C 32 = 3,2°C

        //crsf.send_tele_Voltages(0, {4200, 4180, 4171});  // voltage_source_id 0–127 = Zellen, 128–255 = Sensor; Voltage_values[] bis zu 29 Werte, in V 4200 = 4,200V 

        //crsf.send_tele_Attitude(1500, -300, 0);  // pitch in rad 1500 = 0,15rad, roll -300 = -0,03rad, yaw 0 = 0,00rad

    }
}