/***************************************************
 Water Research Module
 <https://github.com/TonyCooT/water_research_module>
 
 ***************************************************
 The code below describes the methods of working with a total dissolved solids (TDS) sensor.
 For example, the TDS-meter (Troyka-module) by LLC "Amperka" is selected.
 The sensor is calibrated with standard buffer solution (707ppm(1413us/cm) @ 25^c is recommended).

 Created 2022-09-28
 By TonyCooT <https://github.com/TonyCooT>
 
 Apache License 2.0.
 See <https://www.apache.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/
 
#ifndef TDS_SENSOR_HPP
#define TDS_SENSOR_HPP

#include <base_sensor.hpp>

class TdsSensor final : public BaseSensor
{
    private:
    sensor_type m_type;
    bool m_isFiltered;
    
    private:
    float m_analogReference;
    float m_rangeADC;
    float m_kProbe;
    float m_kTemp;
    float m_kTds;
    float m_temperature;
    
    private:
    float * m_buffer;
    size_t m_size;
    size_t m_index;

    private:
    float get_middle_value(float * arr, size_t size) const;
    float get_filtered_value(float value);
    
    public:
    explicit TdsSensor(uint8_t pin, float init = 0.0f, bool isFiltered = false, size_t size = 8);
    ~TdsSensor();
    
    public:
    void set_settings(float analogReference = 5.0f, float rangeADC = 1023.0f, float kProbe = 0.5f, float kTemp = 0.02f, float kTds = 0.65f);
    void set_temperature(float value);
    
    public:
    void calibrate(float value);
    void reset_calibration();
    
    public:
    float read_value() override;
    void update_value() override;
    uint8_t get_sensor_type() const override;
};

#endif // !TDS_SENSOR_HPP
