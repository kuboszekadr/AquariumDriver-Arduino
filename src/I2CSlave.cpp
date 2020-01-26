#include "I2CSlave.h"

char i2c::dataBuffer[512];
char i2c::commandBuffer[128];
// char i2c::responseBuffer[128];

i2c::TransmissionStep i2c::transmissionStep = EMPTY;
i2c::Order i2c::order = UNKNOWN;

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
        memset(commandBuffer, 0, 128); // clear buffer
        transmissionStep = ONGOING;    // change current transmission status
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
        step++;                       // end first step of data sending
    }
    else if (step == 1)
    {
        // send package to the master
        package_size = package_size < length ? package_size : length;    // determine amount of data to be send
        substring(package, dataBuffer, package_start, package_size); // copy part of the data

        Wire.write(package); // send package to the master

        package_start += package_size; // increment package start
        length -= package_size;        // shorten remaining data to be send

        // check if data was send
        if (length <= 0)
        {
            // restore defaults
            step = 0;
            package_start = 0;
        }
    }
    return;
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
        return Order::UPDATE_WIFI_STATUS;
    case '2':
        return Order::WATER_CHANGE;
    default:
        return Order::UNKNOWN;
    }
}

void i2c::clearBuffer()
{
    memset(commandBuffer, 0, 128);
}