#include "Lighting.h"

uint8_t Lighting::programs_amount = 0;
Lighting::Cover *Lighting::covers[LIGHTING_COVERS_AMOUNT];       // that array has to be ordered properly
Lighting::Program *Lighting::programs[LIGHTING_PROGRAMS_AMOUNT]; // that not necessarily

void Lighting::loop()
{
    uint32_t now = RTC::now(); // get current timestamp
    uint8_t pixels = 0;        // this is for shifting program start / end values

    // loop through all covers with pixels
    for (uint8_t i = 0; i < LIGHTING_COVERS_AMOUNT; i++)
    {
        Cover *cover = covers[i];
        if (!cover)
        {
            return;  // just in case when null ptr is found
        }
        
        // loop through cover pixels
        for (uint16_t pixel = 0; pixel < cover->numPixels(); pixel++)
        {
            uint32_t pixel_color = cover->getPixelColor(pixel);
            cover->setPixelProgram(now, pixel, pixels);
            pixels++;
        }
    }
}

Lighting::Program *Lighting::Cover::getPixelProgram(uint32_t now, uint8_t pixel)
{
    // first convert current timestamp to HHMMSS
    Timestamp ts_now = Timestamp(now);
    now = ts_now.extract(DatePart::HHMMSS);

    Program *program; // program to be applied to the pixel

    for (uint8_t i = 0; i < programs_amount; i++)
    {
        Program *p = programs[i]; // take the program

        // check when the program for the pixel should start
        uint32_t p_start = p->start() + LIGHTING_PROGRAM_OFFSET * pixel;
        uint32_t p_end = p->end() + LIGHTING_PROGRAM_OFFSET * pixel;

        // if 'now' is within program duration start start the program
        if ((now >= p_start) & (now <= p_end))
        {
            program = p;
        }
    }
    return program;
}

Lighting::Cover::Cover(uint8_t order, uint16_t pin, uint16_t pixels_amount) : Adafruit_NeoPixel(pixels_amount, pin)
{
    if (order >= LIGHTING_COVERS_AMOUNT)
    {
        return;
    }

    covers[order] = this;
}

void Lighting::Cover::setPixelProgram(uint32_t now, uint16_t pixel_cover, uint16_t pixel_number)
{
    Program *program = getPixelProgram(now, pixel_number);

    if (!program)  // no program to run
    {
        return;
    }    

    uint32_t color = program->getPixelColor(now, pixel_number);

    setPixelColor(pixel_cover, color);
}

Lighting::Program::Program(uint32_t start, uint32_t end, uint8_t *pixel_start_cond, uint8_t *pixel_end_cond)
{
    if (programs_amount == LIGHTING_PROGRAMS_AMOUNT)
    {
        return;
    }

    // translate timestamp into HHMM
    _start = start * 100; // to be able to add seconds later on
    _end = end * 100;

    memcpy(_pixel_start_cond, pixel_start_cond, 3);
    memcpy(_pixel_end_cond, pixel_end_cond, 3);

    _pixel_diff[0] = pixel_end_cond[0] - pixel_start_cond[0];
    _pixel_diff[1] = pixel_end_cond[1] - pixel_start_cond[1];
    _pixel_diff[2] = pixel_end_cond[2] - pixel_start_cond[2];

    programs[programs_amount] = this;
    programs_amount++;
}

uint32_t Lighting::Program::getPixelColor(uint32_t timestamp, uint8_t pixel_number)
{
    uint32_t ts = Timestamp::extract(DatePart::HHMMSS, timestamp);
    float progress = getProgress(ts, pixel_number);
    
    uint8_t r = (uint8_t)(_pixel_start_cond[0] + _pixel_diff[0] * progress);
    uint8_t w = (uint8_t)(_pixel_start_cond[1] + _pixel_diff[1] * progress);
    uint8_t b = (uint8_t)(_pixel_start_cond[2] + _pixel_diff[2] * progress);

    uint32_t color = ((uint32_t)(r << 16)) |
                     ((uint32_t)(w << 8)) |
                     ((uint32_t)b);
    return color;
}

float Lighting::Program::getProgress(uint32_t timestamp, uint8_t pixel_number)
{
    uint32_t offset = LIGHTING_PROGRAM_OFFSET * pixel_number;
    uint32_t start = _start + offset;
    uint32_t end = _end + offset;

    return ((timestamp - start) * 1.0) / (end - start);
}
