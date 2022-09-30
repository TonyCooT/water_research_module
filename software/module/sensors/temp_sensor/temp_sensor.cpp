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
 
#include <temp_sensor.hpp>

TempSensor::TempSensor(uint8_t pin, float init = 0.0f, bool isFiltered = false, size_t size = 8): BaseSensor(pin, init),
    m_type(sensor_type::TEMP), m_isFiltered(isFiltered),
    m_bus(OneWire(pin)), m_sensor(DallasTemperature(&m_bus)),
    m_buffer(nullptr), m_size(size), m_index(0)
{
    m_sensor.begin();
    
    if (m_isFiltered)
    {
        m_buffer = new float[m_size];
        
        for(size_t i = 0; i < m_size; i++)
            m_buffer[i] = init;
    }
}

TempSensor::~TempSensor()
{
    delete[] m_buffer;
}

float TempSensor::get_middle_value(float * arr, size_t size) const
{
    float value = 0;
    for (size_t i = 0; i < size; i++)
        value += arr[i];
    
    value /= size;
    
    return value; 
}

float TempSensor::get_filtered_value(float value)
{
    m_buffer[m_index] = value;
    
    m_index++;
    if (m_index >= m_size) 
        m_index = 0;
    
    float filteredValue = get_middle_value(m_buffer, m_size);
    
    return filteredValue;
}

float TempSensor::read_value()
{
    float value = BaseSensor::get_value();
    
    if (m_isFiltered)
        value = get_filtered_value(value);
    
    return value;
}

void TempSensor::update_value() 
{
    m_sensor.requestTemperatures();
    BaseSensor::set_value(m_sensor.getTempCByIndex(0));
}

uint8_t TempSensor::get_sensor_type() const
{
    return static_cast<uint8_t>(m_type);
}
