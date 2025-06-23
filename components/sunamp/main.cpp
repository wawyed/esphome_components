#include <iostream>
#include <map>
#include <vector>


class SunampParser {
    std::vector<std::string> raw_vector;
    std::map<std::string, float> myMap{
        {"temp1", 0}, {"temp2", 0}, {"temp3", 0}, {"err", 0}, {"SOHT", 0}, {"ELCD", 0}, {"extD", 0}, {"SOC", 0},
        {"CHG", 0}, {"DC_R1", 0}, {"DC_R2", 0}
    };

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

    float getTemp3() const {
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


std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

void send_value(const std::string &name, const std::string &value) {
    double value_as_float = atof(value.c_str());
    // id(name).publish_state(value);
    std::cout << "name: " << name << ", value: " << value_as_float << std::endl;
}

int main() {
    std::string test =
            "HP58-SG V12.2.0 37/0/0 F:0, TS: 57.66, 57.60, 56.42, err: 0, SOHT: 0, ELCD: 1, extD: 0, SOC: 4, CHG: 0, DC_R1: 0, DC_R2: 0, RLY: 0, RLY1: 0, CL: 0, L3: 0, DSR: 0";
    std::string delimiter = ",";
    std::vector<std::string> v = split(test, delimiter);

    std::cout << test << std::endl;
    std::cout << v.size() << std::endl;

    if (v.size() != 17) {
        return 0;
    }

    std::string temp1_raw = v[1];

    std::vector<std::string> temp1_split = split(temp1_raw, ":");

    send_value("temp1", temp1_split[1]);
    send_value("temp2", v[2]);
    send_value("temp3", v[3]);

    for (int i = 4; i < v.size(); i++) {
        std::vector<std::string> v2 = split(v[i], ":");

        if (v2.size() == 2) {
            std::string name = v2[0].substr(1, v2[0].size());
            send_value(name, v2[1]);
        }
    }

    return 0;
}
