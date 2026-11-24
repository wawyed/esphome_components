import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.components import sensor
from esphome.components import text_sensor
from esphome.const import CONF_ID, CONF_SENSORS, ENTITY_CATEGORY_DIAGNOSTIC

CODEOWNERS = ["@wawyed"]

DEPENDENCIES = ['uart']
AUTO_LOAD = ['text_sensor']

CONF_RAW = 'raw'

serial_ns = cg.esphome_ns.namespace('serial')

Sunamp = serial_ns.class_('Sunamp', cg.Component, sensor.Sensor, uart.UARTDevice)


CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(Sunamp),
        cv.Optional(CONF_RAW): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Required(CONF_SENSORS): cv.ensure_list(
             sensor.sensor_schema().extend(
                {
                    cv.Required("sunamp_id"): cv.string,
                }
            )
        ),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    if CONF_RAW in config:
        raw = await text_sensor.new_text_sensor(config[CONF_RAW])
        cg.add(var.set_raw_sensor(raw))
    for conf in config[CONF_SENSORS]:
        sens = await sensor.new_sensor(conf)
        sunamp_id = conf["sunamp_id"]
        cg.add(var.add_sensor(sunamp_id, sens))
