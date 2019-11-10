#ifndef ESP_8266_h
#define ES_8266P_h

#define TIMEOUT 10  // attempts to execute AT command
#define SENDING_INTERVAL 30000  // how often new data will be sent to the server

#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ReadingsQueue.h>
#include <Utils.h>
#include <Log.h>

enum ConnectionStatus {DISCONECTED=-1, WIFI_CONNECTED, HOST_CONNECTED, HOST_DISCONNECTED, UNKNOWN};

class ESP
{
    public:
        ESP(int rx, int tx);
        
        bool restart();
        bool connect_wifi(const char *ssid, const char *pwd);
        bool connect_host(const char *address);

        bool execute_at(const char *cmd);
        bool execute_at(const char *cmd, const char *escape);

        bool send_post_request(const char *host, const ReadingsQueue &queue);
        bool connected();

        ConnectionStatus check_connection_status();
        ConnectionStatus get_connection_status();        

    private:
        SoftwareSerial *_self;
        ConnectionStatus _connection_status = DISCONECTED;

        char _command[512];
        char _post_data[256];
        char _last_status[256];
        
        bool _is_connected = false; 
        bool _execute_at(const char *cmd, const char *escape);

        void _handle_err(const String *msg);

};

#endif