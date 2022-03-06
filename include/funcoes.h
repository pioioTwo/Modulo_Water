

// generate unique name from MAC addr
String macToStr(const uint8_t *mac)
{
  String result;
  for (int i = 0; i < 6; ++i)
  {
    result += String(mac[i], 16);
    if (i < 5)
    {
      result += ':';
    }
  }
  return result;
}

/* Retorna o nivel de sinal em % */
int getRSSIasQuality(int RSSI)
{
  int quality = 0;
  if (RSSI <= -100)
  {
    quality = 0;
  }
  else if (RSSI >= -50)
  {
    quality = 100;
  }
  else
  {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}

/* Recebe um array de char e retorna o valor em inteiro */
int getIntValue(char *_dados)
{
  int value = String(_dados).toInt();
  return value;
}



