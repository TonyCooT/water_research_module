/***************************************************
 Water Research Module
 <https://github.com/TonyCooT/water_research_module>
 
 ***************************************************
 The code below is an example program for an Arduino family microcontroller. 
 The microcontroller communicates with the GUI via the serial port, configures sensors and sends registered data.

 Created 2022-09-28
 By TonyCooT <https://github.com/TonyCooT>
 
 Apache License 2.0.
 See <https://www.apache.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

#include <temp_sensor.hpp>
#include <ph_sensor.hpp>
#include <tds_sensor.hpp>

const uint8_t PREFIX = 0x53;    // 'S'
const uint8_t IN_MESSAGE_SIZE = 6;
const uint8_t OUT_MESSAGE_SIZE = 5;

uint8_t buf[IN_MESSAGE_SIZE];

enum class sensor_type: uint8_t {TEMP,PH,TDS};
enum class cmd_type: uint8_t {RESET,CAL,CAL_LOW,CAL_MIDDLE,CAL_HIGH,MODE};

TempSensor temp(10, 25.0f, true, 8);
PhSensor ph(A1, 7.0f, true, 8);
TdsSensor tds(A3, 0.0, true, 8);

static void parse_data();
static void send_data(uint8_t typeSensor, float value);

void setup()
{
    ph.set_settings(5.0f, 1023.0f, 0.0f, 3.5f);
    tds.set_settings(5.0f, 1023.0f, 0.5f, 0.02f, 0.65f);
    
    Serial.begin(115200);
}

void loop()
{
    parse_data();
    
    static uint32_t samplingTime = millis();
    static uint32_t printTime = millis();
    
    if (millis() - samplingTime >= 5000) 
    {
        samplingTime = millis();
        
        float temperature = temp.update_value();
        ph.update_value();
        tds.set_temperature(temperature);
        tds.update_value();
    }
    
    if (millis() - printTime >= 1000) 
    {
        printTime = millis();
        
        send_data(temp.get_sensor_type(), temp.read_value() * 10.0f);
        send_data(ph.get_sensor_type(), ph.read_value() * 10.0f);
        send_data(tds.get_sensor_type(), tds.read_value() * 10.0f);
    }
}

static uint8_t get_sum(const uint8_t * arr, uint8_t len)
{
    uint8_t sum = 0;
    
    for (uint8_t i = 0; i < len; ++i)
        sum += arr[i]; 
    
    return sum;
}

static void parse_data()
{
    if (Serial.available() > IN_MESSAGE_SIZE)
    {
        Serial.readBytes(buf, IN_MESSAGE_SIZE);
        
        if ((buf[0] != PREFIX) && (buf[IN_MESSAGE_SIZE - 1] != get_sum(buf, (IN_MESSAGE_SIZE - 1))))
            return;
        
        if (buf[1] == static_cast<uint8_t>(sensor_type::TEMP))
            return;
        else if (buf[1] == static_cast<uint8_t>(sensor_type::PH))
        {
            switch (buf[2])
            {
                case static_cast<uint8_t>(cmd_type::RESET):
                    ph.reset_calibration();
                    break;
                case static_cast<uint8_t>(cmd_type::CAL):
                    ph.calibrate();
                    break;
                case static_cast<uint8_t>(cmd_type::CAL_LOW):
                    ph.calibrate_low();
                    break;
                case static_cast<uint8_t>(cmd_type::CAL_MIDDLE):
                    ph.calibrate_middle();
                    break;
                case static_cast<uint8_t>(cmd_type::CAL_HIGH):
                    ph.calibrate_high();
                    break;
                case static_cast<uint8_t>(cmd_type::MODE):
                    ph.change_mode(static_cast<bool>(buf[3]));
                    break;
                default:
                    break;
            }
        }
        else if (buf[1] == static_cast<uint8_t>(sensor_type::TDS))
        {
            switch (buf[2])
            {
                case static_cast<uint8_t>(cmd_type::RESET):
                    tds.reset_calibration();
                    break;
                case static_cast<uint8_t>(cmd_type::CAL):
                    float value = (static_cast<uint16_t>(buf[3]) << 8) + static_cast<uint16_t>(buf[4]);
                    tds.calibrate(value / 10.0f);
                    break;
                default:
                    break;
            }
        }
    }
}

static void send_data(uint8_t type, float value)
{
    uint8_t message[OUT_MESSAGE_SIZE] = {PREFIX, type};
    
    uint16_t data = value;
    message[2] = data >> 8;
    message[3] = data & 0xFF;
    
    message[OUT_MESSAGE_SIZE - 1] = get_sum(message, (OUT_MESSAGE_SIZE - 1));
    
    Serial.write(message, OUT_MESSAGE_SIZE);
}
