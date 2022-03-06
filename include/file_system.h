

#ifdef LITTLE_FS
#include "LittleFS.h"


// Initialize LittleFS
void initLittleFS()
{
    if (!LittleFS.begin())
    {
#ifdef DEBUG_
        Serial.println("An error has occurred while mounting LittleFS");
#endif
    }
#ifdef DEBUG_
    Serial.println("LittleFS mounted successfully");
#endif
}
#endif