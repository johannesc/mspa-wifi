import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import (
    binary_sensor,
    number,
    sensor,
    switch,
    uart,
)
from esphome.const import (
    CONF_ID,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_TEMPERATURE,
    ICON_FAN,
    ICON_LIGHTBULB,
    ICON_THERMOMETER,
    ICON_WATER,
    UNIT_CELSIUS,
)

# -------------------------
# LOCAL CONFIG KEYS
# -------------------------

LOCAL_CONF_UART_BOX_TO_REMOTE_ID = "uart_box_to_remote_id"
LOCAL_CONF_UART_REMOTE_TO_BOX_ID = "uart_remote_to_box_id"

LOCAL_CONV_UVC_COMMAND = "uvc_command"

LOCAL_CONF_FLOW_IN = "flow_in"
LOCAL_CONF_FLOW_OUT = "flow_out"
LOCAL_CONF_WATER_TEMPERATURE = "water_temperature"

LOCAL_CONF_FILTER_PUMP = "filter_pump"
LOCAL_CONF_UVC = "uvc"
LOCAL_CONF_OZONE = "ozone"
LOCAL_CONF_HEATER = "heater"

LOCAL_CONF_TARGET_WATER_TEMPERATURE = "target_water_temperature"
LOCAL_CONF_BUBBLE_SPEED = "bubble_speed"

# -------------------------
# COMPONENT SETUP
# -------------------------

DEPENDENCIES = ["uart", "binary_sensor", "sensor", "switch", "number"]
AUTO_LOAD = ["binary_sensor", "sensor", "switch", "number"]

mspa_wifi_ns = cg.esphome_ns.namespace("mspa_wifi")

MspaWifi = mspa_wifi_ns.class_("MspaWifi", cg.Component)
MspaSwitch = mspa_wifi_ns.class_("MspaSwitch", switch.Switch)
MspaNumber = mspa_wifi_ns.class_("MspaNumber", number.Number)

# -------------------------
# CONFIG SCHEMA:s
# -------------------------

FLOW_IN_SCHEMA = binary_sensor.binary_sensor_schema(icon=ICON_WATER)
FLOW_OUT_SCHEMA = binary_sensor.binary_sensor_schema(icon=ICON_WATER)

WATER_TEMPERATURE_SCHEMA = sensor.sensor_schema(
    device_class=DEVICE_CLASS_TEMPERATURE,
    icon=ICON_THERMOMETER,
    unit_of_measurement="°C",
    accuracy_decimals=1,
)

TARGET_WATER_TEMPERATURE_SCHEMA = number.number_schema(
    MspaNumber,
    icon=ICON_THERMOMETER,
    unit_of_measurement=UNIT_CELSIUS,
    device_class=DEVICE_CLASS_TEMPERATURE,
).extend(
    {
        cv.GenerateID(): cv.declare_id(MspaNumber),
    }
).extend(
    {
        cv.GenerateID(): cv.declare_id(MspaNumber),
        cv.Optional(CONF_MIN_VALUE, default=20): cv.int_range(min=0, max=40),
    }
)


BUBBLE_SPEED_SCHEMA = (
    number.number_schema(
        MspaNumber,
        icon=ICON_FAN,
        unit_of_measurement="",
        device_class=DEVICE_CLASS_EMPTY,
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(MspaNumber),
            cv.Optional(CONF_MAX_VALUE, default=3): cv.int_range(min=1, max=3),
        }
    )
)

# -------------------------
# ROOT CONFIG SCHEMA
# -------------------------

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(MspaWifi),
        cv.Required(LOCAL_CONF_UART_BOX_TO_REMOTE_ID): cv.use_id(uart.UARTComponent),
        cv.Required(LOCAL_CONF_UART_REMOTE_TO_BOX_ID): cv.use_id(uart.UARTComponent),
        cv.Optional(LOCAL_CONV_UVC_COMMAND, default=0x15): cv.one_of(0x10, 0x15, int),
        cv.Optional(LOCAL_CONF_FLOW_IN): FLOW_IN_SCHEMA,
        cv.Optional(LOCAL_CONF_FLOW_OUT): FLOW_OUT_SCHEMA,
        cv.Optional(LOCAL_CONF_FILTER_PUMP): switch.switch_schema(
            MspaSwitch, icon="mdi:pump"
        ),
        cv.Optional(LOCAL_CONF_UVC): switch.switch_schema(
            MspaSwitch, icon=ICON_LIGHTBULB
        ),
        cv.Optional(LOCAL_CONF_OZONE): switch.switch_schema(
            MspaSwitch, icon="mdi:gas-cylinder"
        ),
        cv.Optional(LOCAL_CONF_HEATER): switch.switch_schema(
            MspaSwitch, icon=ICON_LIGHTBULB
        ),
        cv.Optional(LOCAL_CONF_WATER_TEMPERATURE): WATER_TEMPERATURE_SCHEMA,
        cv.Optional(
            LOCAL_CONF_TARGET_WATER_TEMPERATURE
        ): TARGET_WATER_TEMPERATURE_SCHEMA,
        cv.Optional(LOCAL_CONF_BUBBLE_SPEED): BUBBLE_SPEED_SCHEMA,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # UART:s
    cg.add(
        var.set_box_to_remote_uart(
            await cg.get_variable(config[LOCAL_CONF_UART_BOX_TO_REMOTE_ID])
        )
    )
    cg.add(
        var.set_remote_to_box_uart(
            await cg.get_variable(config[LOCAL_CONF_UART_REMOTE_TO_BOX_ID])
        )
    )

    # Configuration
    if LOCAL_CONV_UVC_COMMAND in config:
        cg.add(var.set_uvc_command(config[LOCAL_CONV_UVC_COMMAND]))

    # Sensors
    if LOCAL_CONF_FLOW_IN in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_FLOW_IN])
        cg.add(var.set_flow_in_binary_sensor(sens))

    if LOCAL_CONF_FLOW_OUT in config:
        sens = await binary_sensor.new_binary_sensor(config[LOCAL_CONF_FLOW_OUT])
        cg.add(var.set_flow_out_binary_sensor(sens))

    if LOCAL_CONF_WATER_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[LOCAL_CONF_WATER_TEMPERATURE])
        cg.add(var.set_water_temperature_sensor(sens))

    # Number controls
    if LOCAL_CONF_TARGET_WATER_TEMPERATURE in config:
        num = await number.new_number(
            config[LOCAL_CONF_TARGET_WATER_TEMPERATURE],
            min_value=config[LOCAL_CONF_TARGET_WATER_TEMPERATURE][CONF_MIN_VALUE],
            max_value=40,
            step=1,
        )
        cg.add(
            num.set_type(
                cg.RawExpression("esphome::mspa_wifi::MspaNumberType::TARGET_TEMP")
            )
        )
        cg.add(var.set_target_water_temperature_number(num))
        cg.add(num.set_mspa(var))

    if LOCAL_CONF_BUBBLE_SPEED in config:
        max_v = config[LOCAL_CONF_BUBBLE_SPEED][CONF_MAX_VALUE]
        num = await number.new_number(
            config[LOCAL_CONF_BUBBLE_SPEED],
            min_value=0,
            max_value=max_v,
            step=1,
        )
        cg.add(
            num.set_type(
                cg.RawExpression("esphome::mspa_wifi::MspaNumberType::BUBBLE_SPEED")
            )
        )
        cg.add(var.set_target_bubble_speed_number(num))
        cg.add(num.set_mspa(var))

    # Switches
    for conf, switch_type in {
            LOCAL_CONF_FILTER_PUMP: "FILTER",
            LOCAL_CONF_HEATER: "HEATER",
            LOCAL_CONF_UVC: "UVC",
            LOCAL_CONF_OZONE: "OZONE",
        }.items():
        sw = await switch.new_switch(config[conf])
        cg.add(getattr(var, f"set_{conf}_switch")(sw))
        cg.add(
            sw.set_mspa(
                var,
                cg.RawExpression(f"esphome::mspa_wifi::MspaSwitchType::{switch_type}"),
            )
        )
