#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/application.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/uart/uart_component.h"

#define TAG "MspaWifi"

namespace esphome
{
  namespace mspa_wifi
  {
    class MspaWifi : public Component
    {
      class MspaCom
      {
      public:
        MspaCom(uart::UARTComponent *uart, MspaWifi *mspa, const char *name)
        { // Constructor with parameters
          uart_ = uart;
          mspa_ = mspa;
          name_ = name;
          package_start_ = millis();
        }

        void handle_packet(void);
        void loop(void);
        void set_target_water_temperature(float target);
        void send_packet(const uint8_t *packet);

      private:
        uart::UARTComponent *uart_;
        MspaWifi *mspa_;

        enum class states
        {
          WAITING_FOR_START_BYTE,
          WAITING_FOR_PACKET_END,
        };
        enum states state_
        {
          states::WAITING_FOR_START_BYTE
        };
        int bytes_received_;
        uint32_t package_start_;
        uint8_t packet_[4];
        const char *name_{nullptr};
      };

      SUB_SENSOR(water_temperature);
      SUB_SENSOR(target_water_temperature);
      SUB_SENSOR(bubble_speed);
      SUB_BINARY_SENSOR(filter_pump);
      SUB_BINARY_SENSOR(uvc);
      SUB_BINARY_SENSOR(ozone);
      SUB_BINARY_SENSOR(heater);
      SUB_BINARY_SENSOR(flow_in);
      SUB_BINARY_SENSOR(flow_out);

      // SUB_SWITCH(bubble_control);

    public:
      void setup() override;
      float get_setup_priority() const override { return setup_priority::LATE; }
      void loop() override;

      void set_box_to_remote_uart(uart::UARTComponent *box_uart) { this->box_to_remote_uart_ = box_uart; }
      void set_remote_to_box_uart(uart::UARTComponent *remote_uart) { this->remote_to_box_uart_ = remote_uart; }

      void set_output_1(bool state)
      {
        ESP_LOGI("MyComponent", "Output 1 set to %s", state ? "HIGH" : "LOW");
      }

      void set_target_water_temperature(float target);

    private:
      uart::UARTComponent *box_to_remote_uart_{nullptr};
      uart::UARTComponent *remote_to_box_uart_{nullptr};

      MspaCom *mspa_box_to_remote_{nullptr};
      MspaCom *mspa_remote_to_box_{nullptr};
    };
  }
}
