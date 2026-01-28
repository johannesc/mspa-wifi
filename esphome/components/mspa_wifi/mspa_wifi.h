#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/uart/uart_component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/application.h"
#include "esphome/core/component.h"

#define CMD_SET_UVC_ALT_1 0x10
#define CMD_SET_UVC_ALT_2 0x15

namespace esphome
{
  namespace mspa_wifi
  {
    class MspaWifi : public Component
    {
      class MspaCom
      {
      public:
        MspaCom(uart::UARTComponent *uart, const char* name)
        {
          uart_ = uart;
          name_ = name;
          package_start_ = millis();
        }
        void loop(void);

      protected:
        void fill_crc(uint8_t *packet);
        void send_packet(const uint8_t *packet);
        virtual void handle_packet(uint8_t *packet) = 0;
        const char *name_{nullptr};

      private:
        uart::UARTComponent *uart_;
        uint8_t packet_[4];

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
      };
      class MspaRemoteToBoxCom : public MspaCom
      {
      public:
        MspaRemoteToBoxCom(uart::UARTComponent *uart, MspaWifi *mspa, uint8_t uvc_command, const char *name)
          : MspaCom(uart, name)
        {
          mspa_ = mspa;
          uvc_command_ = uvc_command;
        }

        void set_target_water_temperature(float target);
        void set_bubble_speed(uint8_t speed);

        void set_heater(bool enabled);
        void set_filter(bool enabled);
        void set_ozone(bool enabled);
        void set_uvc(bool enabled);

      protected:
        void handle_packet(uint8_t *packet) override;

      private:
        MspaWifi *mspa_;

        uint8_t uvc_command_ = 0;
      };
      class MspaBoxToRemoteCom : public MspaCom
      {
      public:
        MspaBoxToRemoteCom(uart::UARTComponent *uart, MspaWifi *mspa, const char *name)
          : MspaCom(uart, name)
        {
          mspa_ = mspa;
        }

      protected:
        void handle_packet(uint8_t *packet) override;

      private:
        MspaWifi *mspa_;
      };

      typedef struct {
        bool heater;
        bool filter;
        uint8_t bubble;
        bool ozone;
        bool uvc;
      } mspa_state_t;

      mspa_state_t actual_state_ = {0};
      mspa_state_t remote_state_ = {0};

      SUB_SENSOR(water_temperature);
      SUB_BINARY_SENSOR(flow_in);
      SUB_BINARY_SENSOR(flow_out);
      SUB_BINARY_SENSOR(remote_filter_pump);

      SUB_SWITCH(filter_pump);
      SUB_SWITCH(heater);
      SUB_SWITCH(uvc);
      SUB_SWITCH(ozone);

    public:
      float get_setup_priority() const override { return setup_priority::LATE; }
      void loop() override;

      void set_box_to_remote_uart(uart::UARTComponent *box_uart);
      void set_remote_to_box_uart(uart::UARTComponent *remote_uart);

      void set_target_water_temperature_number(number::Number *number) { this->target_water_temperature_number_ = number; }
      void set_target_bubble_speed_number(number::Number *number) { this->target_bubble_speed_number_ = number; }
      void set_uvc_command(int command) { this->uvc_command_ = command; }

      void set_target_water_temperature(float target);
      void set_bubble_speed(uint8_t speed);

      void set_heater(bool enabled);
      void set_filter(bool enabled);
      void set_ozone(bool enabled);
      void set_uvc(bool enabled);

    private:
      uint8_t uvc_command_ = CMD_SET_UVC_ALT_2;

      number::Number *target_water_temperature_number_{nullptr};
      number::Number *target_bubble_speed_number_{nullptr};

      MspaBoxToRemoteCom *mspa_box_to_remote_{nullptr};
      MspaRemoteToBoxCom *mspa_remote_to_box_{nullptr};
    };
  }
}
