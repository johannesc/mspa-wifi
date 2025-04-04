#include "mspa_wifi.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

#define MSPA_START_BYTE 0xA5
#define MSPA_PACKET_LEN 4
#define MAX_PACKET_TIME_MS 100

#define CMD_TEMP_REPORT 0x06
#define CMD_FLOW_REPORT 0x08

#define CMD_SET_TARGET_TEMP 0x04
#define CMD_SET_HEATER 0x01
#define CMD_SET_FILTER 0x02
#define CMD_SET_BUBBLE 0x03
#define CMD_SET_TARGET_TEMP 0x04
#define CMD_SET_OZONE 0x0E
#define CMD_SET_UVC 0x15

namespace esphome
{
  namespace mspa_wifi
  {
    void MspaWifi::setup()
    {
      mspa_box_to_remote_ = new MspaCom(box_to_remote_uart_, this, "-->");
      mspa_remote_to_box_ = new MspaCom(remote_to_box_uart_, this, "<--");
    }

    void MspaWifi::set_target_water_temperature(float target)
    {
      mspa_remote_to_box_->set_target_water_temperature(target);
      target_water_temperature_sensor_->publish_state(target);
    }

    void MspaWifi::MspaCom::set_target_water_temperature(float target)
    {
      ESP_LOGI(TAG, "Set target temp to %f", target);
      uint8_t packet[MSPA_PACKET_LEN] = {MSPA_START_BYTE, CMD_SET_TARGET_TEMP, (uint8_t)target, 0};
      uint8_t crc = 0;
      for (int i = 0; i < (MSPA_PACKET_LEN - 1); i++)
      {
        crc += packet[i];
      }
      send_packet(packet);
    }

    void MspaWifi::MspaCom::send_packet(const uint8_t *packet)
    {
      for (int i = 0; i < MSPA_PACKET_LEN; i++)
      {
        uart_->write_byte(packet_[i]);
      }
    }

    void MspaWifi::MspaCom::handle_packet()
    {
      ESP_LOGI(TAG, "%s: Packet: %02X %02X %02X %02X", name_, packet_[0], packet_[1], packet_[2], packet_[3]);
      uint8_t crc = 0;
      for (int i; i < (MSPA_PACKET_LEN - 1); i++)
      {
        crc += packet_[i];
      }

      if (crc != packet_[MSPA_PACKET_LEN - 1])
      {
        ESP_LOGE(TAG, "%s: Bad CRC", name_);
        return;
      }

      switch (packet_[1])
      {
      case CMD_TEMP_REPORT:
      {
        float water_temperature = (float)packet_[2] / 2.0f;
        ESP_LOGI(TAG, "%s: Water temp report %f", name_, water_temperature);
        mspa_->water_temperature_sensor_->publish_state(water_temperature);
        break;
      }
      case CMD_FLOW_REPORT:
      {
        bool flow_in = (packet_[2] & 0x01) != 0;
        bool flow_out = (packet_[2] & 0x02) != 0;
        ESP_LOGI(TAG, "%s: Flow in: %s, out: %s", name_, flow_in ? "On" : "Off", flow_out ? "On" : "Off");
        mspa_->flow_in_binary_sensor_->publish_state(flow_in);
        mspa_->flow_out_binary_sensor_->publish_state(flow_out);
        break;
      }
      case CMD_SET_TARGET_TEMP:
      {
        float target_temperature = packet_[2];
        mspa_->target_water_temperature_sensor_->publish_state(target_temperature);
        ESP_LOGI(TAG, "%s: Set target temp %f", name_, target_temperature);
        break;
      }
      case CMD_SET_HEATER:
      {
        bool heater_enabled = packet_[2] == 0x01;
        mspa_->heater_binary_sensor_->publish_state(heater_enabled);
        ESP_LOGI(TAG, "%s: Heater enabled: %s", name_, heater_enabled ? "true" : "false");
        break;
      }
      case CMD_SET_FILTER:
      {
        bool filter_enabled = packet_[2] == 0x01;
        mspa_->filter_pump_binary_sensor_->publish_state(filter_enabled);
        ESP_LOGI(TAG, "%s: Filter enabled: %s", name_, filter_enabled ? "true" : "false");
        break;
      }
      case CMD_SET_BUBBLE:
      {
        uint8_t bubble_speed = packet_[2];
        mspa_->bubble_speed_sensor_->publish_state(bubble_speed);
        ESP_LOGI(TAG, "%s: Bubble speed: %d", name_, bubble_speed);
        break;
      }
      case CMD_SET_OZONE:
      {
        bool ozone_enabled = packet_[2] == 0x01;
        mspa_->ozone_binary_sensor_->publish_state(ozone_enabled);
        ESP_LOGI(TAG, "%s: Ozone enabled: %s", name_, ozone_enabled ? "true" : "false");
        break;
      }
      case CMD_SET_UVC:
      {
        bool uvc_enabled = packet_[2] == 0x01;
        mspa_->uvc_binary_sensor_->publish_state(uvc_enabled);
        ESP_LOGI(TAG, "%s: UVC enabled: %s", name_, uvc_enabled ? "true" : "false");
        break;
      }
      default:
      {
        ESP_LOGE(TAG, "%s: Uknown cmd 0x%02X", name_, packet_[1]);
        break;
      }
      }
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
        // uart_->write_byte(data);

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
            handle_packet();
            send_packet(packet_);
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
