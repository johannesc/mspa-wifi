#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "mspa_number.h"

namespace esphome
{
    namespace mspa_wifi
    {
        static const char *TAG = "mspa.number";

        void MspaNumber::setup()
        {
        }

        void MspaNumber::control(float value)
        {
            if (mspa_ == nullptr)
            {
                return;
            }

            switch (type_)
            {
            case MspaNumberType::TARGET_TEMP:
                ESP_LOGI("number_template", "New target temp from HA: %f", value);
                mspa_->set_target_water_temperature(value);
                break;

            case MspaNumberType::BUBBLE_SPEED:
                ESP_LOGI("number_template", "New bubble speed from HA: %f", value);
                mspa_->set_bubble_speed(value);
                break;
            }

            this->publish_state(value);
        }

        void MspaNumber::dump_config()
        {
            ESP_LOGCONFIG(TAG, "number");
        }

    } // namespace mspa_wifi
} // namespace esphome
