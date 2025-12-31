#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

#include "mspa_wifi.h"

namespace esphome
{
  namespace mspa_wifi
  {
    enum class MspaNumberType
    {
      TARGET_TEMP,
      BUBBLE_SPEED,
    };
    typedef void (*set_state_func)();

    class MspaNumber : public number::Number, public Component
    {
    public:
      void setup() override;
      void dump_config() override;
      void control(float value) override;

      void set_mspa(MspaWifi *mspa)
      {
        mspa_ = mspa;
      }

      void set_type(MspaNumberType type) { type_ = type; }

    protected:
      MspaWifi *mspa_;
      MspaNumberType type_;
      float last_value_ = -1;
    };

  } // namespace mspa_wifi
} // namespace esphome
