#include "Lighting.h"

uint8_t Lighting::programs_amount = 0;
Lighting::Cover *Lighting::covers[LIGHTING_COVERS_AMOUNT];       // that array has to be ordered properly
Lighting::Program *Lighting::programs[LIGHTING_PROGRAMS_AMOUNT] = {}; // that not necessarily

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
            return; // just in case when null ptr is found
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

    Program *program = nullptr; // program to be applied to the pixel

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
    uint32_t pixel_color = 0L;

    if (program) // no program to run
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

Lighting::Program::Program(const char *name)
{
    if (programs_amount == LIGHTING_PROGRAMS_AMOUNT)
    {
        return;
    }

    strcpy(_name, name);
    programs[programs_amount] = this;
    programs_amount++;
}

void Lighting::Program::setup(uint32_t start, uint32_t end, uint8_t *pixel_start_cond, uint8_t *pixel_end_cond)
{
    // translate timestamp into HHMM
    _start = start * 100L; // to be able to add seconds later on
    _end = end * 100L;
 
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

void Lighting::Program::loadConfig()
{
    char file_name[40] = {};
    sprintf_P(file_name, lighting_config_path, _name);

    StaticJsonDocument<LIGHTING_CONFIG_SIZE> doc;
    if(!loadFile(file_name, doc))
    {
        return;
    }

    int start = doc["s_h"];
    int end = doc["e_h"];

    uint8_t start_cond[3] = {
        doc["s_cond"][0],
        doc["s_cond"][1],
        doc["s_cond"][2]
    };

    uint8_t end_cond[3] = {
        doc["e_cond"][0],
        doc["e_cond"][1],
        doc["e_cond"][2]
    };

    setup(start, end, start_cond, end_cond);
}

void Lighting::Program::saveConfig()
{
    StaticJsonDocument<LIGHTING_CONFIG_SIZE> doc;

    doc["s_h"] = getStart() / 100L;
    doc["e_h"] = getEnd() / 100L;

    JsonArray s_cond = doc.createNestedArray("s_cond");
    s_cond.add(_pixel_start_cond[0]);
    s_cond.add(_pixel_start_cond[1]);
    s_cond.add(_pixel_start_cond[2]);

    JsonArray e_cond = doc.createNestedArray("e_cond");
    e_cond.add(_pixel_end_cond[0]);
    e_cond.add(_pixel_end_cond[1]);
    e_cond.add(_pixel_end_cond[2]);

    char file_name[40] = {};
    sprintf_P(file_name, lighting_config_path, _name);
    Serial.println(file_name);
    saveToFile(file_name, doc);
}

void Lighting::saveConfig()
{
    for (uint8_t i = 0; i < programs_amount; i++)
    {
        Program *program  = programs[i];
        program->saveConfig();
    }
}

void Lighting::loadConfig()
{
    char lighting_config_dir[] = "config/lighting";

    File dir = SD.open(lighting_config_dir);
    uint8_t i = 0;

    while (i < LIGHTING_PROGRAMS_AMOUNT)
    {
        File entry = dir.openNextFile();
        if(!entry)
        {
            break;
        }
        char file_name[15] = {};
        strcpy(file_name, entry.name());

        char program_name[9] = {};
        strncpy(program_name, file_name, strlen(file_name) - 4);

        Program *program = new Program(program_name);
        program->loadConfig();
        programs[i++] = program;

        entry.close();
    }
}

void Lighting::start()
{
     // loop through all covers with pixels
    for (uint8_t i = 0; i < LIGHTING_COVERS_AMOUNT; i++)
    {
        Cover *cover = covers[i];
        if (!cover)
        {
            return; // just in case when null ptr is found
        }
        cover->start();
    }   
}