import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_NAME, CONF_ID
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
LOCAL_CONF_BUBBLE = "bubble"

_CMD_ID_HEATER = 1
_CMD_ID_FILTER = 2
_CMD_ID_UVC = 0x10
_CMD_ID_OZONE = 0xE
_CMD_ID_BUBBLE = 0x03

_SWITCHES = {
    LOCAL_CONF_FILTER_PUMP: _CMD_ID_FILTER,
    LOCAL_CONF_HEATER: _CMD_ID_HEATER,
    LOCAL_CONF_UVC: _CMD_ID_UVC,
    #LOCAL_CONF_OZONE: _CMD_ID_OZONE,
    LOCAL_CONF_BUBBLE: _CMD_ID_BUBBLE,
}

MspaSwitch = mspa_wifi_ns.class_('MspaSwitch', switch.Switch, cg.Component)


CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_MSPA_WIFI_ID): cv.use_id(MspaWifiComponent),
    cv.Optional(LOCAL_CONF_FILTER_PUMP): switch.switch_schema(switch.Switch,
        icon="mdi:pump",
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
    cv.Optional(LOCAL_CONF_HEATER): switch.switch_schema(switch.Switch,
        icon=ICON_HEATING_COIL,
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
    cv.Optional(LOCAL_CONF_UVC): switch.switch_schema(switch.Switch,
        icon=ICON_LIGHTBULB,
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
    # cv.Optional(LOCAL_CONF_OZONE): switch.switch_schema(switch.Switch,
        # icon="mdi:gas-cylinder",
    # ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
    cv.Optional(LOCAL_CONF_BUBBLE): switch.switch_schema(switch.Switch,
        icon="mdi:fan",
    ).extend({cv.GenerateID(): cv.declare_id(MspaSwitch),}),
})

async def to_code(config):
    MspaWifi_component = await cg.get_variable(config[CONF_MSPA_WIFI_ID])

    for conf, cmd_id in _SWITCHES.items():
        var = await switch.new_switch(config[conf])
        cg.add(getattr(MspaWifi_component, f"set_{conf}_switch")(var))
        cg.add(var.set_mspa(MspaWifi_component, cmd_id))


    # if LOCAL_CONF_FILTER_PUMP in config:
    #     cfg = config[LOCAL_CONF_FILTER_PUMP]
    #     var = await switch.new_switch(cfg)
    #     cg.add(MspaWifi_component.set_filter_pump_switch(var))
    #     cg.add(var.set_mspa(MspaWifi_component, _CMD_ID_FILTER))

