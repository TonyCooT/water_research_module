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
 
#include <ph_sensor.hpp>

PhSensor::PhSensor(uint8_t pin, float init = 0.0f, bool isFiltered = false, size_t size = 8): BaseSensor(pin, init),
    m_type(sensor_type::PH), m_isFiltered(isFiltered), m_isAdvanced(false),
    m_analogReference(5.0f), m_rangeADC(1023.0f), m_zero(0.0f), m_k(3.5f),
    m_low(1.15f), m_middle(2.0f), m_high(2.85f),
    m_buffer(nullptr), m_size(size), m_index(0)
{
    if (m_isFiltered)
    {
        m_buffer = new float[m_size];
        
        for(size_t i = 0; i < m_size; i++)
            m_buffer[i] = init;
    }        
}

PhSensor::~PhSensor()
{
    delete[] m_buffer;
}

void PhSensor::change_mode(bool isAdvanced)
{
    m_isAdvanced = isAdvanced;
}

void PhSensor::set_settings(float analogReference = 5.0f, float rangeADC = 1023.0f, float zero = 0.0f, float k = 3.5f)
{
    m_analogReference = analogReference;
    m_rangeADC = rangeADC;
    m_zero = zero;
    m_k = k;
}

void PhSensor::set_advanced_settings(float low = 1.15f, float middle = 2.0f, float high = 2.85f)
{
    m_low = low;
    m_middle = middle;
    m_high = high;
}

void PhSensor::calibrate()
{
    float voltage = m_analogReference * analogRead(BaseSensor::get_pin()) / m_rangeADC;
    m_zero = 7.0f / m_k - voltage;
}

void PhSensor::calibrate_low()
{
    m_low = m_analogReference * analogRead(BaseSensor::get_pin()) / m_rangeADC;
}

void PhSensor::calibrate_middle()
{
    m_middle = m_analogReference * analogRead(BaseSensor::get_pin()) / m_rangeADC;
}

void PhSensor::calibrate_high()
{
    m_high = m_analogReference * analogRead(BaseSensor::get_pin()) / m_rangeADC;
}

void PhSensor::reset_calibration()
{
    if(m_isAdvanced)
    {
        m_low = 1.15f;
        m_middle = 2.0f;
        m_high = 2.85f;
    }
    else
        m_zero = 0.0f;
}

void PhSensor::bubble_sort(float * arr, size_t size) const
{
    for (size_t i = 0; i <= (size / 2 + 1); i++) 
    { 
        for (size_t j = 0; j < (size - i - 1); j++) 
        {
            if (arr[j] > arr[j + 1]) 
            {
                float temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

float PhSensor::get_median_value(float * arr, size_t size) const
{
    float buffer[size];
    for (size_t i = 0; i < size; i++) 
        buffer[i] = arr[i];  
    
    bubble_sort(buffer, size);
    
    float value;
    if (size % 2 == 0)
        value = (buffer[size / 2] + buffer[size / 2 + 1]) / 2;
    else
        value = buffer[size / 2];
    
    return value; 
}

float PhSensor::get_filtered_value(float value)
{
    m_buffer[m_index] = value;
    
    m_index++;
    if (m_index >= m_size) 
        m_index = 0;
    
    float result = get_median_value(m_buffer, m_size);
    
    return result;
}

float PhSensor::read_value()
{
    float value = BaseSensor::get_value();
    
    if (m_isFiltered)
        value = get_filtered_value(value);
    
    return value;
}

void PhSensor::update_value() 
{
    float voltage = m_analogReference * analogRead(BaseSensor::get_pin()) / m_rangeADC;
    
    float value;
    if (m_isAdvanced)
    {
        if (voltage > m_middle)
            value = 7.0f - 3.0f / (m_middle - m_high) * (voltage - m_middle);
        else
            value = 7.0f - 3.0f / (m_low - m_middle) * (voltage - m_middle);
    }
    else
        value = m_k * (voltage + m_zero);
    
    BaseSensor::set_value(value);
}

uint8_t PhSensor::get_sensor_type() const
{
    return static_cast<uint8_t>(m_type);
}
