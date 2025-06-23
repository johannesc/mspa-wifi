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
            ESP_LOGI(TAG, "Write state %s", state ? "true" : "false");
            mspa_->set_switch_command(command_id_, state);
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
