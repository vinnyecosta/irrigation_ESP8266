//ROTINA PARA CONTROLAR A HORA POR NUMEROS INTEIROS
u16 calc_timer()
{
    u32 var = 0;
    u8 m, h;
    h = ttime/3600;
    var = h>=1 ? ttime - (h*3600) : ttime;
    m = var/60;
    return ((h<<8) | m);
    // Serial.println(h);
    // Serial.println(m);
    // s = var%60;
}
//ROTINA PARA CONVERTER HORA EM NUMERO
u32 calc_rtimer(u8 h, u8 m)
{
  u32 result;
  if (h==0)
    result =  m*60;
  else if (m==0)
    result =  h*60*60;
  else
    result =  h*m*60;
  return result;
}
