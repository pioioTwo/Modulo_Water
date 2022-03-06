
#include <math.h>
#define PIN_SENSE 14 // Pino que habilita sensor de temperatura da agua quente

unsigned int Rs = 100000; // Valor da resistencia de queda de tensão do thermistor para vcc
double Vcc = 3.3;         // Valor ds tensão maxima da entrada analogica


void initWater()
{

}

/* Filtro da entrada Analogica */
int AnalogRead()
{
    digitalWrite(PIN_SENSE, 1); // habilita o pino para leitura do thermistor
    delay(50);
    int val = 0;
    for (int i = 0; i < 50; i++)
    {
        val += analogRead(A0);
        delay(1);
    }
    val = val / 50;
    digitalWrite(PIN_SENSE, 0); // desabilita o pino para leitura do thermistor
    return val;
}

/* Calcula a temperatura no trhermistor */
double Thermister()
{
    int val = AnalogRead();
    double V_NTC = (double)val / 1024;
    double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC);
    R_NTC = log(R_NTC);
    double Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * R_NTC * R_NTC)) * R_NTC);
    Temp = Temp - 273.15;
    return Temp;
}