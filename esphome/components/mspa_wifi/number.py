import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    UNIT_CELSIUS,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_EMPTY,
    ICON_THERMOMETER,
    ICON_FAN,
    CONF_MAX_VALUE,
)
from . import CONF_MSPA_WIFI_ID, MspaWifiComponent, mspa_wifi_ns

LOCAL_CONF_TARGET_WATER_TEMPERATURE = "target_water_temperature"
LOCAL_CONF_BUBBLE_SPEED = "bubble_speed"

MspaNumber = mspa_wifi_ns.class_("MspaNumber", number.Number, cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MSPA_WIFI_ID): cv.use_id(MspaWifiComponent),
        cv.Optional(LOCAL_CONF_TARGET_WATER_TEMPERATURE): number.number_schema(
            MspaNumber,
            icon=ICON_THERMOMETER,
            unit_of_measurement=UNIT_CELSIUS,
            device_class=DEVICE_CLASS_TEMPERATURE,
        ),
        cv.Optional(LOCAL_CONF_BUBBLE_SPEED): number.number_schema(
            MspaNumber,
            icon=ICON_FAN,
            unit_of_measurement="",
            device_class=DEVICE_CLASS_EMPTY,
        ).extend(
            {
                cv.Optional("max_value"): cv.float_,
            }
        ),
    }
)


async def to_code(config):
    MspaWifi_component = await cg.get_variable(config[CONF_MSPA_WIFI_ID])

    if LOCAL_CONF_TARGET_WATER_TEMPERATURE in config:
        num = await number.new_number(
            config[LOCAL_CONF_TARGET_WATER_TEMPERATURE],
            min_value=24,
            max_value=40,
            step=1,
        )
        cg.add(MspaWifi_component.set_target_water_temperature_number(num))
        cg.add(num.set_mspa(MspaWifi_component))
        cg.add(
            num.set_type(
                cg.RawExpression("esphome::mspa_wifi::MspaNumberType::TARGET_TEMP")
            )
        )

    if LOCAL_CONF_BUBBLE_SPEED in config:
        bubble_conf = config[LOCAL_CONF_BUBBLE_SPEED]
        num = await number.new_number(
            bubble_conf, min_value=0, max_value=bubble_conf[CONF_MAX_VALUE], step=1
        )
        cg.add(MspaWifi_component.set_target_bubble_speed_number(num))
        cg.add(num.set_mspa(MspaWifi_component))
        cg.add(
            num.set_type(
                cg.RawExpression("esphome::mspa_wifi::MspaNumberType::BUBBLE_SPEED")
            )
        )
