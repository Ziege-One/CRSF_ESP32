#include "crsf_esp32.h"   // Einbinden der CRSF-Bibliothek

/*
 * Beispielcode zur Nutzung der CRSF-ESP32 Bibliothek
 * Demonstriert: Initialisierung, Paketlesen, Parameterverwaltung und Geräteantworten
 */

// Beispiel-Variablen, die später über CRSF-Parameter geschrieben/gelesen werden können
uint8_t Variable1 = 2;
uint8_t Variable2 = 3;
uint8_t Variable3 = 4;
uint8_t Variable4 = 5;
uint8_t Variable5 = 6;
uint8_t Variable6 = 7;

// Erzeugen eines CRSF-Objekts
CRSF crsf;

// Task-Funktion zum kontinuierlichen Lesen von CRSF-Paketen
void read(void *pvParameters) {
    while (1) {
        crsf.read_packets(0);                  // Pakete einlesen und verarbeiten
        vTaskDelay(5 / portTICK_PERIOD_MS);    // kleine Pause, um CPU zu entlasten
    }
    vTaskDelete(NULL);                         // Task beenden (wird hier nie erreicht)
}

void setup() {
    Serial.begin(115200);                      // Debug-Serial starten

    pinMode(LED_BUILTIN, OUTPUT);              // Onboard-LED als Ausgang setzen
    digitalWrite(LED_BUILTIN, LOW);            // LED ausschalten

    // Initialisierung der seriellen Schnittstelle für CRSF
    // Unterschiedliche Pins je nach ESP32-Variante
#ifndef CONFIG_IDF_TARGET_ESP32C3
    crsf.init_crsf(&Serial1, 16, 17);          // RX=16, TX=17 für Standard-ESP32
#else
    crsf.init_crsf(&Serial1, 20, 21);          // RX=20, TX=21 für ESP32-C3
#endif

    // Task erstellen, um CRSF-Pakete im Hintergrund zu lesen (nur bei ESP32, nicht C3)
#ifndef CONFIG_IDF_TARGET_ESP32C3
    xTaskCreatePinnedToCore(
        read,          // Task-Funktion
        "read",        // Task-Name
        4096,          // Stackgröße
        NULL,          // Parameter
        1,             // Priorität
        NULL,          // Task-Handle
        1              // Core-ID
    );
#endif
}

void loop() {
#ifdef CONFIG_IDF_TARGET_ESP32C3
    crsf.read_packets(0);                      // Bei ESP32-C3 direkt im Loop lesen
#endif

    // Beispiel für Kanalausgabe (auskommentiert)
    // Serial.print("Channel 0: ");
    // Serial.println(channels[0]);
    // delay(100);

    // Geräte-Info anfragen/antworten
    if (crsf.getDeviceInfoReplyPending()) {
        crsf.send_device_info("ESP32-S3-TX", 11);
    }

    // Parameter lesen
    if (crsf.getDeviceReadReplyPending()) {
        handle_param_request(crsf.get_crfs_buffer(5));
    }

    // Parameter schreiben
    if (crsf.getDeviceWriteReplyPending()) {
        crsf.setDeviceWriteReplyPending(false);
        handle_param_write(crsf.get_crfs_buffer(5), crsf.get_crfs_buffer(6));
    }

    // Geräte-Kommandos verarbeiten
    if (crsf.getDeviceCommandReplyPending()) {
        crsf.setDeviceCommandReplyPending(false);
    }
}

// Verarbeitung von Schreibzugriffen auf Parameter
void handle_param_write(uint8_t id, uint8_t value) {
    Serial.println(id);
    Serial.println(value);
    switch (id) {
        case 1: Variable2 = value; break;
        case 5: Variable1 = value; break;
        case 7: Variable3 = value; break;
        case 8: Variable4 = value; break;
        case 9: Variable5 = value; break;
        case 10: Variable6 = value; break;
    }
}

// Verarbeitung von Leseanfragen auf Parameter
void handle_param_request(uint8_t id) {
    switch (id) {
        case 0: crsf.send_param_response_CRSF_FOLDER(id, 0, "ROOT",{1,2,3,4,5,9}); break;   // Root-Ordner mit Parametern
        case 1: crsf.send_param_response_CRSF_UINT8(id, 0, "Liter", Variable2, 0, 10, " ml"); break;
        case 2: crsf.send_param_response_CRSF_FOLDER(id, 0, "Ordner (ID2)", {8,9,6}); break;
        case 3: crsf.send_param_response_CRSF_FOLDER(id, 0, "Ordner (ID3)", {4}); break;
        case 4: crsf.send_param_response_CRSF_TEXT_SELECTION(id, 3, "BT Teleme 3", "Off;On", 0, 0, 1); break;
        case 5: crsf.send_param_response_CRSF_UINT8(id, 0, "Test Wert", Variable1, 0, 5, " kg123"); break;
        case 6: crsf.send_param_response_CRSF_INFO(id, 2, "Hallo", "Welt"); break;
        case 7: crsf.send_param_response_CRSF_UINT8(id, 0, "Test 2", Variable3, 0, 11, "kg"); break;
        case 8: crsf.send_param_response_CRSF_UINT8(id, 2, "Test 3", Variable4, 0, 12, "kg"); break;
        case 9: crsf.send_param_response_CRSF_UINT8(id, 2, "Test 4", Variable5, 0, 13, "kg"); break;
        case 10: crsf.send_param_response_CRSF_UINT8(id, 0, "Test 5", Variable6, 0, 14, "kg"); break;
        case 11: crsf.send_param_response_CRSF_TEXT_SELECTION(id, 0, "BT Teleme 6", "Off;On", 0, 0, 1); break;
        default: Serial.printf("❔ Unknown param ID %d\n", id); break;
    }

    // Weitere mögliche Antworten:
    // crsf.send_param_response_CRSF_UINT8(id, "Test Wert", 5, 0, 5, "kg");
    // crsf.send_param_response_CRSF_FLOAT(id, "Ziel", 1234, 0, 5000, 0, 2, 0.2, "°C");
    // crsf.send_param_response_CRSF_INFO(id, "Hallo", "Welt");
    // crsf.send_param_response_CRSF_FOLDER(id, "ordner", 0);
}