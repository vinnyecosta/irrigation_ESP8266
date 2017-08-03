/*
* Search for a entry on the eeprom memory
* Input   : None
* Output  : u8 has_entry == i | Does not have an entry == 0
* Desc    : It searches if the entry is already stored into memory
*/
u16 search_entryeeprom(void)
{

  u16 reg_qty = 0;
  u16 i;
  reg_qty = (EEPROM.read(0));
  reg_qty = (reg_qty<<8) + EEPROM.read(1);
  Serial.printf("R : %d\n", reg_qty);
  for (i = 10; i <= (reg_qty*10); i = i+10)
  {
    Serial.printf("R%d -> %d\n", i, EEPROM.read(i));
    Serial.printf("R%d -> %d\n", i, EEPROM.read(i+1));
    Serial.printf("R%d -> %d\n", i, EEPROM.read(i+2));
    Serial.printf("R%d -> %d\n", i, EEPROM.read(i+3));
    Serial.printf("R%d -> %d\n", i, EEPROM.read(i+4));
    if ((EEPROM.read(i)==dt_p->weekday)&&(EEPROM.read(i+1)==dt_p->sethour)&&(EEPROM.read(i+2)==dt_p->setminute))
    {// if entry already in memory
      return 0xFFFF;
    }
    else if((EEPROM.read(i)==dt_p->weekday)==0)
    {// if entry was deleted and there is a empty space
      return i;
    }
  }// if the list is full without any empty space
  return 0;
}

/*
* Save data into eeprom
* Input   : None
* Output  : None
* Desc    : It saves the data structure into eeprom and increment the total
*           register stored into memory
*/
void save_data2eeprom(u16 entry_found)
{
  u16 reg_qty = 0;
  u16 i;
  reg_qty = ((EEPROM.read(0)<<8) | EEPROM.read(1));
  if ((u8)reg_qty == 0xFF)
  {
    EEPROM.write(0, reg_qty>>8 + 0x01);
    EEPROM.write(1,  0x00);
  }
  else
  {
    EEPROM.write(1, (u8)(reg_qty) + 0x01);
  }
  if (entry_found == 0x0000)
  {
    reg_qty = reg_qty+10;
  }
  else
  {
    reg_qty = entry_found;
  }
  EEPROM.write(reg_qty++,(u8)(dt_p->weekday));
  EEPROM.write(reg_qty++,(u8)(dt_p->sethour));
  EEPROM.write(reg_qty++,(u8)(dt_p->setminute));
  EEPROM.write(reg_qty++,(u8)(dt_p->sethouron));
  EEPROM.write(reg_qty++,(u8)(dt_p->setminuteon));
  EEPROM.commit();
}
/*
* Remove data from eeprom
* Input   : None
* Output  : None
* Desc    : It removes the data structure from eeprom and increment the total
*           register stored into memory
*/
void edit_data2eeprom(void)
{
  u16 reg_qty = 0;
  u16 i;
  reg_qty = ((EEPROM.read(0)<<8) | EEPROM.read(1));
  for (i = 10; i <= (reg_qty*10); i = i+10)
  {
    if ((EEPROM.read(i)==dt_p->weekday)&&(EEPROM.read(i+1)==dt_p->sethour)&&(EEPROM.read(i+2)==dt_p->setminute))
    {// when entry has been found
      EEPROM.write(reg_qty++,(u8)(dt_p->weekday));
      EEPROM.write(reg_qty++,(u8)(dt_p->sethour));
      EEPROM.write(reg_qty++,(u8)(dt_p->setminute));
      EEPROM.write(reg_qty++,(u8)(dt_p->sethouron));
      EEPROM.write(reg_qty++,(u8)(dt_p->setminuteon));
      break;
    }
  }
  EEPROM.commit();
}
/*
* Remove data from eeprom
* Input   : None
* Output  : None
* Desc    : It removes the data structure from eeprom and increment the total
*           register stored into memory
*/
void remove_data2eeprom(void)
{
  u16 reg_qty = 0;
  u16 i;
  reg_qty = ((EEPROM.read(0)<<8) | EEPROM.read(1));
  if ((u8)reg_qty == 0x00)
  {
    EEPROM.write(0, reg_qty>>8 - 0x01);
    EEPROM.write(1,  0xFF);
  }
  else
  {
    EEPROM.write(1, (u8)reg_qty - 0x01);
  }
  for (i = 10; i <= (reg_qty*10); i = i+10)
  {
    if ((EEPROM.read(i)==dt_p->weekday)&&(EEPROM.read(i+1)==dt_p->sethour)&&(EEPROM.read(i+2)==dt_p->setminute))
    {// when entry has been found
      EEPROM.write(i,0);
      break;
    }
  }
  EEPROM.commit();
}

void removeall_data2eeprom(void)
{
  EEPROM.write(0, 0x00);
  EEPROM.write(1, 0x00);
  EEPROM.commit();
}

/*
* Search for a entry on the eeprom memory
* Input   : None
* Output  : u8 has_entry == i | Does not have an entry == 0
* Desc    : It searches if the entry is already stored into memory
*/
u16 search_timeeeprom(u16 hm)
{

  u16 reg_qty = 0;
  u16 i;
  reg_qty = (EEPROM.read(0));
  reg_qty = (reg_qty<<8) + EEPROM.read(1);
  for (i = 10; i <= (reg_qty*10); i = i+10)
  {
    // Serial.printf("R%d -> %d\n", i, EEPROM.read(i));
    // Serial.printf("R%d -> %d\n", i, EEPROM.read(i+1));
    // Serial.printf("R%d -> %d\n", i, EEPROM.read(i+2));
    // Serial.printf("R%d -> %d\n", i, EEPROM.read(i+3));
    // Serial.printf("R%d -> %d\n", i, EEPROM.read(i+4));
    if ((EEPROM.read(i+1)==(u8)(hm>>8))&&(EEPROM.read(i+2)==(u8)(hm)))
    {
      reg_timeoff = calc_rtimer(EEPROM.read(i+1)+EEPROM.read(i+3), EEPROM.read(i+2)+EEPROM.read(i+4));
      // Serial.printf("Reg_timeroff -> %d\n", reg_timeoff);
      en_timeon = 1;
      return 1;
    }
    else
      return 0;
  }
}
