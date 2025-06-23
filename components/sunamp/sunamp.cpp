#include "sunamp.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "esphome/core/log.h"

namespace esphome {
    namespace serial {
        static const char *TAG = "sunamp stream";

        void Sunamp::loop() {
            while (this->available()) {
                uint8_t c;
                this->read_byte(&c);
                if (c == '\r')
                    continue;
                if (c == '\n')
                    this->parse_values_();
                else
                    this->rx_message_.push_back(c);
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
            std::vector<std::string> values;

            std::vector<std::string> raw_vector = _split(s, ",");

            std::string temp1_raw = raw_vector[1];

            std::vector<std::string> temp1_split = _split(temp1_raw, ":");

            _add("temp1", temp1_split[1]);
            _add("temp2", raw_vector[2]);
            _add("temp3", raw_vector[3]);
            myMap["tempAvg"] = (myMap["temp1"] + myMap["temp2"] + myMap["temp3"]) / 3;

            for (int i = 4; i < raw_vector.size(); i++) {
                std::vector<std::string> v2 = _split(raw_vector[i], ":");

                if (v2.size() == 2) {
                    std::string name = v2[0].substr(1, v2[0].size());
                    _add(name, v2[1]);
                }
            }


            this->rx_message_.clear();
            for (auto sens: this->sensors_) {
                sens.second->publish_state(myMap.at[sens.first]);
            }
        }

        void Sunamp::dump_config() {
            ESP_LOGCONFIG("", "Sunamp Serial Reader");
            for (auto sens: this->sensors_) {
                ESP_LOGCONFIG(TAG, "Index %d", sens.first);
                LOG_SENSOR(TAG, "", sens.second);
            }
        }
    } // namespace serial
} // namespace esphome
