#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace serial {

class Sunamp : public Component,  public uart::UARTDevice {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }
  void loop() override;

  void _add(const std::string &name, const std::string &value);

  void dump_config() override;

  void add_sensor(std::string sunamp_id, sensor::Sensor *sens) {
    this->sensors_.push_back(std::make_pair(sunamp_id, sens));
  }

  // Optional diagnostic text sensor that publishes the raw line received from the Sunamp.
  void set_raw_sensor(text_sensor::TextSensor *raw_sensor) { this->raw_sensor_ = raw_sensor; }

 protected:
  void parse_values_();
  std::vector<uint8_t> rx_message_;
  std::vector<std::pair<std::string, sensor::Sensor *>> sensors_;
  std::map<std::string, float> myMap;
  std::set<std::string> missing_warned_;
  text_sensor::TextSensor *raw_sensor_{nullptr};
};

}  // namespace serial
}  // namespace esphome
