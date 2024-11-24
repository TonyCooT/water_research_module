/***************************************************
 Water Research Module
 <https://github.com/TonyCooT/water_research_module>
 
 ***************************************************
 The code below describes the methods of working with a pH sensor.
 For example, the E-201-C (Troyka-module) by LLC "Amperka" is selected.
 The sensor is calibrated with standard buffer solution (4,7,10pH @ 25^c is recommended).

 Created 2022-09-28
 By TonyCooT <https://github.com/TonyCooT>
 
 Apache License 2.0.
 See <https://www.apache.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/
 
#ifndef PH_SENSOR_HPP
#define PH_SENSOR_HPP
 
#include <base_sensor.hpp>

class PhSensor final : public BaseSensor
{
    private:
    sensor_type m_type;
    bool m_isFiltered;
    bool m_isAdvanced;
    
    private:
    float m_analogReference;
    float m_rangeADC;
    float m_zero;
    float m_k;
    
    private:
    float m_low;
    float m_middle;
    float m_high;
    
    private:
    float * m_buffer;
    size_t m_size;
    size_t m_index;
    
    private:
    void bubble_sort(float * arr, size_t size) const;
    float get_median_value(float * arr, size_t size) const;
    float get_filtered_value(float value);
    
    public:
    explicit PhSensor(uint8_t pin, float init = 0.0f, bool isFiltered = false, size_t size = 8);
    ~PhSensor();
    
    public:
    void change_mode(bool isAdvanced);
    void set_settings(float analogReference = 5.0f, float rangeADC = 1023.0f, float zero = 0.0f, float k = 3.5f);
    void set_advanced_settings(float low = 1.15f, float middle = 2.0f, float high = 2.85f);
    
    public:
    void calibrate();
    void calibrate_low();
    void calibrate_middle();
    void calibrate_high();
    void reset_calibration();
    
    public:
    float read_value() override;
    void update_value() override;
    uint8_t get_sensor_type() const override;
};

#endif // !PH_SENSOR_HPP
