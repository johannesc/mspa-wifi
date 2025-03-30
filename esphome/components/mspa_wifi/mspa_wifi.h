#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/application.h"
#include "esphome/components/uart/uart.h"

#define TAG "MspaWifi"

namespace esphome
{
  namespace mspa_wifi
  {
    class MspaWifi : public uart::UARTDevice, public Component
    {

      SUB_SENSOR(water_temperature);
      SUB_SENSOR(bubble_speed);
      SUB_BINARY_SENSOR(filter_pump);
      SUB_BINARY_SENSOR(uvc);
      SUB_BINARY_SENSOR(ozone);
      SUB_BINARY_SENSOR(heater);
      SUB_BINARY_SENSOR(flow_in);
      SUB_BINARY_SENSOR(flow_out);

    public:
      void setup() override;
      float get_setup_priority() const override { return setup_priority::LATE; }
      void loop() override;

    protected:
      void handle_packet(void);

    private:
      enum class states
      {
        WAITING_FOR_START_BYTE,
        WAITING_FOR_PACKET_END,
      };
      enum states m_state
      {
        states::WAITING_FOR_START_BYTE
      };
      int m_bytes_received;
      uint32_t m_package_start;
      uint8_t m_packet[4];
    };
  }
}
