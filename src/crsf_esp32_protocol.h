#define CRSF_SYNC_byte 0xC8

//CRSF FRAMETYPE Stand 03.06.2025 https://github.com/crsf-wg/crsf/wiki/Packet-Types
#define CRSF_FRAMETYPE_GPS 0x02                         // GPS position, ground speed, heading, altitude, satellite count
#define CRSF_FRAMETYPE_GPS_TIME 0x03                    // This frame is needed for synchronization with the ublox time pulse. The maximum offset of time is +/-10ms.
#define CRSF_FRAMETYPE_GPS_Extended 0x06
#define CRSF_FRAMETYPE_VARIO 0x07                       // Vertical speed
#define CRSF_FRAMETYPE_BATTERY_SENSOR 0x08              // Battery voltage, current, mAh, remaining percent
#define CRSF_FRAMETYPE_BARO_ALTITUDE 0x09               // Barometric altitude, vertical speed (optional)
#define CRSF_FRAMETYPE_AIRSPEED 0x0A
#define CRSF_FRAMETYPE_HEARTBEAT 0x0B                   // (CRSFv3) Heartbeat
#define CRSF_FRAMETYPE_RPM 0x0C
#define CRSF_FRAMETYPE_TEMP 0x0D
#define CRSF_FRAMETYPE_VOLTAGES 0x0E
#define CRSF_FRAMETYPE_LINK_STATISTICS 0x14             // Signal information. Uplink/Downlink RSSI, SNR, Link Quality (LQ), RF mode, transmit power
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16          // Channels data (both handset to TX and RX to flight controller)
#define CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED 0x17   // (CRSFv3) Channels subset data
#define CRSF_FRAMETYPE_LINK_RX_ID 0x1C                  // Receiver RSSI percent, power?
#define CRSF_FRAMETYPE_LINK_TX_ID 0x1D                  // Transmitter RSSI percent, power, fps?
#define CRSF_FRAMETYPE_ATTITUDE 0x1E                    // Attitude: pitch, roll, yaw
#define CRSF_FRAMETYPE_FLIGHT_MODE 0x21                 // Flight controller flight mode string
    // Extended Header Frames, range: 0x28 to 0x96
#define CRSF_FRAMETYPE_DEVICE_PING 0x28                 // Sender requesting DEVICE_INFO from all destination devices
#define CRSF_FRAMETYPE_DEVICE_INFO 0x29                 // Device name, firmware version, hardware version, serial number (PING response)
#define CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY 0x2B    // Configuration item data chunk
#define CRSF_FRAMETYPE_PARAMETER_READ 0x2C              // Configuration item read request
#define CRSF_FRAMETYPE_PARAMETER_WRITE 0x2D             // Configuration item write request
#define CRSF_FRAMETYPE_ELRS_STATUS 0x2E                 // !!Non Standard!! ExpressLRS good/bad packet count, status flags
#define CRSF_FRAMETYPE_COMMAND 0x32                     // CRSF command execute
#define CRSF_FRAMETYPE_RADIO_ID 0x3A                    // Extended type used for OPENTX_SYNC
#define CRSF_FRAMETYPE_KISS_REQ 0x78                    // KISS request
#define CRSF_FRAMETYPE_KISS_RESP 0x79                   // KISS response
    // MSP commands
#define CRSF_FRAMETYPE_MSP_REQ 0x7A                     // MSP parameter request / command
#define CRSF_FRAMETYPE_MSP_RESP 0x7B                    // MSP parameter response chunk
#define CRSF_FRAMETYPE_MSP_WRITE 0x7C                   // 	MSP parameter write
#define CRSF_FRAMETYPE_DISPLAYPORT_CMD 0x7D             // (CRSFv3) MSP DisplayPort control command
#define CRSF_FRAMETYPE_ARDUPILOT_RESP 0x80              // Ardupilot output?


// CRSF ADDRESS Stand 03.06.2025 https://github.com/crsf-wg/crsf/wiki/CRSF-Addresses
#define CRSF_ADDRESS_BROADCAST 0x00           // Broadcast (all devices process packet)
#define CRSF_ADDRESS_USB 0x10                 // ?
#define CRSF_ADDRESS_BLUETOOTH 0x12           // Bluetooth module
#define CRSF_ADDRESS_TBS_CORE_PNP_PRO 0x80    // ?
#define CRSF_ADDRESS_RESERVED1 0x8A           // Reserved, for one
#define CRSF_ADDRESS_CURRENT_SENSOR 0xC0      // External current sensor
#define CRSF_ADDRESS_GPS 0xC2                 // External GPS
#define CRSF_ADDRESS_TBS_BLACKBOX 0xC4        // External Blackbox logging device
#define CRSF_ADDRESS_FLIGHT_CONTROLLER 0xC8   // Flight Controller (Betaflight / iNav)
#define CRSF_ADDRESS_RESERVED2 0xCA           // Reserved, for two
#define CRSF_ADDRESS_RACE_TAG 0xCC            // Race tag?
#define CRSF_ADDRESS_RADIO_TRANSMITTER 0xEA   // Handset (EdgeTX), not transmitter
#define CRSF_ADDRESS_CRSF_RECEIVER 0xEC       // Receiver hardware (TBS Nano RX / RadioMaster RP1)
#define CRSF_ADDRESS_CRSF_TRANSMITTER 0xEE    // Transmitter module, not handset
#define CRSF_ADDRESS_ELRS_LUA 0xEF            // !!Non-Standard!! Source address used by ExpressLRS Lua


// crsf_value_type
#define CRSF_UINT8 0x00
#define CRSF_INT8 0x01
#define CRSF_UINT16 0x02
#define CRSF_INT16 0x03
#define CRSF_UINT32 0x04
#define CRSF_INT32 0x05
#define CRSF_UINT64 0x06
#define CRSF_INT64 0x07
#define CRSF_FLOAT 0x08
#define CRSF_TEXT_SELECTION 0x09
#define CRSF_STRING 0x0A
#define CRSF_FOLDER 0x0B
#define CRSF_INFO 0x0C
#define CRSF_COMMAND 0x0D
#define CRSF_VTX 0x0F
#define CRSF_OUT_OF_RANGE 0x7F

struct LinkStatistics {
  uint8_t uplinkRSSI = 0;       // Sender → Empfänger RSSI (dBm)
  uint8_t uplinkLQ = 0;         // Link Quality (%)
  int8_t  uplinkSNR = 0;        // Signal-Rausch-Verhältnis (dB)

  uint8_t activeAntenna = 0;    // 0 = Ant1, 1 = Ant2, 2 = Diversity
  uint8_t rfMode = 0;           // RF Mode (z. B. 0 = 4Hz, 1 = 50Hz, …)
  uint8_t txPower = 0;          // Sendeleistung (mW)

  uint8_t downlinkRSSI = 0;     // Empfänger → Sender RSSI (dBm)
  uint8_t downlinkLQ = 0;       // Link Quality (%)
  int8_t  downlinkSNR = 0;      // Signal-Rausch-Verhältnis (dB)

  unsigned long lastUpdate = 0; // Zeitstempel des letzten Updates (ms)
};

struct DeviceInfo {
  char     deviceName[32];               // Variabel, max 31 Zeichen + Null
  uint32_t serialNumber;
  uint32_t hardwareID;
  uint32_t firmwareID;
  uint8_t  parametersTotal;
  uint8_t  parameterVersionNumber;
};