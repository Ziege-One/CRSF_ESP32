# CRSF_ESP32

Eine leistungsstarke und leichtgewichtige **ESP32-Bibliothek** zur nativen Verarbeitung des **CRSF-Protokolls (Crossfire)**. Diese Bibliothek ermöglicht das vollständige Frame-Parsing von Steuerbefehlen sowie das Senden individueller Telemetriedaten.

## Features

- ⚡ **Echtzeit-Parsing:** Schnelle und effiziente Verarbeitung eingehender CRSF-Frames.
- 📡 **Telemetrie-Unterstützung:** Senden von Telemetriedaten (z. B. Akkuspannung, GPS, RSSI) zurück zum Sender.
- 🛠️ **Hardware-Serial-Kompatibilität:** Optimiert für die Verwendung der ESP32 Hardware-UARTs.
- 🧩 **Einfache Integration:** Modulare Struktur für den schnellen Einsatz in eigenen Projekten (z. B. Flight Controller, RC-Receiver, Custom-Projekte).

## Hardware-Anforderungen

- **Mikrocontroller:** ESP32 (S2, S3, C3 etc.)
- **Empfänger:** Jeder CRSF-kompatible RC-Empfänger (z. B. TBS Crossfire, ExpressLRS / ELRS)
- **Verbindung:** UART-Verbindung (TX/RX) zwischen ESP32 und dem Empfänger.

## Installation

### Über PlatformIO (Empfohlen)
Füge die Bibliothek zu deiner `platformio.ini` hinzu (sobald im Repository oder Registry registriert):
```ini
lib_deps =
    Ziege-One/CRSF_ESP32
```

### Über die Arduino IDE
1. Lade dieses Repository als `.zip`-Datei herunter.
2. Navigiere in der Arduino IDE zu **Sketch** -> **Bibliothek einbinden** -> **.ZIP-Bibliothek hinzufügen...**.
3. Wähle die heruntergeladene Datei aus.

## Schnelleinstieg (Beispiel)

Hier ist ein einfaches Beispiel, wie du die Kanäle des Empfängers auslesen kannst:

```cpp
#include <Arduino.h>
#include <CRSF_ESP32.h>

// Definiere die UART-Pins für den ESP32
#define CRSF_RX_PIN 16
#define CRSF_TX_PIN 17

CRSF_ESP32 crsf;

void setup() {
    Serial.begin(115200);
    
    // Initialisiere das CRSF-Protokoll auf Serial2
    crsf.begin(Serial2, CRSF_RX_PIN, CRSF_TX_PIN);
    Serial.println("CRSF Parser gestartet...");
}

void loop() {
    // Aktualisiere und verarbeite eingehende Daten
    crsf.update();

    if (crsf.isLinkUp()) {
        // Kanäle auslesen (1-basiert oder 0-basiert, je nach deiner API)
        int roll = crsf.getChannel(1);
        int pitch = crsf.getChannel(2);
        
        Serial.printf("Roll: %d | Pitch: %d\n", roll, pitch);
    }
    
    delay(10); // Kurze Pause zur Entlastung
}
```

*Hinweis: Weitere und detailliertere Beispiele findest du im Ordner `examples/`.*

## Lizenz

Dieses Projekt ist unter der **GPL-3.0-Lizenz** lizenziert. Weitere Details findest du in der [LICENSE](LICENSE) Datei.

## Beitragen (Contributing)

Beiträge, Fehlerberichte und Feature-Wünsche sind herzlich willkommen! 
1. Forke das Projekt.
2. Erstelle einen Feature-Branch.
3. Committe deine Änderungen.
4. Pushe den Branch.
5. Öffne einen Pull Request.
