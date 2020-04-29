#include "I2CSlave.h"

char i2c::dataBuffer[DATA_BUFFER_SIZE];
char i2c::commandBuffer[RESPONSE_BUFFER_SIZE];

i2c::TransmissionStep i2c::transmissionStep = EMPTY;
i2c::Order i2c::order = NONE;

void i2c::begin(int address)
{
    Wire.begin(address);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
}

void i2c::receiveEvent(int count)
{
    static int size = 0; // amount of currently readed bytes
    if (transmissionStep != ONGOING)
    {
        memset(commandBuffer, 0, RESPONSE_BUFFER_SIZE); // clear buffer
        transmissionStep = ONGOING;                     // change current transmission status
    }

    // read incoming data
    char c;
    while (Wire.available())
    {
        c = Wire.read();
        commandBuffer[size] = c;
        size++;
    }

    // check if last char is transmission terminator
    if (c == '#')
    {
        size = 0;                    // restore buffer
        transmissionStep = FINISHED; // end transmission
        order = parseOrder();
    }
    return;
}

void i2c::requestEvent()
{
    static int step = 0;
    static int package_start = 0; // from which point start data transfer
    static int length;            // length of data to be send

    int package_size = 32; // maximum package size (derived from Wire.h)
    char package[33];      // empty char array for package data

    if (step == 0)
    {
        // get length of the data
        char post_data_length[4];
        length = strlen(dataBuffer);
        sprintf(post_data_length, "%03d", length);
        Wire.write(post_data_length); //notify master about data length

        transmissionStep = length > 0 ? ONGOING : FINISHED;
        step += length > 0 ? 1 : 0; // end first step of data sending
    }
    else if (step == 1)
    {
        // send package to the master
        package_size = package_size < length ? package_size : length; // determine amount of data to be send
        substring(package, dataBuffer, package_start, package_size);  // copy part of the data

        Wire.write(package); // send package to the master

        package_start += package_size; // increment package start
        length -= package_size;        // shorten remaining data to be send

        // check if data was send completly
        if (length <= 0)
        {
            // restore initials
            step = 0;
            package_start = 0;
            transmissionStep = FINISHED;
        }
    }
    return;
}

void i2c::addToBuffer(const char *data)
{
    // check bytes available in the buffer
    unsigned int buffer_length = strlen(dataBuffer);

    // check if buffer has enough space
    if ((buffer_length > DATA_BUFFER_SIZE - 1) ||
        (buffer_length + strlen(data) > DATA_BUFFER_SIZE - 1))
    {
        Logger::log(F("Not enough space in buffer"), LogLevel::WARNING);
        return; // avoid buffer overwride
    }
    else if (buffer_length > 0)
    {
        strcat(dataBuffer, ","); // add data object separator
    }
    strcat(dataBuffer, data);
}

i2c::Order i2c::parseOrder()
{
    char order_id = commandBuffer[0]; // first char stands for order to be executed

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

void i2c::clearBuffer()
{
    memset(commandBuffer, 0, RESPONSE_BUFFER_SIZE);
    memset(dataBuffer, 0, DATA_BUFFER_SIZE);
}
