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

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MSPA_WIFI_ID): cv.use_id(MspaWifiComponent),
        cv.Optional(LOCAL_CONF_FLOW_IN): binary_sensor.binary_sensor_schema(
            icon=ICON_WATER,
        ),
        cv.Optional(LOCAL_CONF_FLOW_OUT): binary_sensor.binary_sensor_schema(
            icon=ICON_WATER,
        ),
    }
)


async def to_code(config):
    MspaWifi_component = await cg.get_variable(config[CONF_MSPA_WIFI_ID])

    if LOCAL_CONF_FLOW_IN in config:
        print("new bin sens:", config[LOCAL_CONF_FLOW_IN])
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_FLOW_IN])
        cg.add(MspaWifi_component.set_flow_in_binary_sensor(sens))

    if LOCAL_CONF_FLOW_OUT in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_FLOW_OUT])
        cg.add(MspaWifi_component.set_flow_out_binary_sensor(sens))
