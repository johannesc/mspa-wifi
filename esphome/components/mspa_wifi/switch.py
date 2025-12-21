import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch

from . import CONF_MSPA_WIFI_ID, MspaWifiComponent, mspa_wifi_ns

from esphome.const import (
    ICON_WATER,
    ICON_HEATING_COIL,
    ICON_LIGHTBULB,
)

LOCAL_CONF_FILTER_PUMP = "filter_pump"
LOCAL_CONF_HEATER = "heater"
LOCAL_CONF_UVC = "uvc"
LOCAL_CONF_OZONE = "ozone"

_CMD_ID_HEATER = 1
_CMD_ID_FILTER = 2
_CMD_ID_UVC = 0x15
_CMD_ID_OZONE = 0xE

_SWITCHES = {
    LOCAL_CONF_FILTER_PUMP: _CMD_ID_FILTER,
    LOCAL_CONF_HEATER: _CMD_ID_HEATER,
    LOCAL_CONF_UVC: _CMD_ID_UVC,
    LOCAL_CONF_OZONE: _CMD_ID_OZONE,
}

MspaSwitch = mspa_wifi_ns.class_('MspaSwitch', switch.Switch, cg.Component)


CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_MSPA_WIFI_ID): cv.use_id(MspaWifiComponent),
    cv.Optional(LOCAL_CONF_FILTER_PUMP): switch.switch_schema(switch.Switch,
        icon=ICON_WATER,
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
    cv.Optional(LOCAL_CONF_HEATER): switch.switch_schema(switch.Switch,
        icon=ICON_HEATING_COIL,
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
    cv.Optional(LOCAL_CONF_UVC): switch.switch_schema(switch.Switch,
        icon=ICON_LIGHTBULB,
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
    cv.Optional(LOCAL_CONF_OZONE): switch.switch_schema(switch.Switch,
        icon="mdi:gas-cylinder",
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
})

async def to_code(config):
    MspaWifi_component = await cg.get_variable(config[CONF_MSPA_WIFI_ID])

    for conf, cmd_id in _SWITCHES.items():
        var = await switch.new_switch(config[conf])
        cg.add(getattr(MspaWifi_component, f"set_{conf}_switch")(var))
        cg.add(var.set_mspa(MspaWifi_component, cmd_id))
