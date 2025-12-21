#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

#include "mspa_wifi.h"


namespace esphome
{
  namespace mspa_wifi
  {
    enum class MspaSwitchType
    {
      HEATER,
      FILTER,
      UVC,
      OZONE,
    };
    typedef void (*set_state_func)();

    class MspaSwitch : public switch_::Switch, public Component
    {
    public:
      void setup() override;
      void write_state(bool state) override;
      void dump_config() override;

      void set_mspa(MspaWifi *mspa, MspaSwitchType switch_type)
      {
        mspa_ = mspa;
        switch_type_ = switch_type;
      }

    protected:
      MspaWifi *mspa_;
      MspaSwitchType switch_type_;
    };

  } // namespace mspa_wifi
} // namespace esphome
