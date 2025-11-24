#include "HEXBuilder.h"
#include "crsf_esp32.h"

HardwareSerial* serial_data;

LinkStatistics linkStats;

DeviceInfo info;

#define DEBUG_CRSF false
#define DEBUG_CRSF_TYPE false
#define DEBUG_CRSF_SEND true

CRSF::CRSF() {                  // Initialisierung
    deviceInfoReplyPending = 0; 
    deviceEntryReplyPending = 0;
    deviceReadReplyPending = 0;
    deviceWriteReplyPending = 0;
    deviceCommandReplyPending = 0;
}

unsigned char crc8tab[256] = {
    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9};

const uint8_t crc8_ba_tab[256] = {
    0x00, 0xBA, 0xB3, 0x09, 0xA7, 0x1D, 0x14, 0xAE, 0xD3, 0x69, 0x60, 0xDA, 0x74, 0xCE, 0xC7, 0x7D,
    0xAB, 0x11, 0x18, 0xA2, 0x0C, 0xB6, 0xBF, 0x05, 0x78, 0xC2, 0xCB, 0x71, 0xDF, 0x65, 0x6C, 0xD6,
    0x9D, 0x27, 0x2E, 0x94, 0x3A, 0x80, 0x89, 0x33, 0x4E, 0xF4, 0xFD, 0x47, 0xE9, 0x53, 0x5A, 0xE0,
    0x36, 0x8C, 0x85, 0x3F, 0x91, 0x2B, 0x22, 0x98, 0xE5, 0x5F, 0x56, 0xEC, 0x42, 0xF8, 0xF1, 0x4B,
    0x3B, 0x81, 0x88, 0x32, 0x9C, 0x26, 0x2F, 0x95, 0xE8, 0x52, 0x5B, 0xE1, 0x4F, 0xF5, 0xFC, 0x46,
    0x90, 0x2A, 0x23, 0x99, 0x37, 0x8D, 0x84, 0x3E, 0x43, 0xF9, 0xF0, 0x4A, 0xE4, 0x5E, 0x57, 0xED,
    0xA6, 0x1C, 0x15, 0xAF, 0x01, 0xBB, 0xB2, 0x08, 0x75, 0xCF, 0xC6, 0x7C, 0xD2, 0x68, 0x61, 0xDB,
    0x0D, 0xB7, 0xBE, 0x04, 0xAA, 0x10, 0x19, 0xA3, 0xDE, 0x64, 0x6D, 0xD7, 0x79, 0xC3, 0xCA, 0x70,
    0x76, 0xCC, 0xC5, 0x7F, 0xD1, 0x6B, 0x62, 0xD8, 0xA5, 0x1F, 0x16, 0xAC, 0x02, 0xB8, 0xB1, 0x0B,
    0xDD, 0x67, 0x6E, 0xD4, 0x7A, 0xC0, 0xC9, 0x73, 0x0E, 0xB4, 0xBD, 0x07, 0xA9, 0x13, 0x1A, 0xA0,
    0xEB, 0x51, 0x58, 0xE2, 0x4C, 0xF6, 0xFF, 0x45, 0x38, 0x82, 0x8B, 0x31, 0x9F, 0x25, 0x2C, 0x96,
    0x40, 0xFA, 0xF3, 0x49, 0xE7, 0x5D, 0x54, 0xEE, 0x93, 0x29, 0x20, 0x9A, 0x34, 0x8E, 0x87, 0x3D,
    0x4D, 0xF7, 0xFE, 0x44, 0xEA, 0x50, 0x59, 0xE3, 0x9E, 0x24, 0x2D, 0x97, 0x39, 0x83, 0x8A, 0x30,
    0xE6, 0x5C, 0x55, 0xEF, 0x41, 0xFB, 0xF2, 0x48, 0x35, 0x8F, 0x86, 0x3C, 0x92, 0x28, 0x21, 0x9B,
    0xD0, 0x6A, 0x63, 0xD9, 0x77, 0xCD, 0xC4, 0x7E, 0x03, 0xB9, 0xB0, 0x0A, 0xA4, 0x1E, 0x17, 0xAD,
    0x7B, 0xC1, 0xC8, 0x72, 0xDC, 0x66, 0x6F, 0xD5, 0xA8, 0x12, 0x1B, 0xA1, 0x0F, 0xB5, 0xBC, 0x06};
// Calculate checksum
uint8_t crc8(const uint8_t * ptr, uint8_t len)
{
  uint8_t crc = 0;
  for (uint8_t i=0; i<len; i++)
      crc = crc8tab[crc ^ *ptr++];
  return crc;
}

uint8_t crc8_ba(const uint8_t* ptr, size_t len, uint8_t init = 0x00) {
    uint8_t crc = init;
    for (size_t i = 0; i < len; ++i) {
        crc = crc8_ba_tab[crc ^ ptr[i]];
    }
    return crc;
}

// Extract channels from CRSF buffer
void CRSF::updateChannels() {
      channels[0] = ((crfs_buffer[3] | crfs_buffer[4] << 8) & CHANNEL_MASK);
      channels[1] = ((crfs_buffer[4] >> 3 | crfs_buffer[5] << 5) & CHANNEL_MASK);
      channels[2] = ((crfs_buffer[5] >> 6 | crfs_buffer[6] << 2 | crfs_buffer[7] << 10) & CHANNEL_MASK);
      channels[3] = ((crfs_buffer[7] >> 1 | crfs_buffer[8] << 7) & CHANNEL_MASK);
      channels[4] = ((crfs_buffer[8] >> 4 | crfs_buffer[9] << 4) & CHANNEL_MASK);
      channels[5] = ((crfs_buffer[9] >> 7 | crfs_buffer[10] << 1 | crfs_buffer[11] << 9) & CHANNEL_MASK);
      channels[6] = ((crfs_buffer[11] >> 2 | crfs_buffer[12] << 6) & CHANNEL_MASK);
      channels[7] = ((crfs_buffer[12] >> 5 | crfs_buffer[13] << 3) & CHANNEL_MASK);
      channels[8] = ((crfs_buffer[14] | crfs_buffer[15] << 8) & CHANNEL_MASK);
      channels[9] = ((crfs_buffer[15] >> 3 | crfs_buffer[16] << 5) & CHANNEL_MASK);
      channels[10] = ((crfs_buffer[16] >> 6 | crfs_buffer[17] << 2 | crfs_buffer[18] << 10) & CHANNEL_MASK);
      channels[11] = ((crfs_buffer[18] >> 1 | crfs_buffer[19] << 7) & CHANNEL_MASK);
      channels[12] = ((crfs_buffer[19] >> 4 | crfs_buffer[20] << 4) & CHANNEL_MASK);
      channels[13] = ((crfs_buffer[20] >> 7 | crfs_buffer[21] << 1 | crfs_buffer[22] << 9) & CHANNEL_MASK);
      channels[14] = ((crfs_buffer[22] >> 2 | crfs_buffer[23] << 6) & CHANNEL_MASK);
      channels[15] = ((crfs_buffer[23] >> 5 | crfs_buffer[24] << 3) & CHANNEL_MASK);
}

void CRSF::updateLink_Statistics() {
    linkStats.uplinkRSSI     = crfs_buffer[3];
    linkStats.uplinkLQ       = crfs_buffer[4];
    linkStats.uplinkSNR      = (int8_t)crfs_buffer[5];
    linkStats.activeAntenna  = crfs_buffer[6];
    linkStats.rfMode         = crfs_buffer[7];
    linkStats.txPower        = crfs_buffer[8];
    linkStats.downlinkRSSI   = crfs_buffer[9];
    linkStats.downlinkLQ     = crfs_buffer[10];
    linkStats.downlinkSNR    = (int8_t)crfs_buffer[11];
    linkStats.lastUpdate     = millis();

/*
    Serial.println("📡 LinkStatistics empfangen:");
    Serial.printf("  ➤ Uplink     : RSSI %ddBm | LQ %d%% | SNR %ddB\n", linkStats.uplinkRSSI, linkStats.uplinkLQ, linkStats.uplinkSNR);
    Serial.printf("  ➤ Downlink   : RSSI %ddBm | LQ %d%% | SNR %ddB\n", linkStats.downlinkRSSI, linkStats.downlinkLQ, linkStats.downlinkSNR);
    Serial.printf("  ➤ TX Power   : %dmW\n", linkStats.txPower);
    Serial.printf("  ➤ RF Mode    : %d\n", linkStats.rfMode);
    Serial.printf("  ➤ Antenne    : %d\n", linkStats.activeAntenna);
    Serial.printf("  ➤ Letztes Update: %lu ms\n", linkStats.lastUpdate);
*/    
}

void CRSF::updateDevice_Info() {

uint8_t frameLength = crfs_buffer[1];         // Länge ab crfs_buffer[2]
uint8_t payloadLength = frameLength - 1; // ohne Typ-Byte
uint8_t fixedFields = 4 * 3 + 2;         // 3 × uint32_t + 2 × uint8_t = 14
uint8_t nameLength = payloadLength - fixedFields-1;

if (nameLength > 31) nameLength = 31;   // Sicherheit

memcpy(info.deviceName, &crfs_buffer[5], nameLength);
info.deviceName[nameLength] = '\0';     // Null-terminieren

uint8_t i = 3 + nameLength;
info.serialNumber           = (crfs_buffer[i] << 24) | (crfs_buffer[i+1] << 16) | (crfs_buffer[i+2] << 8) | crfs_buffer[i+3];
info.hardwareID             = (crfs_buffer[i+4] << 24) | (crfs_buffer[i+5] << 16) | (crfs_buffer[i+6] << 8) | crfs_buffer[i+7];
info.firmwareID             = (crfs_buffer[i+8] << 24) | (crfs_buffer[i+9] << 16) | (crfs_buffer[i+10] << 8) | crfs_buffer[i+11];
info.parametersTotal        = crfs_buffer[i+12];
info.parameterVersionNumber = crfs_buffer[i+13];
/*
Serial.println("📦 Erweiterte Device Info:");
Serial.printf("  ➤ Name             : %s (%d Bytes)\n", info.deviceName, nameLength);
Serial.printf("  ➤ Serial Number    : %08X\n", info.serialNumber);
Serial.printf("  ➤ Hardware ID      : %08X\n", info.hardwareID);
Serial.printf("  ➤ Firmware ID      : %08X\n", info.firmwareID);
Serial.printf("  ➤ Parameter Count  : %d\n", info.parametersTotal);
Serial.printf("  ➤ Param Version    : %d\n", info.parameterVersionNumber);
*/
}

void CRSF::setDeviceInfoReplyPending(int newValue) {
    deviceInfoReplyPending = newValue;
}

void CRSF::setDeviceEntryReplyPending(int newValue) {
    deviceEntryReplyPending = newValue;
}

void CRSF::setDeviceReadReplyPending(int newValue) {
    deviceReadReplyPending = newValue;
}

void CRSF::setDeviceWriteReplyPending(int newValue) {
    deviceWriteReplyPending = newValue;
}

void CRSF::setDeviceCommandReplyPending(int newValue) {
    deviceCommandReplyPending = newValue;
}

void CRSF::set_crsf_channel(uint8_t ch, uint16_t value) {
    if (ch < CRSF_MAX_CHANNELS) 
        channels[ch] = value;
}

void CRSF::init_crsf(HardwareSerial *serialPort, uint8_t rxPin = 16, uint8_t txPin = 17) {
  serial_data = &Serial1;
  serial_data->begin(BAUD_RATE, SERIAL_8N1, rxPin, txPin);
#if DEBUG_CRSF
  Serial.println("Initializing CRSF...");
#endif  
}

void CRSF::print_channels() {
    Serial.print("Channels: ");
    for (int i = 0; i < CRSF_MAX_CHANNELS; i++) {
        Serial.print(channels[i]);
        Serial.print(" ");
    }
    Serial.println();
}

// New packet CRSF Data
void CRSF::crsfDataReceive() {
    uint8_t TYPE = crfs_buffer[2];

    switch (TYPE) {
        case CRSF_FRAMETYPE_LINK_STATISTICS:
            updateLink_Statistics();
#if DEBUG_CRSF_TYPE        
            //Serial.println("📤 CRSF_FRAMETYPE_LINK_STATISTICS");
#endif             
            break;

        case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
            updateChannels();
#if DEBUG_CRSF_TYPE
            //Serial.println("📤 CRSF_FRAMETYPE_RC_CHANNELS_PACKED");
#endif             
            break;

        case CRSF_FRAMETYPE_DEVICE_PING:
            deviceInfoReplyPending = true; 
#if DEBUG_CRSF_TYPE      
            Serial.print("📤 CRSF_FRAMETYPE_DEVICE_PING Destination: ");
            Serial.print(crfs_buffer[3], HEX);
            Serial.print(" Source: ");
            Serial.println(crfs_buffer[4], HEX);
#endif             
            break;   

        case CRSF_FRAMETYPE_DEVICE_INFO:
            deviceInfoReplyPending = true;       
            //Serial.print("📤 CRSF_FRAMETYPE_DEVICE_INFO Destination: ");
            updateDevice_Info();
            break;  

        case CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY:
            deviceEntryReplyPending = true;
#if DEBUG_CRSF_TYPE 
            Serial.println("📤 CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY Destination: ");
            Serial.print(crfs_buffer[3], HEX);
            Serial.print(" Source: ");
            Serial.print(crfs_buffer[4], HEX);
            Serial.print(" ID: ");
            Serial.println(crfs_buffer[5]);
#endif             
            break; 

        case CRSF_FRAMETYPE_PARAMETER_READ:
            deviceReadReplyPending = true;
#if DEBUG_CRSF_TYPE 
            Serial.print("📤 CRSF_FRAMETYPE_PARAMETER_READ Destination: ");
            Serial.print(crfs_buffer[3], HEX);
            Serial.print(" Source: ");
            Serial.print(crfs_buffer[4], HEX);
            Serial.print(" ID: ");
            Serial.println(crfs_buffer[5]);
#endif             
            break; 

        case CRSF_FRAMETYPE_PARAMETER_WRITE:
            deviceWriteReplyPending = true;
#if DEBUG_CRSF_TYPE 
            Serial.print("📤 CRSF_FRAMETYPE_PARAMETER_WRITE Destination: ");
            Serial.print(crfs_buffer[3], HEX);
            Serial.print(" Source: ");
            Serial.print(crfs_buffer[4], HEX);
            Serial.print(" ID: ");
            Serial.print(crfs_buffer[5], HEX);
            Serial.print(" Value: ");
            Serial.println(crfs_buffer[6], HEX);
#endif             
            break;    

        case CRSF_FRAMETYPE_COMMAND:
            deviceCommandReplyPending = true;
#if DEBUG_CRSF_TYPE 
            Serial.println("📤 CRSF_FRAMETYPE_COMMAND");
#endif             
            break;    
         
    }
}

void CRSF::send_packets(byte* buffer, size_t length, uint8_t debug) {
    if (debug) 
    {
        Serial.print("TX-Buffer: ");
        for (size_t i = 0; i < length; i++) {
            Serial.print(buffer[i], HEX); // Ausgabe in Hexadezimalformat
            Serial.print(" ");
        }
        Serial.println("");
    }
    serial_data->write(buffer, length);
}

// Read CRSF packets from serial port
void CRSF::read_packets(uint8_t debug) {
    uint8_t data, length, byte_index = 0;

    while (serial_data->available() > 0) {
        data = serial_data->read();

        if ((data != CRSF_SYNC_byte && data != CRSF_ADDRESS_RADIO_TRANSMITTER) && byte_index == 0) {
            continue; 
        } 

        if ((data == CRSF_SYNC_byte || data == CRSF_ADDRESS_RADIO_TRANSMITTER) && byte_index == 0) {
            crfs_buffer[byte_index++] = data;
            continue;
        }

        if (byte_index == 1) {
            length = data;
            crfs_buffer[byte_index++] = data;
            continue;
        }

        if (byte_index == length + 1) {
            crfs_buffer[byte_index++] = data;
            if (data == crc8(&crfs_buffer[2], length - 1)) {
                crfs_buffer[byte_index++] = data;
                crsfDataReceive();
                if (debug) {
                    Serial.print("RX-Buffer: ");
                    for (size_t i = 0; i < length; i++) {
                        Serial.print(crfs_buffer[i], HEX); // Ausgabe in Hexadezimalformat
                        Serial.print(" ");
                    }
                    Serial.println("");
                }
            } else {
                Serial.println("CRC Error");
            }
            byte_index = 0;
            continue;
        }
        if (byte_index < CRSF_PACKET_SIZE) {
            crfs_buffer[byte_index++] = data;
        } else {
            byte_index = 0;
        }
    }
}

void CRSF::send_rc_channels_packed() {

    // const uint8_t crc = crsf_crc8(&packet[2], CRSF_PACKET_SIZE-3);
    /*
     * Map 1000-2000 with middle at 1500 chanel values to
     * 173-1811 with middle at 992 S.BUS protocol requires
    */

    uint8_t len = 24;  
    uint8_t packet[64];
    // packet[0] = UART_SYNC; //Header
    packet[0] = CRSF_ADDRESS_CRSF_TRANSMITTER;   // sync ? OpenTX/EdgeTX sends the channels packet starting with 0xEE instead of 0xC8
    packet[1] = len;           // len
    packet[2] = CRSF_FRAMETYPE_RC_CHANNELS_PACKED;  // type
    packet[3] = (uint8_t)(channels[0] & 0x07FF);
    packet[4] = (uint8_t)((channels[0] & 0x07FF) >> 8 | (channels[1] & 0x07FF) << 3);
    packet[5] = (uint8_t)((channels[1] & 0x07FF) >> 5 | (channels[2] & 0x07FF) << 6);
    packet[6] = (uint8_t)((channels[2] & 0x07FF) >> 2);
    packet[7] = (uint8_t)((channels[2] & 0x07FF) >> 10 | (channels[3] & 0x07FF) << 1);
    packet[8] = (uint8_t)((channels[3] & 0x07FF) >> 7 | (channels[4] & 0x07FF) << 4);
    packet[9] = (uint8_t)((channels[4] & 0x07FF) >> 4 | (channels[5] & 0x07FF) << 7);
    packet[10] = (uint8_t)((channels[5] & 0x07FF) >> 1);
    packet[11] = (uint8_t)((channels[5] & 0x07FF) >> 9 | (channels[6] & 0x07FF) << 2);
    packet[12] = (uint8_t)((channels[6] & 0x07FF) >> 6 | (channels[7] & 0x07FF) << 5);
    packet[13] = (uint8_t)((channels[7] & 0x07FF) >> 3);
    packet[14] = (uint8_t)((channels[8] & 0x07FF));
    packet[15] = (uint8_t)((channels[8] & 0x07FF) >> 8 | (channels[9] & 0x07FF) << 3);
    packet[16] = (uint8_t)((channels[9] & 0x07FF) >> 5 | (channels[10] & 0x07FF) << 6);
    packet[17] = (uint8_t)((channels[10] & 0x07FF) >> 2);
    packet[18] = (uint8_t)((channels[10] & 0x07FF) >> 10 | (channels[11] & 0x07FF) << 1);
    packet[19] = (uint8_t)((channels[11] & 0x07FF) >> 7 | (channels[12] & 0x07FF) << 4);
    packet[20] = (uint8_t)((channels[12] & 0x07FF) >> 4 | (channels[13] & 0x07FF) << 7);
    packet[21] = (uint8_t)((channels[13] & 0x07FF) >> 1);
    packet[22] = (uint8_t)((channels[13] & 0x07FF) >> 9 | (channels[14] & 0x07FF) << 2);
    packet[23] = (uint8_t)((channels[14] & 0x07FF) >> 6 | (channels[15] & 0x07FF) << 5);
    packet[24] = (uint8_t)((channels[15] & 0x07FF) >> 3);

    packet[25] = crc8(&packet[2], packet[1] - 1); // CRC

    send_packets(packet, len + 2, 0);   //Len + 2
    //Serial.println("📤 RC Channels Packed sent");
}

void CRSF::send_ping() {

    uint8_t len = 4;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_DEVICE_PING;   // type
    packet[3] = CRSF_ADDRESS_BROADCAST; 
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    
    // Frame CRC8 (Polynom 0xD5) über [type ... Command_CRC8]
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);

    //Serial.println("📤 Ping sent");
}
 

void CRSF::send_device_info(const char* name, uint8_t parameter) {

    deviceInfoReplyPending = false;

    uint8_t len_name = strlen(name) + 1; // len + null terminator

    uint8_t len = 14 + len_name + 4; // + 4 = "ELRS" for ExpressLRS
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_DEVICE_INFO;   // type
    packet[3] = CRSF_ADDRESS_RADIO_TRANSMITTER; 
    packet[4] = CRSF_ADDRESS_FLIGHT_CONTROLLER;

    strcpy((char*)&packet[5], name); // Name
    strcpy((char*)&packet[5 + len_name],"ELRS"); //always "ELRS" for ExpressLRS

    packet[5 + len_name + 4] = 0x00; // hardware version 1
    packet[6 + len_name + 4] = 0x00; // hardware version 2
    packet[7 + len_name + 4] = 0x00; // hardware version 3
    packet[8 + len_name + 4] = 0x00; // hardware version 4

    packet[9 + len_name + 4] = 0x00; // software version 1
    packet[10 + len_name + 4] = 0x00; // software version 2
    packet[11 + len_name + 4] = 0x00; // software version 3
    packet[12 + len_name + 4] = 0x00; // software version 4

    packet[13 + len_name + 4] = parameter; // number of config parameters
    packet[14 + len_name + 4] = 0x00; // parameter protocol version

    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2, 0);   //Len + 2

#if DEBUG_CRSF_SEND
    Serial.println("📤 Device Info sent");
#endif    
}

void CRSF::send_param_response_CRSF_UINT8(uint8_t param_id, uint8_t parent, const char* name, uint8_t val, uint8_t min, uint8_t max, const char* unit) { 

    deviceReadReplyPending = false;

    uint8_t len_name = strlen(name) + 1; // len + null terminator
    uint8_t len_unit = strlen(unit) + 1; // len + null terminator

    uint8_t len = 11 + len_name + len_unit;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY;   // type
    packet[3] = CRSF_ADDRESS_RADIO_TRANSMITTER; 
    packet[4] = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    packet[5] = param_id; //config field index, parameter field 03
    packet[6] = 0x00; // chunks remaining, 0 chunks remaining after this one
    
    packet[7] = parent; // Parent folder
    packet[8] = CRSF_UINT8; // Data type

    strcpy((char*)&packet[9], name); // Name

    packet[9 + len_name] = val;     // Value
    packet[10 + len_name] = min;    // min Value
    packet[11 + len_name] = max;    // max Value

    strcpy((char*)&packet[12 + len_name], unit); // Name
    
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2, 0);

#if DEBUG_CRSF_SEND    
    Serial.print("📤 Device Parameter UINT8 sent ID: "); 
    Serial.print(param_id);
    Serial.print(" parent: ");
    Serial.print(parent);
    Serial.print(" Val: ");
    Serial.println(val);
#endif    
}

void CRSF::send_param_response_CRSF_FLOAT(uint8_t param_id, uint8_t parent, const char* name, uint32_t val, uint32_t min, uint32_t max, uint32_t default_val, uint8_t decimal_point, uint32_t step_size, const char* unit) {

    deviceReadReplyPending = false;

    uint8_t len_name = strlen(name) + 1; // len + null terminator
    uint8_t len_unit = strlen(unit) + 1; // len + null terminator

    uint8_t len = 29 + len_name + len_unit;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY;   // type
    packet[3] = CRSF_ADDRESS_RADIO_TRANSMITTER; 
    packet[4] = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    packet[5] = param_id; //config field index, parameter field 03
    packet[6] = 0x00; // chunks remaining, 0 chunks remaining after this one
    
    packet[7] = parent; // Parent folder
    packet[8] = CRSF_FLOAT; // Data type

    strcpy((char*)&packet[9], name); // Name

    packet[9 + len_name] = (val & 0xFF);            // Value 1
    packet[10 + len_name] = ((val >> 8) & 0xFF);    // Value 2
    packet[11 + len_name] = ((val >> 16) & 0xFF);   // Value 3
    packet[12 + len_name] = ((val >> 24) & 0xFF);   // Value 4

    packet[13 + len_name] = (min & 0xFF);           // min Value 1
    packet[14 + len_name] = ((min >> 8) & 0xFF);    // min Value 2
    packet[14 + len_name] = ((min >> 16) & 0xFF);   // min Value 3
    packet[16 + len_name] = ((min >> 24) & 0xFF);   // min Value 4

    packet[17 + len_name] = (max & 0xFF);           // max Value 1
    packet[18 + len_name] = ((max >> 8) & 0xFF);    // max Value 2
    packet[19 + len_name] = ((max >> 16) & 0xFF);   // max Value 3
    packet[20 + len_name] = ((max >> 24) & 0xFF);   // max Value 4

    packet[21 + len_name] = (default_val & 0xFF);           // default Value 1
    packet[22 + len_name] = ((default_val >> 8) & 0xFF);    // default Value 2
    packet[23 + len_name] = ((default_val >> 16) & 0xFF);   // default Value 3
    packet[24 + len_name] = ((default_val >> 24) & 0xFF);   // default Value 4    

    packet[25 + len_name] = decimal_point;   // Decimal_point

    packet[26 + len_name] = (step_size & 0xFF);           // Step Size Value 1
    packet[27 + len_name] = ((step_size >> 8) & 0xFF);    // Step Size Value 2
    packet[28 + len_name] = ((step_size >> 16) & 0xFF);   // Step Size Value 3
    packet[29 + len_name] = ((step_size >> 24) & 0xFF);   // Step Size Value 4  

    strcpy((char*)&packet[30 + len_name], unit); // Name
    
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2, 0);

#if DEBUG_CRSF_SEND
    Serial.print("📤 Device Parameter FLOAT sent ID: "); 
    Serial.print(param_id);
    Serial.print(" parent: ");
    Serial.print(parent);
    Serial.print(" Val: ");
    Serial.println(val);
#endif    
}

void CRSF::send_param_response_CRSF_TEXT_SELECTION(uint8_t param_id, uint8_t parent, const char* name, const char* options, uint8_t val, uint8_t min, uint8_t max) {

    deviceReadReplyPending = false;

    uint8_t len_name = strlen(name) + 1; // len + null terminator
    uint8_t len_options = strlen(options) + 1; // len + null terminator

    uint8_t len = 13 + len_name + len_options;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY;   // type
    packet[3] = CRSF_ADDRESS_RADIO_TRANSMITTER; 
    packet[4] = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    packet[5] = param_id; //crfs_buffer[5]; //config field index, parameter field 03
    packet[6] = 0x00; // chunks remaining, 0 chunks remaining after this one
    
    packet[7] = parent; // Parent folder
    packet[8] = CRSF_TEXT_SELECTION; // Data type

    strcpy((char*)&packet[9], name); // Name
    strcpy((char*)&packet[9 + len_name],options); //Option

    packet[9 + len_name + len_options] = val; // Value
    packet[10 + len_name + len_options] = min; // min
    packet[11 + len_name + len_options] = max; // max
    packet[12 + len_name + len_options] = 0x00; // default
    packet[13 + len_name + len_options] = 0x00; // units (null-terminated string)
    
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2, 0);

#if DEBUG_CRSF_SEND
    Serial.print("📤 Device Parameter TEXT_SELECTION sent ID: ");
    Serial.print(param_id);
    Serial.print(" parent: ");
    Serial.print(parent);
    Serial.print(" Val: ");
    Serial.println(val);
#endif    
}

void CRSF::send_param_response_CRSF_STRING(uint8_t param_id, uint8_t parent, const char* name, const char* value, uint8_t val, uint8_t max_length) {

    deviceReadReplyPending = false;

    uint8_t len_name = strlen(name) + 1; // len + null terminator
    uint8_t len_value = strlen(value) + 1; // len + null terminator

    uint8_t len = 9 + len_name + len_value;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY;   // type
    packet[3] = CRSF_ADDRESS_RADIO_TRANSMITTER; 
    packet[4] = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    packet[5] = param_id; //config field index, parameter field 03
    packet[6] = 0x00; // chunks remaining, 0 chunks remaining after this one
    
    packet[7] = parent; // Parent folder
    packet[8] = CRSF_STRING; // Data type

    strcpy((char*)&packet[9], name); // Name
    strcpy((char*)&packet[9 + len_name],value); //Option

    packet[9 + len_name + len_value] = 0x00; // String_max_length
    
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2, 0);

#if DEBUG_CRSF_SEND
    Serial.println("📤 Device Parameter STRING sent");
#endif    
}

void CRSF::send_param_response_CRSF_FOLDER(uint8_t param_id, uint8_t parent, const char* name, std::initializer_list<uint8_t> children) {

    deviceReadReplyPending = false;

    uint8_t len_name = strlen(name) + 1; // len + null terminator
    uint8_t len_children = children.size();

    uint8_t len = 9 + len_name + len_children;    
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY;   // type
    packet[3] = CRSF_ADDRESS_RADIO_TRANSMITTER; 
    packet[4] = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    packet[5] = param_id; //config field index, parameter field 03
    packet[6] = 0x00; // chunks remaining, 0 chunks remaining after this one
    
    packet[7] = parent; // Parent folder
    packet[8] = CRSF_FOLDER; // Data type

    strcpy((char*)&packet[9], name); // Name

    // List_of_children
    int packet_count = 9;
    for (uint8_t c : children) {
        packet[packet_count + len_name] = c; 
        packet_count ++;
    }
    
    //packet[packet_count + len_name] = 0xFF;        // children Liste mit 0xFF abschließen
    packet[9 + len_name + len_children] = 0xFF;        // children Liste mit 0xFF abschließen ????

    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2, 0);

#if DEBUG_CRSF_SEND
    Serial.println("📤 Device Parameter FOLDER sent");
#endif    
}

void CRSF::send_param_response_CRSF_INFO(uint8_t param_id, uint8_t parent, const char* name, const char* info) {

    deviceReadReplyPending = false;

    uint8_t len_name = strlen(name) + 1; // len + null terminator
    uint8_t len_info = strlen(info) + 1; // len + null terminator

    uint8_t len = 8 + len_name + len_info;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY;   // type
    packet[3] = CRSF_ADDRESS_RADIO_TRANSMITTER; 
    packet[4] = CRSF_ADDRESS_FLIGHT_CONTROLLER;
    packet[5] = param_id; //config field index, parameter field 03
    packet[6] = 0x00; // chunks remaining, 0 chunks remaining after this one
    
    packet[7] = parent; // Parent folder
    packet[8] = CRSF_INFO; // Data type

    strcpy((char*)&packet[9], name); // Name
    strcpy((char*)&packet[9 + len_name], info); // Info
    
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);

#if DEBUG_CRSF_SEND
    Serial.print("📤 Device Parameter INFO sent ID: ");
    Serial.print(param_id);
    Serial.print(" parent: ");
    Serial.println(parent);
#endif    
}

void CRSF::send_command_MWSET(uint8_t adress, uint8_t state) {

    uint8_t len = 9;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_COMMAND;   // type
    packet[3] = CRSF_ADDRESS_FLIGHT_CONTROLLER; 
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    packet[5] = 0xa1; // Commands for MultiSwitch (realm 0xa1)
    packet[6] = 0x01; // set 8 binary-switches

    packet[7] = adress; // switch-address (0 …​ 255)
    packet[8] = state;  // state of 8 binary switches
    
    // Command_CRC8 (Polynom 0xBA) über [Frame Type (0x32)] + [Destination] + [Origin] + [Command ID] + [Payload]
    packet[9] = crc8_ba(&packet[2], len - 2);
    
    // Frame CRC8 (Polynom 0xD5) über [type ... Command_CRC8]
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);
    Serial.println("📤 Command MWSET sent");
}

void CRSF::send_command_MWSET4(uint8_t adress, uint8_t stateH, uint8_t stateL) {

    uint8_t len = 10;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_COMMAND;   // type
    packet[3] = CRSF_ADDRESS_FLIGHT_CONTROLLER; 
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    packet[5] = 0xa1; // Commands for MultiSwitch (realm 0xa1)
    packet[6] = 0x07; // set 8 4-state switches (2-bits for each switch → 2 bytes payload)

    packet[7] = adress; // switch-address (0 …​ 255)
    packet[8] = stateH; // state of 4 2-ary switches
    packet[9] = stateL; // state of 4 2-ary switches 

    // Command_CRC8 (Polynom 0xBA) über [Frame Type (0x32)] + [Destination] + [Origin] + [Command ID] + [Payload]
    packet[10] = crc8_ba(&packet[2], len - 2);
    
    // Frame CRC8 (Polynom 0xD5) über [type ... Command_CRC8]
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);

    //Serial.println("📤 Command MWSET4 sent");
    //Serial.println(adress);
    //Serial.println(stateH);
    //Serial.println(stateL);
    //Serial.println("---");
}

void CRSF::send_command_MWSET4M(uint8_t adress_1, uint8_t stateH_1, uint8_t stateL_1, uint8_t adress_2, uint8_t stateH_2, uint8_t stateL_2) {

    uint8_t len = 14;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_COMMAND;   // type
    packet[3] = CRSF_ADDRESS_FLIGHT_CONTROLLER; 
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    packet[5] = 0xa1; // Commands for MultiSwitch (realm 0xa1)
    packet[6] = 0x09; // set 8 4-state switches (2-bits for each switch → 2 bytes payload)

    packet[7] = 2; // number of following 3-byte sequences
    packet[8] = adress_2; // switch-address (0 …​ 255)
    packet[9] = stateH_2; // state of 4 2-ary switches
    packet[10] = stateL_2; // state of 4 2-ary switches
    packet[11] = adress_1; // switch-address (0 …​ 255)
    packet[12] = stateH_1; // state of 4 2-ary switches
    packet[13] = stateL_1; // state of 4 2-ary switches    

    // Command_CRC8 (Polynom 0xBA) über [Frame Type (0x32)] + [Destination] + [Origin] + [Command ID] + [Payload]
    packet[14] = crc8_ba(&packet[2], len - 2);
    
    // Frame CRC8 (Polynom 0xD5) über [type ... Command_CRC8]
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);

    //Serial.println("📤 Command MWSET4M sent");
    //Serial.println(adress_1);
    //Serial.println(stateH_1);
    //Serial.println(stateL_1);
    //Serial.println("---");
    //Serial.println(adress_2);
    //Serial.println(stateH_2);
    //Serial.println(stateL_2);
}

void CRSF::send_command_MWPROP(uint8_t adress, uint8_t channel, uint8_t duty ) {

    uint8_t len = 9;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_COMMAND;   // type
    packet[3] = CRSF_ADDRESS_FLIGHT_CONTROLLER; 
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    packet[5] = 0xa1; // Commands for MultiSwitch (realm 0xa1)
    packet[6] = 0x02; // set a proportional value (8-bit resolution)

    packet[7] = adress;  // switch-address (0 …​ 255)
    packet[8] = channel; // number of the output channel
    packet[9] = duty;    // duty for the output channel
    
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);
    //Serial.println("📤 Command MWPROP sent");
    //Serial.println(adress);
    //Serial.println(channel);
    //Serial.println(duty);
}

void CRSF::send_param(uint8_t parameter_number, uint8_t data) {

    uint8_t len = 6;
    uint8_t packet[64];
    packet[0] = CRSF_ADDRESS_CRSF_TRANSMITTER;   // sync ? OpenTX/EdgeTX sends the channels packet starting with 0xEE instead of 0xC8
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_WRITE;
    packet[3] = CRSF_ADDRESS_CRSF_TRANSMITTER;
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    packet[5] = parameter_number;
    packet[6] = data;
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);

#if DEBUG_CRSF_SEND
    Serial.println("📤 Paramenter sent");
#endif     
}

void CRSF::read_param(uint8_t parameter_number, uint8_t parameter_chunk_number){

    uint8_t len = 6;
    uint8_t packet[64];
    packet[0] = CRSF_ADDRESS_CRSF_TRANSMITTER;   // sync ? OpenTX/EdgeTX sends the channels packet starting with 0xEE instead of 0xC8
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_PARAMETER_READ;
    packet[3] = CRSF_ADDRESS_CRSF_TRANSMITTER;
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    packet[5] = parameter_number;
    packet[6] = parameter_chunk_number;
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

    send_packets(packet, len + 2 , 0);

#if DEBUG_CRSF_SEND
    Serial.println("📤 Paramenter read");
#endif     
}

void send_command(uint8_t command_id, std::initializer_list<uint8_t> payload) {   

    uint8_t len_payload = payload.size();

    uint8_t len = 5 + len_payload;
    uint8_t packet[64];
    packet[0] = CRSF_SYNC_byte;   // sync
    packet[1] = len;    // len
    packet[2] = CRSF_FRAMETYPE_COMMAND;   // type
    packet[3] = CRSF_ADDRESS_FLIGHT_CONTROLLER; 
    packet[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
    packet[5] = command_id; // Command ID

   // List_of_payload
    int packet_count = 6;
    for (uint8_t c : payload) {
        packet[packet_count] = c; 
        packet_count ++;
    }
    
    //packet[packet_count + len_name] = 0xFF;        // children Liste mit 0xFF abschließen
    packet[5 + len_payload] = crc8_ba(&packet[2], packet[1] - 1);        // children Liste mit 0xFF abschließen ????
    
    packet[packet[1]+1] = crc8(&packet[2], packet[1] - 1);

#if DEBUG_CRSF_SEND
    Serial.println("📤 Command send");
#endif     
}