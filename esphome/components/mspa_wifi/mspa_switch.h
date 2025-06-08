#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

#include "mspa_wifi.h"


namespace esphome
{
  namespace mspa_wifi
  {
    typedef void (*set_state_func)();

    class MspaSwitch : public switch_::Switch, public Component
    {
    public:
      void setup() override;
      void write_state(bool state) override;
      void dump_config() override;

      void set_mspa(MspaWifi *mspa, uint8_t command_id)
      {
        mspa_ = mspa;
        command_id_ = command_id;
      }

    protected:
      MspaWifi *mspa_;
      uint8_t command_id_;
    };

  } // namespace mspa_wifi
} // namespace esphome
