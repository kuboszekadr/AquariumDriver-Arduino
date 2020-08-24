#ifndef Lighting_h
#define Lighting_h

// that variable is relevant in that case because under the cover there are three
// lighting covers, unfortunatelly complicates the case a bit
#define LIGHTING_COVERS_AMOUNT 3
#define LIGHTING_PROGRAM_OFFSET 30L // how each pixel row is turned ON (in seconds!)

#define LIGHTING_PROGRAMS_AMOUNT 7
#define LIGHTING_CONFIG_SIZE 120

#include "Config.h"
#include "RTC.h"
#include "Timestamp.h"

#include <Adafruit_NeoPixel.h>

namespace Lighting
{
    const char lighting_config_path[] PROGMEM = "config/lighting/%s.txt";

    class Program : public Config
    {
    public:
        Program(const char *name);
        void setup(uint32_t start, uint32_t end, uint8_t *pixel_start_cond, uint8_t *pixel_end_cond);

        uint32_t getPixelColor(uint32_t timestamp, uint8_t pixel_number);

        uint32_t getStart() { return _start; };
        uint32_t getEnd() { return _end; };

        uint8_t *getStartCondition() { return _pixel_start_cond; };
        uint8_t *getEndCondition() { return _pixel_end_cond; };

        void loadConfig();
        void saveConfig();

    private:
        float getProgress(uint32_t timestamp, uint8_t pixel_number);
        const char _name[9];

        uint32_t _start; // when the program starts (HHMM)
        uint32_t _end;   // when the program ends (HHMM)

        uint8_t _pixel_start_cond[3] = {};
        uint8_t _pixel_end_cond[3] = {};
        int16_t _pixel_diff[3] = {};
    };

    class Cover : public Adafruit_NeoPixel
    {
    public:
        Cover(uint8_t order, uint16_t pin, uint16_t pixels_amount);
        void setPixelProgram(uint32_t now, uint16_t pixel_cover, uint16_t pixel_nuber);
        void start();

    private:
        Program *getPixelProgram(uint32_t now, uint8_t pixel);
    };

    extern Cover *covers[LIGHTING_COVERS_AMOUNT];       // that array has to be ordered properly
    extern Program *programs[LIGHTING_PROGRAMS_AMOUNT]; // that not necessarily

    extern uint8_t programs_amount;

    void loop();
    void loadConfig();
    void saveConfig();
    void start();
} // namespace Lighting

#endif