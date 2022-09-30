/***************************************************
 Water Research Module
 <https://github.com/TonyCooT/water_research_module>
 
 ***************************************************
 The code below describes the methods of working with a temperature sensor.
 For example, the DS18B20 by Maxim Integrated Products, Inc. is selected.
 
 Created 2022-09-28
 By TonyCooT <https://github.com/TonyCooT>
 
 Apache License 2.0.
 See <https://www.apache.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <base_sensor.hpp>
#include <OneWire.h>
#include <DallasTemperature.h>

class TempSensor final : public BaseSensor
{
    private:
    sensor_type m_type;
    bool m_isFiltered;
    
    private:
    OneWire m_bus;
    DallasTemperature m_sensor;
    
    private:
    float * m_buffer;
    size_t m_size;
    size_t m_index;

    private:
    float get_middle_value(float * arr, size_t size) const;
    float get_filtered_value(float value);
    
    public:
    explicit TempSensor(uint8_t pin, float init = 0.0f, bool isFiltered = false, size_t size = 8);
    ~TempSensor();
    
    public:
    float read_value() override;
    void update_value() override;
    uint8_t get_sensor_type() const override;
};

#endif // !TEMP_SENSOR_H
