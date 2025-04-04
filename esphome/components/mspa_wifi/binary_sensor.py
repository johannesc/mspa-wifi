import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor

from esphome.const import (
    ICON_WATER,
    ICON_HEATING_COIL,
    ICON_LIGHTBULB,
)
from . import CONF_MSPA_WIFI_ID, MspaWifiComponent

LOCAL_CONF_FLOW_IN = "flow_in"
LOCAL_CONF_FLOW_OUT = "flow_out"
LOCAL_CONF_FILTER_PUMP = "filter_pump"
LOCAL_CONF_UVC = "uvc"
LOCAL_CONF_OZONE = "ozone"
LOCAL_CONF_HEATER = "heater"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MSPA_WIFI_ID): cv.use_id(MspaWifiComponent),
        cv.Optional(LOCAL_CONF_FLOW_IN): binary_sensor.binary_sensor_schema(
            icon=ICON_WATER,
        ),
        cv.Optional(LOCAL_CONF_FLOW_OUT): binary_sensor.binary_sensor_schema(
            icon=ICON_WATER,
        ),
        cv.Optional(LOCAL_CONF_FILTER_PUMP): binary_sensor.binary_sensor_schema(
            icon=ICON_WATER,
        ),
        cv.Optional(LOCAL_CONF_UVC): binary_sensor.binary_sensor_schema(
            icon=ICON_LIGHTBULB,
        ),
        cv.Optional(LOCAL_CONF_OZONE): binary_sensor.binary_sensor_schema(
            icon="mdi:gas-cylinder",
        ),
        cv.Optional(LOCAL_CONF_HEATER): binary_sensor.binary_sensor_schema(
            icon=ICON_HEATING_COIL,
        ),
    }
)


async def to_code(config):
    MspaWifi_component = await cg.get_variable(config[CONF_MSPA_WIFI_ID])

    if LOCAL_CONF_FLOW_IN in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_FLOW_IN])
        cg.add(MspaWifi_component.set_flow_in_binary_sensor(sens))

    if LOCAL_CONF_FLOW_OUT in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_FLOW_OUT])
        cg.add(MspaWifi_component.set_flow_out_binary_sensor(sens))

    if LOCAL_CONF_FILTER_PUMP in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_FILTER_PUMP])
        cg.add(MspaWifi_component.set_filter_pump_binary_sensor(sens))

    if LOCAL_CONF_UVC in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_UVC])
        cg.add(MspaWifi_component.set_uvc_binary_sensor(sens))

    if LOCAL_CONF_OZONE in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_OZONE])
        cg.add(MspaWifi_component.set_ozone_binary_sensor(sens))

    if LOCAL_CONF_HEATER in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_HEATER])
        cg.add(MspaWifi_component.set_heater_binary_sensor(sens))
