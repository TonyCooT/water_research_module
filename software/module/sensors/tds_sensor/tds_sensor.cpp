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
 
#include <tds_sensor.hpp>

TdsSensor::TdsSensor(uint8_t pin = A0, float init = 0.0f, bool isFiltered = false, size_t size = 8): BaseSensor(pin, init),
    m_type(sensor_type::TDS), m_isFiltered(isFiltered),
    m_analogReference(5.0f), m_rangeADC(1023.0f), m_kProbe(0.5f), m_kTemp(0.02f), m_kTds(0.65f), m_temperature(25.0f),
    m_buffer(nullptr), m_size(size), m_index(0)
{
    if (m_isFiltered)
    {
        m_buffer = new float[m_size];
        
        for(size_t i = 0; i < m_size; i++)
            m_buffer[i] = init;
    }        
}

TdsSensor::~TdsSensor()
{
    delete[] m_buffer;
}

void TdsSensor::set_settings(float analogReference = 5.0f, float rangeADC = 1023.0f, float kProbe = 0.5f, float kTemp = 0.02f, float kTds = 0.65f)
{
    m_analogReference = analogReference;
    m_rangeADC = rangeADC;
    m_kProbe = kProbe;
    m_kTemp = kTemp;
    m_kTds = kTds;
}

void TdsSensor::set_temperature(float value)
{
    m_temperature = value;
}

void TdsSensor::calibrate(float value)
{
    float voltage = m_analogReference * analogRead(BaseSensor::get_pin()) / m_rangeADC;
    
    float rawEc = value / m_kTds;
    rawEc = rawEc * (1.0f + m_kTemp * (m_temperature - 25.0f));
    
    m_kProbe = rawEc / (133.42f * powf(voltage, 3.0f) - 255.86f * powf(voltage, 2.0f) + 857.39f * voltage);
}

void TdsSensor::reset_calibration()
{
    m_kProbe = 0.5f;
}

float TdsSensor::get_middle_value(float * arr, size_t size) const
{
    float value = 0;
    for (size_t i = 0; i < size; i++)
        value += arr[i];
    
    value /= size;
    
    return value; 
}

float TdsSensor::get_filtered_value(float value)
{
    m_buffer[m_index] = value;
    
    m_index++;
    if (m_index >= m_size) 
        m_index = 0;
    
    float filteredValue = get_middle_value(m_buffer, m_size);
    
    return filteredValue;
}

float TdsSensor::read_value()
{
    float value = BaseSensor::get_value();
    
    if (m_isFiltered)
        value = get_filtered_value(value);
    
    return value;
}

void TdsSensor::update_value() 
{
    float voltage = m_analogReference * analogRead(BaseSensor::get_pin()) / m_rangeADC;
    
    float ec = m_kProbe * (133.42f * powf(voltage, 3.0f) - 255.86f * powf(voltage, 2.0f) + 857.39f * voltage);
    ec = ec / (1.0f + m_kTemp * (m_temperature - 25.0f));
    
    float value = m_kTds * ec;
    
    BaseSensor::set_value(value);
}

uint8_t TdsSensor::get_sensor_type() const
{
    return static_cast<uint8_t>(m_type);
}
