#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/application.h"
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#include "esphome/components/uart/uart.h"
#include "esphome/components/uart/uart_component.h"

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

        bool handle_packet(void);
        void loop(void);
        void set_target_water_temperature(float target);
        void set_bubble_speed(uint8_t speed);

        void set_heater(bool enabled);
        void set_filter(bool enabled);
        void set_ozone(bool enabled);
        void set_uvc(bool enabled);

        void send_packet(const uint8_t *packet);
        void fill_crc(uint8_t *packet);

      private:
        uart::UARTComponent *uart_;
        MspaWifi *mspa_;

        typedef struct {
          bool heater;
          bool filter;
          uint8_t bubble;
          bool ozone;
          bool uvc;
        } mspa_state_t;

        mspa_state_t actual_state_ = {0};
        mspa_state_t remote_state_ = {0};

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
      SUB_BINARY_SENSOR(flow_in);
      SUB_BINARY_SENSOR(flow_out);

#ifdef USE_SWITCH
      SUB_SWITCH(filter_pump);
      SUB_SWITCH(heater);
      SUB_SWITCH(uvc);
      SUB_SWITCH(ozone);
#endif

    public:
      void setup() override;
      float get_setup_priority() const override { return setup_priority::LATE; }
      void loop() override;

      void set_box_to_remote_uart(uart::UARTComponent *box_uart) { this->box_to_remote_uart_ = box_uart; }
      void set_remote_to_box_uart(uart::UARTComponent *remote_uart) { this->remote_to_box_uart_ = remote_uart; }
      void set_target_water_temperature_number(number::Number *number) { this->target_water_temperature_number_ = number; }
      void set_target_bubble_speed_number(number::Number *number) { this->target_bubble_speed_number_ = number; }

      void set_output_1(bool state)
      {
        ESP_LOGI("MyComponent", "Output 1 set to %s", state ? "HIGH" : "LOW");
      }

      void set_target_water_temperature(float target);
      void set_bubble_speed(uint8_t speed);

      void set_switch_command(uint8_t command_id, bool enabled);

    private:
      uart::UARTComponent *box_to_remote_uart_{nullptr};
      uart::UARTComponent *remote_to_box_uart_{nullptr};

      number::Number *target_water_temperature_number_{nullptr};
      number::Number *target_bubble_speed_number_{nullptr};

      MspaCom *mspa_box_to_remote_{nullptr};
      MspaCom *mspa_remote_to_box_{nullptr};
    };
  }
}
