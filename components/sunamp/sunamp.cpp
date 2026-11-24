#include "sunamp.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "esphome/core/log.h"

namespace esphome {
    namespace serial {
        static const char *TAG = "sunamp stream";

        // A real line is well under 200 characters. Anything much longer means the line ending was lost.
        static const size_t MAX_LINE_LENGTH = 512;

        void Sunamp::loop() {
            while (this->available()) {
                uint8_t c;
                this->read_byte(&c);
                if (c == '\r')
                    continue;
                if (c == '\n')
                    this->parse_values_();
                else if (this->rx_message_.size() < MAX_LINE_LENGTH)
                    this->rx_message_.push_back(c);
                else
                    this->rx_message_.clear();
            }
        }

        void Sunamp::_add(const std::string &name, const std::string &value) {
            float value_as_float = atof(value.c_str());
            myMap[name] = value_as_float;
        }

        static std::vector<std::string> _split(const std::string &s, const std::string &delimiter) {
            size_t pos_start = 0, pos_end, delim_len = delimiter.length();
            std::vector<std::string> res;

            while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
                std::string token = s.substr(pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                res.push_back(token);
            }

            res.push_back(s.substr(pos_start));
            return res;
        }

        void Sunamp::parse_values_() {
            std::string s(this->rx_message_.begin(), this->rx_message_.end());
            // Clear first so that every return path below leaves the buffer empty for the next line.
            this->rx_message_.clear();
            if (s.empty())
                return;

            // Publish the raw line so unknown fields can be inspected in the logs and in Home Assistant.
            ESP_LOGD(TAG, "raw: %s", s.c_str());
            if (this->raw_sensor_ != nullptr)
                this->raw_sensor_->publish_state(s);

            std::vector<std::string> raw_vector = _split(s, ",");
            if (raw_vector.size() < 4) {
                ESP_LOGW(TAG, "ignoring short line (%d fields): %s", (int) raw_vector.size(), s.c_str());
                return;
            }

            std::vector<std::string> temp1_split = _split(raw_vector[1], ":");
            if (temp1_split.size() < 2) {
                ESP_LOGW(TAG, "ignoring line with unexpected first field: %s", s.c_str());
                return;
            }

            _add("temp1", temp1_split[1]);
            _add("temp2", raw_vector[2]);
            _add("temp3", raw_vector[3]);
            myMap["tempAvg"] = (myMap["temp1"] + myMap["temp2"] + myMap["temp3"]) / 3;

            for (size_t i = 4; i < raw_vector.size(); i++) {
                std::vector<std::string> v2 = _split(raw_vector[i], ":");

                if (v2.size() == 2 && !v2[0].empty()) {
                    // the first character of the name is a prefix marker and is dropped
                    std::string name = v2[0].substr(1);
                    _add(name, v2[1]);
                }
            }

            for (auto &sens : this->sensors_) {
                auto it = myMap.find(sens.first);
                if (it == myMap.end()) {
                    // not in the stream (yet): skip instead of crashing, and say so once
                    if (this->missing_warned_.insert(sens.first).second)
                        ESP_LOGW(TAG, "sensor '%s' not found in the stream", sens.first.c_str());
                    continue;
                }
                sens.second->publish_state(it->second);
            }
        }

        void Sunamp::dump_config() {
            ESP_LOGCONFIG("", "Sunamp Serial Reader");
            for (auto &sens : this->sensors_) {
                ESP_LOGCONFIG(TAG, "Index %s", sens.first.c_str());
                LOG_SENSOR(TAG, "", sens.second);
            }
            if (this->raw_sensor_ != nullptr)
                LOG_TEXT_SENSOR(TAG, "Raw", this->raw_sensor_);
        }
    } // namespace serial
} // namespace esphome
