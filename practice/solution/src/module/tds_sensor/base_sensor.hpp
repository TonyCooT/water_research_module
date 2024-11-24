/***************************************************
 Water Research Module
 <https://github.com/TonyCooT/water_research_module>
 
 ***************************************************
 The code below describes the basic methods of working with any type of sensor.

 Created 2022-09-28
 By TonyCooT <https://github.com/TonyCooT>
 
 Apache License 2.0.
 See <https://www.apache.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

#ifndef BASE_SENSOR_HPP
#define BASE_SENSOR_HPP

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class BaseSensor
{
    private:
    uint8_t m_pin;
    float m_value;
    
    protected:
    enum class sensor_type: uint8_t {TEMP,PH,TDS,TURBIDITY,DO,ORP};
    
    protected:
    uint8_t get_pin() const { return m_pin; }
    float get_value() const { return m_value; }
    void set_value(float value) { m_value = value; }
    
    public:
    BaseSensor(uint8_t pin, float init = 0.0f): m_pin(pin), m_value(init) {}
    BaseSensor(const BaseSensor&) = delete;
    BaseSensor& operator=(const BaseSensor&) = delete;
    virtual ~BaseSensor() {}
    
    public:
    virtual float read_value() = 0;
    virtual void update_value() = 0;
    virtual uint8_t get_sensor_type() const = 0;
};

#endif // !BASE_SENSOR_HPP
