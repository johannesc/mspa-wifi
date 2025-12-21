import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart

from esphome.const import CONF_ID

DEPENDENCIES = ['uart']

MULTI_CONF = True

mspa_wifi_ns = cg.esphome_ns.namespace("mspa_wifi")
MspaWifiComponent = mspa_wifi_ns.class_("MspaWifi", cg.Component, uart.UARTDevice)

CONF_MSPA_WIFI_ID = "mspa_wifi_id"

LOCAL_CONF_WATER_TEMP = "water_temp"

LOCAL_CONF_UART_BOX_TO_REMOTE_ID = "uart_box_to_remote_id"
LOCAL_CONF_UART_REMOTE_TO_BOX_ID = "uart_remote_to_box_id"

UART_FROM_BOX_DEVICE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(LOCAL_CONF_UART_BOX_TO_REMOTE_ID): cv.use_id(uart.UARTComponent),
    }
)

UART_FROM_BOX_REMOTE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(LOCAL_CONF_UART_REMOTE_TO_BOX_ID): cv.use_id(uart.UARTComponent),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MspaWifiComponent),
        }
    )
    .extend(UART_FROM_BOX_DEVICE_SCHEMA)
    .extend(UART_FROM_BOX_REMOTE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)

    parent = await cg.get_variable(config[LOCAL_CONF_UART_BOX_TO_REMOTE_ID])
    cg.add(var.set_box_to_remote_uart(parent))

    parent = await cg.get_variable(config[LOCAL_CONF_UART_REMOTE_TO_BOX_ID])
    cg.add(var.set_remote_to_box_uart(parent))

