#include "Lighting.h"

uint8_t Lighting::programs_amount = 0;
Lighting::Cover *Lighting::covers[LIGHTING_COVERS_AMOUNT];       // that array has to be ordered properly
Lighting::Program *Lighting::programs[LIGHTING_PROGRAMS_AMOUNT]; // that not necessarily

void Lighting::loop()
{
    uint32_t now = RTC::now(); // get current timestamp
    uint8_t pixels = 0;

    // loop through all covers with pixels
    for (uint8_t i = 0; i < LIGHTING_COVERS_AMOUNT; i++)
    {
        Cover *cover = covers[i];
        if (!cover)
        {
            return;  // just in case when null ptr is found
        }
        
        // loop through cover pixels
        for (uint8_t pixel = 0; pixel < cover->numPixels(); pixel++)
        {
            cover->setPixelProgram(now, pixel, pixels);  
            pixels++;
        }
        cover->show();
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
        uint32_t p_start = p->getStart() + LIGHTING_PROGRAM_OFFSET * pixel;
        uint32_t p_end = p->getEnd() + LIGHTING_PROGRAM_OFFSET * pixel;

        // if 'now' is within program duration start start the program
        if ((now >= p_start) & (now <= p_end))
        {
            program = p;
        }
    }
    return program;
}

Lighting::Cover::Cover(uint8_t order, uint16_t pin, uint16_t pixels_amount) : Adafruit_NeoPixel(pixels_amount, pin, NEO_GRB + NEO_KHZ800)
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
    uint32_t pixel_color = 0; 

    if (program)  // no program to run
    {
        pixel_color = program->getPixelColor(now, pixel_number);
    }
    setPixelColor(pixel_cover, pixel_color);    
}

void Lighting::Cover::start()
{
    begin();
    clear();
}

Lighting::Program::Program()
{
    if (programs_amount == LIGHTING_PROGRAMS_AMOUNT)
    {
        return;
    }

    programs[programs_amount] = this;
    programs_amount++;
}

void Lighting::Program::setup(uint32_t start, uint32_t end, uint8_t *pixel_start_cond, uint8_t *pixel_end_cond)
{
    // translate timestamp into HHMM
    _start = start * 100; // to be able to add seconds later on
    _end = end * 100;

    memcpy(_pixel_start_cond, pixel_start_cond, 3);
    memcpy(_pixel_end_cond, pixel_end_cond, 3);

    _pixel_diff[0] = pixel_end_cond[0] - pixel_start_cond[0];
    _pixel_diff[1] = pixel_end_cond[1] - pixel_start_cond[1];
    _pixel_diff[2] = pixel_end_cond[2] - pixel_start_cond[2];

}

uint32_t Lighting::Program::getPixelColor(uint32_t timestamp, uint8_t pixel_number)
{
    uint32_t ts = Timestamp::extract(DatePart::HHMMSS, timestamp);
    float progress = getProgress(ts, pixel_number);
    
    uint32_t r = (uint8_t)(_pixel_start_cond[0] + _pixel_diff[0] * progress);
    uint32_t b = (uint8_t)(_pixel_start_cond[1] + _pixel_diff[1] * progress);
    uint32_t w = (uint8_t)(_pixel_start_cond[2] + _pixel_diff[2] * progress);

    // char msg[100];
    // Serial.println(w);
    // sprintf(msg, "Pixel %d r %d w %d b %d", pixel_number, r, w, b);
    
    // Serial.println('-------------');
    // Serial.println(msg);
    // Serial.println(progress);
    // Serial.println('-------------');

    uint32_t color = (b << 16) | (r << 8) | w;
    return color;
}

float Lighting::Program::getProgress(uint32_t timestamp, uint8_t pixel_number)
{
    uint32_t offset = LIGHTING_PROGRAM_OFFSET * pixel_number;
    uint32_t start = _start + offset;
    uint32_t end = _end + offset;

    return ((timestamp - start) * 1.0) / (end - start);
}