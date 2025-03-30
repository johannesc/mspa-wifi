#include "mspa_wifi.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

#define MSPA_START_BYTE 0xA5
#define MAX_PACKET_TIME_MS 100
#define MAX_PACKET_LEN 4

#define CMD_TEMP_REPORT 0x06
#define CMD_FLOW_REPORT 0x08

namespace esphome
{
  namespace mspa_wifi
  {

    void MspaWifi::setup()
    {
      m_package_start = millis();
    }

    void MspaWifi::handle_packet(void)
    {
      uint8_t crc = 0;
      for (int i; i < (MAX_PACKET_LEN - 1); i++)
      {
        crc += m_packet[i];
      }

      if (crc != m_packet[MAX_PACKET_LEN - 1])
      {
        ESP_LOGD(TAG, "Bad CRC");
        return;
      }
      ESP_LOGD(TAG, "Got good packet!");

      switch (m_packet[1])
      {
      case CMD_TEMP_REPORT:
      {
        float water_temperature = (float)m_packet[2] / 2.0f;
        ESP_LOGI(TAG, "Water temp report %f", water_temperature);
        this->water_temperature_sensor_->publish_state(water_temperature);
        break;
      }
      case CMD_FLOW_REPORT:
      {
        bool flow_in = (m_packet[2] & 0x01) != 0;
        bool flow_out = (m_packet[2] & 0x02) != 0;
        ESP_LOGI(TAG, "Flow in: %s, out: %s", flow_in ? "On" : "Off", flow_out ? "On" : "Off");
        this->flow_in_binary_sensor_->publish_state(flow_in);
        this->flow_out_binary_sensor_->publish_state(flow_out);
        break;
      }
      }
    }

    void MspaWifi::loop()
    {
      if (!available())
      {
        uint32_t now = millis();
        if ((now - m_package_start) > MAX_PACKET_TIME_MS)
        {
          m_state = states::WAITING_FOR_START_BYTE;
          m_bytes_received = 0;
        }
        return;
      }

      while (available())
      {
        ESP_LOGD(TAG, "Data available");
        int data = read();

        switch (m_state)
        {
        case states::WAITING_FOR_START_BYTE:
          if (data != MSPA_START_BYTE)
          {
            return;
          }
          m_state = states::WAITING_FOR_PACKET_END;
          m_packet[m_bytes_received] = data;
          m_bytes_received = 1;
          break;
        case states::WAITING_FOR_PACKET_END:
          m_packet[m_bytes_received++] = data;
          if (m_bytes_received == MAX_PACKET_LEN)
          {
            m_state = states::WAITING_FOR_START_BYTE;
            m_bytes_received = 0;
          }

          handle_packet();
          break;
        }
      }
    }
  }
}
