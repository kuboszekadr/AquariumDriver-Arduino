#include "I2CSlave.h"

char i2c::data_buffer[I2C_DATA_BUFFER_SIZE];
char i2c::command_buffer[I2C_COMMAND_BUFFER_SIZE];

i2c::TransmissionStep i2c::transmission_step = EMPTY;
i2c::Order i2c::order = NONE;

uint32_t i2c::last_request = 0L;

void i2c::begin(int address)
{
    Wire.begin(address);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
}

void i2c::receiveEvent(int count)
{
    static int size = 0; // amount of currently readed bytes
    if (transmission_step != ONGOING)
    {
        memset(command_buffer, 0, I2C_COMMAND_BUFFER_SIZE); // clear buffer
        transmission_step = ONGOING;                        // change current transmission status
    }

    // read incoming data
    char c;
    while (Wire.available())
    {
        c = Wire.read();
        command_buffer[size] = c;
        size++;
    }

    // check if last char is transmission terminator
    if (c == '#')
    {
        size = 0;                     // restore buffer
        transmission_step = FINISHED; // end transmission
        order = parseOrder();
    }

    last_request = millis();
    return;
}

void i2c::requestEvent()
{
    static int step = 0;
    static int package_start = 0; // from which point start data transfer

    static union
    {
        uint16_t length;
        uint8_t length_byte[4];
    };

    int package_size = 32; // maximum package size (derived from Wire.h)
    char package[33] = {}; // empty char array for package data

    if (step == 0)
    {
        // get length of the data, and respond as fast as possible
        length = strlen(data_buffer); 
        length += length > 0 ? 1 : 0; // add one to include closing braclet
        Wire.write(length_byte, 4);       //notify master about data length

        if (length > 1)
        {
            data_buffer[length-1] = ']'; // close the JSON array
        }

        transmission_step = length > 1 ? ONGOING : FINISHED;
        step += length > 1 ? 1 : 0; // end first step of data sending
    }
    else if (step == 1)
    {
        // send package to the master
        package_size = package_size < length ? package_size : length; // determine amount of data to be send
        substring(package, data_buffer, package_start, package_size); // copy part of the data

        Wire.write(package); // send package to the master

        package_start += package_size; // increment package start
        length -= package_size;        // shorten remaining data to be send

        // check if data was send completly
        if (length <= 0)
        {
            // restore initials
            step = 0;
            package_start = 0;
            transmission_step = FINISHED;
        }
    }

    last_request = millis();  // to track potential timeout
    return;
}

void i2c::addToBuffer(const char *data)
{
    // check bytes available in the buffer
    unsigned int buffer_length = strlen(data_buffer);

    // check if buffer has enough space
    if ((buffer_length > I2C_DATA_BUFFER_SIZE - 2) ||
        (buffer_length + strlen(data) > I2C_DATA_BUFFER_SIZE - 2))
    {
        Logger::log(F("Not enough space in buffer"), LogLevel::WARNING);
        return; // avoid buffer overwride
    }
    else if (buffer_length > 1)
    {
        strcat(data_buffer, ","); // add data object separator
    }
    else if (buffer_length == 0)
    {
        strcat(data_buffer, "["); // add data object separator
    }
    strcat(data_buffer, data);
}

i2c::Order i2c::parseOrder()
{
    char order_id = command_buffer[0]; // first char stands for order to be executed

    // maps command char to order type
    switch (order_id)
    {
    case '0':
        return Order::UPDATE_RTC;
    case '1':
        return Order::WATER_CHANGE;
    default:
        return Order::NONE;
    }
}

void i2c::clearBuffers()
{
    memset(command_buffer, 0, I2C_COMMAND_BUFFER_SIZE);
    memset(data_buffer, 0, I2C_DATA_BUFFER_SIZE);
}

bool i2c::isTimeouted()
{
    return (transmission_step == ONGOING) && (millis() - last_request >= I2C_TIMEOUT);
}