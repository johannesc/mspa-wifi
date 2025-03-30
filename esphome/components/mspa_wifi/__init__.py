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


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MspaWifiComponent),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
