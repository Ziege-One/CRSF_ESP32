#ifndef CRSF_H
#define CRSF_H

#include <Arduino.h>
#include "crsf_esp32_protocol.h"

#define CRSF_MAX_CHANNELS 16
#define CRSF_PACKET_SIZE 64

#define BAUD_RATE 420000
#define CHANNEL_MASK 0x07FF

class CRSF
{
public:

    CRSF();
    void init_crsf(HardwareSerial *serialPort, uint8_t rxPin, uint8_t txPin);
    void read_packets(uint8_t debug);
    void send_packets(byte* buffer, size_t length, uint8_t debug);

    void send_rc_channels_packed();
    void send_ping();
    void send_device_info(const char* name, uint8_t parameter);
    void send_param_response_CRSF_UINT8(uint8_t param_id, uint8_t parent, const char* name, uint8_t val, uint8_t min, uint8_t max, const char* unit);
    void send_param_response_CRSF_FLOAT(uint8_t param_id, uint8_t parent,  const char* name, uint32_t val, uint32_t min, uint32_t max, uint32_t default_val, uint8_t decimal_point, uint32_t step_size, const char* unit);
    void send_param_response_CRSF_TEXT_SELECTION(uint8_t param_id, uint8_t parent,  const char* name, const char* options, uint8_t val, uint8_t min, uint8_t max);
    void send_param_response_CRSF_STRING(uint8_t param_id, uint8_t parent,  const char* name, const char* value, uint8_t val, uint8_t max_length);
    void send_param_response_CRSF_FOLDER(uint8_t param_id, uint8_t parent, const char* name, std::initializer_list<uint8_t> children);
    void send_param_response_CRSF_INFO(uint8_t param_id, uint8_t parent,  const char* name, const char* info);

    void send_tele_GPS(int32_t latitude, int32_t longitude, uint16_t groundspeed, uint16_t heading, uint16_t altitude, uint8_t satellites);
    void send_tele_Battery_Sensor(uint16_t voltage, uint16_t current, uint32_t capacity_used, uint8_t remaining);
    void send_tele_Vario(uint16_t v_speed);   
    void send_tele_Baro_Altitude(int32_t altitude_dm, int16_t vertical_speed_cm_s);
    void send_tele_Airspeed(uint16_t speed);
    void send_tele_RPM(uint8_t rpm_source_id, std::initializer_list<int32_t> rpm_value);
    void send_tele_Temp(uint8_t temp_source_id, std::initializer_list<int32_t> temperature);
    void send_tele_Voltages(uint8_t voltage_source_id, std::initializer_list<int32_t> voltage_values);
    void send_tele_Attitude(int16_t pitch, int16_t roll, int16_t yaw);

    void send_command_MWSET(uint8_t adress, uint8_t state);
    void send_command_MWSET4(uint8_t adress, uint8_t stateH, uint8_t stateL);
    void send_command_MWSET4M(uint8_t adress_1, uint8_t stateH_1, uint8_t stateL_1, uint8_t adress_2, uint8_t stateH_2, uint8_t stateL_2);
    void send_command_MWPROP(uint8_t adress, uint8_t channel, uint8_t duty );

    void send_param(uint8_t parameter_number, uint8_t data);
    void read_param(uint8_t parameter_number, uint8_t parameter_chunk_number);

    void send_command(uint8_t command_id, std::initializer_list<uint8_t> payload);

    uint16_t get_crfs_channels(uint8_t ch) const { return channels[ch]; }
    uint8_t get_crfs_buffer(uint8_t ch) const { return crfs_buffer[ch]; }

    bool getDeviceInfoReplyPending() const { return deviceInfoReplyPending; }
    bool getDeviceEntryReplyPending() const { return deviceEntryReplyPending; }
    bool getDeviceReadReplyPending() const { return deviceReadReplyPending; }
    bool getDeviceWriteReplyPending() const { return deviceWriteReplyPending; }
    bool getDeviceCommandReplyPending() const { return deviceCommandReplyPending; }

    void setDeviceInfoReplyPending(int newValue);
    void setDeviceEntryReplyPending(int newValue);
    void setDeviceReadReplyPending(int newValue);
    void setDeviceWriteReplyPending(int newValue);
    void setDeviceCommandReplyPending(int newValue);

    void set_crsf_channel(uint8_t ch, uint16_t value);

private:
    void updateChannels();
    void updateLink_Statistics();
    void updateDevice_Info();
    void print_channels();
    void crsfDataReceive();

    bool deviceInfoReplyPending;
    bool deviceEntryReplyPending;
    bool deviceReadReplyPending;
    bool deviceWriteReplyPending;
    bool deviceCommandReplyPending;

    uint8_t crfs_buffer[CRSF_PACKET_SIZE];
    uint16_t channels[CRSF_MAX_CHANNELS];
};

#endif