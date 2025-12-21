import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    UNIT_CELSIUS,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    DEVICE_CLASS_EMPTY,
)
from . import CONF_MSPA_WIFI_ID, MspaWifiComponent

LOCAL_CONF_WATER_TEMPERATURE = "water_temperature"
LOCAL_CONF_TARGET_WATER_TEMPERATURE = "target_water_temperature"
LOCAL_CONF_BUBBLE_SPEED = "bubble_speed"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_MSPA_WIFI_ID): cv.use_id(MspaWifiComponent),
        cv.Optional(LOCAL_CONF_WATER_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:thermometer",
        ),
        cv.Optional(LOCAL_CONF_TARGET_WATER_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:thermometer",
        ),
        cv.Optional(LOCAL_CONF_BUBBLE_SPEED): sensor.sensor_schema(
            unit_of_measurement="",
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)

async def to_code(config):
    MspaWifi_component = await cg.get_variable(config[CONF_MSPA_WIFI_ID])

    if LOCAL_CONF_WATER_TEMPERATURE in config:

        print("water temp")
        print("cfg", config[LOCAL_CONF_WATER_TEMPERATURE])

        sens = await sensor.new_sensor(config[LOCAL_CONF_WATER_TEMPERATURE])
        cg.add(MspaWifi_component.set_water_temperature_sensor(sens))

    if LOCAL_CONF_TARGET_WATER_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[LOCAL_CONF_TARGET_WATER_TEMPERATURE])
        cg.add(MspaWifi_component.set_target_water_temperature_sensor(sens))

    if LOCAL_CONF_BUBBLE_SPEED in config:
        sens = await sensor.new_sensor(config[LOCAL_CONF_BUBBLE_SPEED])
        cg.add(MspaWifi_component.set_bubble_speed_sensor(sens))
