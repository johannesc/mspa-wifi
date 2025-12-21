#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "mspa_switch.h"

namespace esphome
{
    namespace mspa_wifi
    {
        static const char *TAG = "mspa.switch";

        void MspaSwitch::setup()
        {
        }

        void MspaSwitch::write_state(bool state)
        {
            switch (switch_type_)
            {
            case MspaSwitchType::HEATER:
                ESP_LOGI(TAG, "Heater enable %s", state ? "true" : "false");
                mspa_->set_heater(state);
                break;
            case MspaSwitchType::FILTER:
                ESP_LOGI(TAG, "Filter enable %s", state ? "true" : "false");
                mspa_->set_filter(state);
                break;
            case MspaSwitchType::UVC:
                ESP_LOGI(TAG, "UVC enable %s", state ? "true" : "false");
                mspa_->set_uvc(state);
                break;
            case MspaSwitchType::OZONE:
                ESP_LOGI(TAG, "Ozone enable %s", state ? "true" : "false");
                mspa_->set_ozone(state);
                break;
            default:
                break;
            }

            //TODO this should be handled in mspa_wifi component
            // e.g. trying to set ozone when filter_pump is not enabled should fail
            this->publish_state(state);
        }

        void MspaSwitch::dump_config()
        {
            ESP_LOGCONFIG(TAG, "Switch");
        }

    } // namespace mspa_wifi
} // namespace esphome
