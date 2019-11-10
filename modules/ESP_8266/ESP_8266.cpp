#include "ESP_8266.h"

ESP::ESP(int rx_pin,  int tx_pin)
{
    _self = new SoftwareSerial (rx_pin, tx_pin);
    _self->begin(9600);
}

bool ESP::connected()
{
    return execute_at("AT");  // test ESP response
}

bool ESP::restart()
{
    bool rst = execute_at("AT+RST");  // restart ESP  
    delay(2000);  // wait for reastart
    clear_buffer(_self);  // clean leftovers

    (void) execute_at("ATE0");  // turn the echo off
    bool mode = execute_at("AT+CIPMODE=0");  // set ESP in normal transmission mode

    return (rst && mode);
}

bool ESP::execute_at(const char *cmd)
{
    return execute_at(cmd, "OK");
}

bool ESP::connect_wifi(const char *ssid, const char *pwd)
{
    sprintf(_command, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);  // generate AT command
    print_serial(_command, _self);  // send to the buffer
    
    int i = 0;
    // _is_connected = false;  // set ESP as not connected
    
    while (!_is_connected)
    {
        // check if new data is available
        if(_self->available() > 0)
        {              
            read_serial(_last_status, 256, _self);

            // check if response is an error
            if(strstr(_last_status, "FAIL")) 
            {
                Logger::log(_last_status);  // show the error message
               break;
            }
            else if (strstr(_last_status, "OK"))
            {
                char msg[100];
                sprintf(msg, "Connected to: %s", ssid);
                Logger::log(msg, LogLevel::VERBOSE);

                clear_buffer(_self);
                clear(msg);
                _is_connected = true;
            }
        }
        else    
            delay(500);  // wait for new data

        if (i == 50)    
            Logger::log(F("Connection takes longer then expected..."), LogLevel::WARNING);
        if (i >= 100)
        {
            Logger::log(F("Connection timeout"), LogLevel::ERROR);
            break;
        }
        i++;
    }

    clear_buffer(_self);
    _connection_status = _is_connected ? WIFI_CONNECTED : DISCONECTED;
    return _is_connected;
}

bool ESP::connect_host(const char *host)
{
    (void) check_connection_status();  // get connection status

    // check if connection is still alive
    if (_connection_status == HOST_CONNECTED)
        return true;  // end if it is
    else if (_connection_status == DISCONECTED)
        return false;  // do not try co connect if not connected with AP    

    // otherwise open new connection
    clear(_command);  // clear previous command
    sprintf(_command, "AT+CIPSTART=\"TCP\",\"%s\",80", host);

    return execute_at(_command); 
}

ConnectionStatus ESP::check_connection_status()
{ 
    if(!execute_at("AT+CIPSTATUS")) // get current connection status   
        return UNKNOWN;
    
    char status = _last_status[7];  // it is important to have ESP echo turned off
    switch (status)
    {
    case '2':
        _connection_status = WIFI_CONNECTED;
        break;
    case '3':
        _connection_status = HOST_CONNECTED;
        break;
    case '4':
        _connection_status = HOST_DISCONNECTED;
        break;
    case '5':
        _connection_status = DISCONECTED;
        break;
    default:
        _connection_status = UNKNOWN;
        break;
    }

    return _connection_status;
}

ConnectionStatus ESP::get_connection_status()
{
    return _connection_status;
}

bool ESP::send_post_request(const char *host, const ReadingsQueue &queue)
{
    Logger::log(F("Connecting to the host..."), LogLevel::VERBOSE);

    // Check if connection is still alive...
    if (!connect_host(host)) 
    {
        Logger::log(F("Can not connect to host"), LogLevel::ERROR);
        return false;
    } 

    Logger::log(F("Creating post request string..."), LogLevel::VERBOSE);
    queue.generate_post_request(_post_data, ";");  // generate post data string
    strcat (_post_data, "&user=aquarium_arduino&pwd=test");  // add user and pwd 

    clear (_command);
    sprintf(_command, 
            "POST /aquarium_post.php HTTP/1.1\r\n" \
            "Host: 198.160.0.179\r\n" \
            "Content-length: %d\r\n" \
            "Connection: Keep-Alive\r\n" \
            "User-Agent: Aquarium_Arduino\r\n" \
            "Content-type: application/x-www-form-urlencoded\r\n\r\n" \
            "%s", strlen(_post_data), _post_data);
    
    // Logger::log(_command);
    Logger::log(F("Openning connection..."), LogLevel::VERBOSE);

    // I want to have post request data ready to send
    // thus i am sending AT command manually
    _self->print("AT+CIPSEND="); 
    _self->println(strlen(_command));
    _self->flush();  // wait for data transfer
    delay(500);  // wait for the response

    Logger::log(F("Reading response from the ESP..."), LogLevel::VERBOSE);
    read_serial(_last_status, 256, _self);  // get ESP response

    if (strstr(_last_status, ">")) // response as expected
        Logger::log(F("Sending data..."), LogLevel::VERBOSE); 
    else // ups...
    {
        Logger::log(F("Sending error, ESP response below:"), LogLevel::ERROR);
        Logger::log(_last_status, LogLevel::ERROR);

        // clear the data
        clear(_post_data);
        clear(_command);
        clear_buffer(_self);
        delay(500);
       (void) execute_at("AT+CIPCLOSE");  // disconnect from the server
        return false;
    }

    _self->print(_command);  // sending post request
    _self->flush();  // wait for transfer

    Logger::log(F("Post request data sent..."), LogLevel::VERBOSE);
    Logger::log(F("Waiting for feedback."), LogLevel::VERBOSE);
    delay(1000);  // wait to read full response

    // Logger::log(F("ESP response:"));
    read_serial(_last_status, 256, _self);
    // Logger::log(_last_status);

    // clear the data
    clear(_post_data);
    clear(_command);
    clear_buffer(_self);

    return true;
}

// void ESP::_handle_err(const String *msg)
// {
//     Serial.println("AP connection error:");
//     char errcode = msg->charAt(msg->indexOf("CWJAP:")+6);

//     switch (errcode)
//     {
//     case '1':
//         Serial.println("Timeout");
//         break;
//     case '2':
//         Serial.println("Wrong password");
//         break;
//     case '3':
//         Serial.println("SSID not found");
//         break;
//     case '4':
//         Serial.println("Connection failed");
//         break;
//     default:
//         Serial.println("Unknow error...");
//         break;
//     }

//     return;
// }

bool ESP::execute_at(const char *cmd, const char *escape)
{
    int i = 0;
    bool status = false;

    while (!status & i < TIMEOUT)
    {
        i++;
        status = _execute_at(cmd, escape);
        delay(500);
    }
    
    if (i==10)
    {
        Logger::log(F("Connection timeout. ESP response:"), LogLevel::ERROR);
        Logger::log(_last_status, LogLevel::ERROR);
    }

    clear_buffer(_self);  // remove leftovers
    return status;
}

bool ESP::_execute_at(const char *cmd, const char *escape)
{
    // send command and wait for response
    print_serial(cmd, _self); 
    
    // wait for response
    while (_self->available() == 0) 
        delay(100); 
    
    // get response
    read_serial(_last_status, 256, _self); 

    // check if ESP response is OK
    if (strstr(_last_status, escape)) 
        return true;
    
    // check if serial is busy if yes, wait
    while (strstr(_last_status, "busy")) 
    {
        print_serial("AT", _self);
        read_serial(_last_status, 256, _self);
        
        if (strstr(_last_status, "OK"))
            break; // break if serial is ready

        Logger::log(F("Waiting for serial to free..."), LogLevel::WARNING);
        delay(500);
    }
    return false; // force next command execution
}
