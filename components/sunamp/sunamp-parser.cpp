#include <iostream>
#include <map>
#include <vector>


class SunampParser {
    std::vector<std::string> raw_vector;
    std::map<std::string, float> myMap;

    void _add(const std::string &name, const std::string &value) {
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

public:
    explicit SunampParser(const std::string &text) {
        raw_vector = _split(text, ",");

        std::string temp1_raw = raw_vector[1];

        std::vector<std::string> temp1_split = _split(temp1_raw, ":");

        _add("temp1", temp1_split[1]);
        _add("temp2", raw_vector[2]);
        _add("temp3", raw_vector[3]);

        for (int i = 4; i < raw_vector.size(); i++) {
            std::vector<std::string> v2 = _split(raw_vector[i], ":");

            if (v2.size() == 2) {
                std::string name = v2[0].substr(1, v2[0].size());
                _add(name, v2[1]);
            }
        }
    }

    [[nodiscard]] float getTemp1() const {
        return myMap.at("temp1");
    };

    [[nodiscard]] float getTemp2() const {
        return myMap.at("temp2");
    };

    [[nodiscard]] float getTemp3() const {
        return myMap.at("temp3");
    };

    [[nodiscard]] float getTempAvg() const {
        return (getTemp1() + getTemp2() + getTemp3()) / 3;
    };

    [[nodiscard]] float getSOC() const {
        return myMap.at("SOC") * 25;
    };

    [[nodiscard]] bool getChargeDemand() const {
        return myMap.at("CHG") > 0;
    };
};