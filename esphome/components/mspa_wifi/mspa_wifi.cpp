#include "mspa_wifi.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

#define MSPA_START_BYTE 0xA5
#define MSPA_PACKET_LEN 4
#define MAX_PACKET_TIME_MS 100

#define CMD_TEMP_REPORT 0x06
#define CMD_FLOW_REPORT 0x08

#define CMD_SET_HEATER 0x01
#define CMD_SET_FILTER 0x02
#define CMD_SET_BUBBLE 0x03
#define CMD_SET_TARGET_TEMP 0x04
#define CMD_SET_OZONE 0x0E
#define CMD_SET_UNKNOWN_0D 0x0D
#define CMD_SET_UNKNOWN_16 0x16

#define TAG "MspaWifi"

namespace esphome
{
  namespace mspa_wifi
  {
    void MspaWifi::set_box_to_remote_uart(uart::UARTComponent *box_uart) {
      mspa_box_to_remote_ = new MspaCom(box_uart, this, uvc_command_, "-->");
    }

    void MspaWifi::set_remote_to_box_uart(uart::UARTComponent *remote_uart) {
      mspa_remote_to_box_ = new MspaCom(remote_uart, this, uvc_command_, "<--");
    }

    void MspaWifi::MspaCom::fill_crc(uint8_t *packet)
    {
      uint8_t crc = 0;
      for (int i = 0; i < (MSPA_PACKET_LEN - 1); i++)
      {
        crc += packet[i];
      }
      packet[3] = crc;
    }

    void MspaWifi::set_bubble_speed(uint8_t speed)
    {
      mspa_remote_to_box_->set_bubble_speed(speed);
    }

    void MspaWifi::set_heater(bool enabled) {
      if (enabled) {
        set_filter(true); // Heater requires filter pump running, also enable that
      }
      mspa_remote_to_box_->set_heater(enabled);
    }

    void MspaWifi::set_filter(bool enabled) {
      if (filter_pump_switch_ != NULL) {
        filter_pump_switch_->publish_state(enabled);
      }
      mspa_remote_to_box_->set_filter(enabled);
    }

    void MspaWifi::set_ozone(bool enabled) {
      if (enabled) {
        set_filter(true); // Ozone requires filter pump running, also enable that
      }
      mspa_remote_to_box_->set_ozone(enabled);
    }

    void MspaWifi::set_uvc(bool enabled) {
      if (enabled) {
        set_filter(true); // UVC requires filter pump running, also enable that
      }
      mspa_remote_to_box_->set_uvc(enabled);
    }

    void MspaWifi::set_target_water_temperature(float target)
    {
      mspa_remote_to_box_->set_target_water_temperature(target);
    }

    void MspaWifi::MspaCom::set_bubble_speed(uint8_t speed)
    {
      actual_state_.bubble = speed; // This will be used in handle_packet

      if (mspa_->target_bubble_speed_number_ != NULL) {
        mspa_->target_bubble_speed_number_->publish_state(actual_state_.bubble);
      }
      uint8_t packet[MSPA_PACKET_LEN] = {MSPA_START_BYTE, CMD_SET_BUBBLE, speed, 0};
      fill_crc(packet);
      send_packet(packet);
    }

    void MspaWifi::MspaCom::set_target_water_temperature(float target)
    {
      ESP_LOGI(TAG, "Set target temp to %f", target);
      if (mspa_->target_water_temperature_number_ != NULL) {
        mspa_->target_water_temperature_number_->publish_state(target);
      }
      uint8_t packet[MSPA_PACKET_LEN] = {MSPA_START_BYTE, CMD_SET_TARGET_TEMP, (uint8_t)target, 0};
      fill_crc(packet);
      send_packet(packet);
    }

    void MspaWifi::MspaCom::set_heater(bool enabled)
    {
      ESP_LOGI(TAG, "Set heater %s", enabled ? "ENABLE" : "DISABLE");
      actual_state_.heater = enabled; // This will be used in handle_packet
      if (mspa_->heater_switch_) {
        mspa_->heater_switch_->publish_state(actual_state_.heater);
      }
      uint8_t data = enabled ? 1 : 0;
      uint8_t packet[MSPA_PACKET_LEN] = {MSPA_START_BYTE, CMD_SET_HEATER, data, 0};
      fill_crc(packet);
      send_packet(packet);
    }

    void MspaWifi::MspaCom::set_filter(bool enabled)
    {
      ESP_LOGI(TAG, "Set filter %s", enabled ? "ENABLE" : "DISABLE");
      actual_state_.filter = enabled; // This will be used in handle_packet
      if (mspa_->filter_pump_switch_ != NULL) {
        mspa_->filter_pump_switch_->publish_state(actual_state_.filter);
      }
      uint8_t data = enabled ? 1 : 0;
      uint8_t packet[MSPA_PACKET_LEN] = {MSPA_START_BYTE, CMD_SET_FILTER, data, 0};
      fill_crc(packet);
      send_packet(packet);
    }

    void MspaWifi::MspaCom::set_ozone(bool enabled)
    {
      ESP_LOGI(TAG, "Set ozone %s", enabled ? "ENABLE" : "DISABLE");
      actual_state_.ozone = enabled; // This will be used in handle_packet
      if (mspa_->ozone_switch_) {
        mspa_->ozone_switch_->publish_state(actual_state_.ozone);
      }
      uint8_t data = enabled ? 1 : 0;
      uint8_t packet[MSPA_PACKET_LEN] = {MSPA_START_BYTE, CMD_SET_OZONE, data, 0};
      fill_crc(packet);
      send_packet(packet);
    }

    void MspaWifi::MspaCom::set_uvc(bool enabled)
    {
      ESP_LOGI(TAG, "Set uvc %s", enabled ? "ENABLE" : "DISABLE");
      actual_state_.uvc = enabled; // This will be used in handle_packet
      if (mspa_->uvc_switch_) {
        mspa_->uvc_switch_->publish_state(actual_state_.uvc);
      }
      uint8_t data = enabled ? 1 : 0;
      uint8_t packet[MSPA_PACKET_LEN] = {MSPA_START_BYTE, uvc_command_, data, 0};
      fill_crc(packet);
      send_packet(packet);
    }

    void MspaWifi::MspaCom::send_packet(const uint8_t *packet)
    {
      ESP_LOGD(TAG, "%s: Send packet: %02X %02X %02X %02X", name_, packet[0], packet[1], packet[2], packet[3]);

      for (int i = 0; i < MSPA_PACKET_LEN; i++)
      {
        uart_->write_byte(packet[i]);
      }
    }

    bool MspaWifi::MspaCom::handle_packet()
    {
      uint8_t crc = 0;
      for (int i = 0; i < (MSPA_PACKET_LEN - 1); i++)
      {
        crc += packet_[i];
      }

      if (crc != packet_[MSPA_PACKET_LEN - 1])
      {
        ESP_LOGE(TAG, "%s: Bad CRC, got 0x%02X, expected 0x%02X", name_, packet_[MSPA_PACKET_LEN - 1], crc);
        return false;
      }

      switch (packet_[1])
      {
      case CMD_TEMP_REPORT:
      {
        float water_temperature = (float)packet_[2] / 2.0f;
        ESP_LOGI(TAG, "%s: Water temp report %f", name_, water_temperature);
        if (mspa_->water_temperature_sensor_ != NULL) {
          mspa_->water_temperature_sensor_->publish_state(water_temperature);
        }
        break;
      }
      case CMD_FLOW_REPORT:
      {
        bool flow_in = (packet_[2] & 0x01) != 0;
        bool flow_out = (packet_[2] & 0x02) != 0;
        ESP_LOGI(TAG, "%s: Flow in: %s, out: %s", name_, flow_in ? "On" : "Off", flow_out ? "On" : "Off");
        if (mspa_->flow_in_binary_sensor_ != NULL) {
          mspa_->flow_in_binary_sensor_->publish_state(flow_in);
        }
        if (mspa_->flow_out_binary_sensor_ != NULL) {
          mspa_->flow_out_binary_sensor_->publish_state(flow_out);
        }
        break;
      }
      case CMD_SET_TARGET_TEMP:
      {
        float target_temperature = packet_[2];
        if (mspa_->target_water_temperature_number_ != NULL) {
          mspa_->target_water_temperature_number_->publish_state(target_temperature);
        }
        ESP_LOGI(TAG, "%s: Set target temp %f", name_, target_temperature);
        break;
      }
      case CMD_SET_HEATER:
      {
        bool heater_enabled = packet_[2] == 0x01;

        if (heater_enabled != remote_state_.heater) {
          // The heater has changed at the remote
          // The remote is now "in control"
          remote_state_.heater = heater_enabled;
          actual_state_.heater = heater_enabled;
          if (mspa_->heater_switch_) {
            mspa_->heater_switch_->publish_state(actual_state_.heater);
          }
        } else if (heater_enabled != actual_state_.heater) {
          packet_[2] = actual_state_.heater;
          fill_crc(packet_);
        }
        ESP_LOGI(TAG, "%s: Heater enabled: %s", name_, actual_state_.heater ? "true" : "false");
        break;
      }
      case CMD_SET_FILTER:
      {
        bool filter_enabled = packet_[2] == 0x01;
        if (filter_enabled != remote_state_.filter) {
          // The filter has changed at the remote
          // The remote is now "in control"
          remote_state_.filter = filter_enabled;
          actual_state_.filter = filter_enabled;
          if (mspa_->filter_pump_switch_ != NULL) {
            mspa_->filter_pump_switch_->publish_state(actual_state_.filter);
          }
        } else if (filter_enabled != actual_state_.filter) {
          packet_[2] = actual_state_.filter;
          fill_crc(packet_);
        }

        ESP_LOGI(TAG, "%s: Filter enabled: %s", name_, actual_state_.filter ? "true" : "false");
        break;
      }
      case CMD_SET_BUBBLE:
      {
        uint8_t bubble_speed = packet_[2];

        if (bubble_speed != remote_state_.bubble) {
          // The bubble speed was changed at the remote
          // The remote is now "in control"
          remote_state_.bubble = bubble_speed;
          actual_state_.bubble = bubble_speed;
          if (mspa_->target_bubble_speed_number_ != NULL) {
            mspa_->target_bubble_speed_number_->publish_state(actual_state_.bubble);
          }
        } else if (bubble_speed != actual_state_.bubble) {
          packet_[2] = actual_state_.bubble;
          fill_crc(packet_);
        }
        ESP_LOGI(TAG, "%s: Bubble speed: %d", name_, packet_[2]);
        break;
      }
      case CMD_SET_OZONE:
      {
        bool ozone_enabled = packet_[2] == 0x01;
        if (ozone_enabled != remote_state_.ozone) {
          // The bubble speed was changed at the remote
          // The remote is now "in control"
          remote_state_.ozone = ozone_enabled;
          actual_state_.ozone = ozone_enabled;
          if (mspa_->ozone_switch_) {
            mspa_->ozone_switch_->publish_state(actual_state_.ozone);
          }
        } else if (ozone_enabled != actual_state_.ozone) {
          packet_[2] = actual_state_.ozone;
          fill_crc(packet_);
        }
        ESP_LOGI(TAG, "%s: Ozone enabled: %s", name_, actual_state_.ozone ? "true" : "false");
        break;
      }
      case CMD_SET_UVC_ALT_1:
      case CMD_SET_UVC_ALT_2:
      {
        if (packet_[1] != this->uvc_command_) {
          ESP_LOGW(TAG, "Warning 'uvc_command: 0x%02X' was specified in yaml, but 0x%02X was received",
            this->uvc_command_, packet_[1]);
        }

        bool uvc_enabled = packet_[2] == 0x01;
        if (uvc_enabled != remote_state_.uvc) {
          // UVC was changed as the remote
          // the remote if now "in control"
          remote_state_.uvc = uvc_enabled;
          actual_state_.uvc = uvc_enabled;
          if (mspa_->uvc_switch_) {
            mspa_->uvc_switch_->publish_state(actual_state_.uvc);
          }
        } else {
          packet_[2] = actual_state_.uvc ? 0x01 : 0x00;
          fill_crc(packet_);
        }

        ESP_LOGI(TAG, "%s: UVC enabled: %s", name_, uvc_enabled ? "true" : "false");
        break;
      }
      default:
      {
        ESP_LOGE(TAG, "%s: Unknown packet: %02X %02X %02X %02X", name_, packet_[0], packet_[1], packet_[2], packet_[3]);
        break;
      }
      }
      return true;
    }

    void MspaWifi::MspaCom::loop()
    {
      if ((!uart_->available()) && (state_ != states::WAITING_FOR_START_BYTE))
      {
        uint32_t now = millis();
        if ((now - package_start_) > MAX_PACKET_TIME_MS)
        {
          ESP_LOGE(TAG, "%s: Timeout", name_);
          state_ = states::WAITING_FOR_START_BYTE;
          bytes_received_ = 0;
        }
        return;
      }

      while (uart_->available())
      {
        uint8_t data;
        uart_->read_byte(&data);

        switch (state_)
        {
        case states::WAITING_FOR_START_BYTE:
          if (data != MSPA_START_BYTE)
          {
            ESP_LOGE(TAG, "%s: Bad start 0x%02X", name_, data);
            return;
          }
          state_ = states::WAITING_FOR_PACKET_END;
          packet_[0] = data;
          bytes_received_ = 1;
          package_start_ = millis();
          break;
        case states::WAITING_FOR_PACKET_END:
          packet_[bytes_received_++] = data;
          if (bytes_received_ == MSPA_PACKET_LEN)
          {
            state_ = states::WAITING_FOR_START_BYTE;
            bytes_received_ = 0;
            if (handle_packet()) {
              send_packet(packet_);
            }
          }
          break;
        }
      }
    }

    void MspaWifi::loop()
    {
      mspa_box_to_remote_->loop();
      mspa_remote_to_box_->loop();
    }
  }
}
