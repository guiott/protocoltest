/*-------------------------------------------------------------------------------
Copyright 2013 Guido Ottaviani
guido@guiott.com
part of "ProtocolTester.ino"
acts as a tester exchanging data with the protocol:
http://www.guiott.com/Rino/CommandDescr/Protocol.htm
*/

byte linBuff[MAX_BUFF]; // Linear buffer 

/*-----------------------------------------------------------------------------*/
// Buffer b_dsNavParam
struct _Buff_b
{
    float PosXmes;  // current X position coordinate
    float PosYmes;  // current Y position coordinate
    int VelInt[4];  // speed in mm/s as an integer for all the wheels
    int ADCValue[4];// 64 sample average ADC also for slave
    byte stasis_err;   // number of times imu and wheels very different
    byte stasis_alarm; // signal too many stasis errors
};

union __Buff_b
{
    struct _Buff_b I;// to use as integers or chars, little endian LSB first
    byte C[26];  // to use as bytes to send on char buffer
}Buff_b;

void b_dsNavParam(void)
{/* details on speed, current and position dsNav -> IMU -> HLS
     0      Header	 @
     1      Id		 0	ASCII	(not used here, just for compatibility)
     2      Cmd		 b 	ASCII
     3      CmdLen	 Num of bytes (bin) following (checksum included)
     4-7    PosXmes current X position coordinate (MSB first)
     8-11   PosYmes current Y position coordinate (MSB first)
     12-19  VelInt[4] speed in mm/s as an integer for all the wheels (4 ints)
     20-27  ADCValue[4] measured current for each motor (4 ints)
     28     stasis_err number of times imu and wheels very different
     29     stasis_alarm stasis_alarm
    */
    int Indx = RX_HEADER_LEN;  // Head length, number of characters in buffer before valid data
    TxBuff[++Indx]=Buff_b.C[3]; // PosXmes MSB
    TxBuff[++Indx]=Buff_b.C[2];
    TxBuff[++Indx]=Buff_b.C[1];
    TxBuff[++Indx]=Buff_b.C[0];
    TxBuff[++Indx]=Buff_b.C[7]; // PosYmes MSB
    TxBuff[++Indx]=Buff_b.C[6];
    TxBuff[++Indx]=Buff_b.C[5];
    TxBuff[++Indx]=Buff_b.C[4];
    TxBuff[++Indx]=Buff_b.C[9]; // VelInt[0]   MSB
    TxBuff[++Indx]=Buff_b.C[8];
    TxBuff[++Indx]=Buff_b.C[11];// VelInt[1]   MSB
    TxBuff[++Indx]=Buff_b.C[10];
    TxBuff[++Indx]=Buff_b.C[13];// VelInt[2]   MSB
    TxBuff[++Indx]=Buff_b.C[12];
    TxBuff[++Indx]=Buff_b.C[15];// VelInt[3]   MSB
    TxBuff[++Indx]=Buff_b.C[14];
    TxBuff[++Indx]=Buff_b.C[17];// ADCValue[0] MSB
    TxBuff[++Indx]=Buff_b.C[16];
    TxBuff[++Indx]=Buff_b.C[19];// ADCValue[1] MSB
    TxBuff[++Indx]=Buff_b.C[18];
    TxBuff[++Indx]=Buff_b.C[21];// ADCValue[2] MSB
    TxBuff[++Indx]=Buff_b.C[20];
    TxBuff[++Indx]=Buff_b.C[23];// ADCValue[3] MSB
    TxBuff[++Indx]=Buff_b.C[22];
    TxBuff[++Indx]=Buff_b.C[24];// stasis_err number
    TxBuff[++Indx]=Buff_b.C[25];// stasis_alarm
    
    TxData('b', Indx+1);
}

/*-----------------------------------------------------------------------------*/
// Buffer T_GpsTime
struct _Buff_T
{
    int UtcYear;  
    byte UtcMonth;  
    byte UtcDay;
    byte UtcHour;
    byte UtcMinute;
    byte UtcSecond;
};

union __Buff_T
{
    struct _Buff_T I;// to use as integers or chars, little endian LSB first
    byte C[8];  // to use as bytes to send on char buffer
}Buff_T;

void T_GpsTime(void)
{/* GPS time parameters GPS -> IMU -> HLS
     0      Header	 @
     1      Id		   0	ASCII	(not used here, just for compatibility)
     2      Cmd		   K 	ASCII
     3      CmdLen	 Num of bytes (bin) following (checksum included)
     4-5    utcYear (unsigned short int)
     6      utcMonth (unsigned char)
     7      utcDay (unsigned char)
     8      utcHour (unsigned char)
     9      utcMinute (unsigned char utcMinute)
     10-11  utcSeconds   (int)
     */
    int Indx = RX_HEADER_LEN;  // Head length, number of characters in buffer before valid data
 
    // simulation. The requested value is looped back as a test
    Buff_T.I.UtcYear = year();  
    Buff_T.I.UtcMonth = month();  
    Buff_T.I.UtcDay = day();
    Buff_T.I.UtcHour = hour();
    Buff_T.I.UtcMinute = minute();
    Buff_T.I.UtcSecond = second();
    //simulation
    
    TxBuff[++Indx]= Buff_T.C[1]; // Year MSB
    TxBuff[++Indx]= Buff_T.C[0]; // Year LSB
    TxBuff[++Indx]= Buff_T.C[2]; // Month
    TxBuff[++Indx]= Buff_T.C[3]; // Day
    TxBuff[++Indx]= Buff_T.C[4]; // Hours
    TxBuff[++Indx]= Buff_T.C[5]; // Minutes
    TxBuff[++Indx]= Buff_T.C[7]; // Seconds MSB
    TxBuff[++Indx]= Buff_T.C[6]; // Seconds
    
    TxData('T', Indx+1);
}

/*-----------------------------------------------------------------------------*/
// Buffer G_GpsService
struct _Buff_G
{
     int week_no;     // Week Number (GPS)
     long tow;        // Time in micro seconds since Saturday midnight GMT (GPS)
     byte hdop;       // Horizontal Diluition Of Precision
     byte svs;        // Number of visible satellites
     long SatIdList;  // Satellite ID list
     long HEPE;       // Horizontal Estimated Position Error
};

union __Buff_G
{
    struct _Buff_G I;// to use as integers or chars, little endian LSB first
    byte C[16];  // to use as bytes to send on char buffer
}Buff_G;

void G_GpsService(void)
{/* GPS service parameters GPS -> IMU -> HLS
     0      Header	 @
     1      Id		 0	ASCII	(not used here, just for compatibility)
     2      Cmd		 G 	ASCII
     3      CmdLen	 Num of bytes (bin) following (checksum included)
     4-5    Week Number (GPS)
     6-9    Time in micro seconds since Saturday midnight GMT (GPS)
     10     hdop
     11     number of visible satellites
     12-15  Satellite ID list
     16-19  HEPE
     */
     
    int Indx = RX_HEADER_LEN;  // Head length, number of characters in buffer before valid data

    // simulation. The requested value is looped back as a test
     Buff_G.I.week_no=0X1234;
     Buff_G.I.tow=0X56789ABC;
     Buff_G.I.hdop=5;
     Buff_G.I.svs=9;
     Buff_G.I.SatIdList=61680;
     Buff_G.I.HEPE=7967;
    // simulation. 
      
    TxBuff[++Indx]=Buff_G.C[1];     // week number MSB first
    TxBuff[++Indx]=Buff_G.C[0];
    TxBuff[++Indx]=Buff_G.C[5];     // tow MSB first
    TxBuff[++Indx]=Buff_G.C[4];
    TxBuff[++Indx]=Buff_G.C[3];
    TxBuff[++Indx]=Buff_G.C[2];
    TxBuff[++Indx]=Buff_G.C[6];     // hdop
    TxBuff[++Indx]=Buff_G.C[7];     // svs
    TxBuff[++Indx]=Buff_G.C[11];    // SatIdList MSB first
    TxBuff[++Indx]=Buff_G.C[10];  
    TxBuff[++Indx]=Buff_G.C[9];
    TxBuff[++Indx]=Buff_G.C[8];
    TxBuff[++Indx]=Buff_G.C[15];    // HEPE MSB first
    TxBuff[++Indx]=Buff_G.C[14];
    TxBuff[++Indx]=Buff_G.C[13];
    TxBuff[++Indx]=Buff_G.C[12];
 
     TxData('G', Indx+1);
}

/*-----------------------------------------------------------------------------*/
// Buffer K_GpsPos
struct _Buff_K
{
     long lat;  // GPS latitude shown as the number of degrees times 10 to the power 7 North of the equator. (negative means south).
     long lon;  // GPS longitude shown as the number of degrees times 10 to the power 7 East of the Greenwich Meridian
     long alt;  // altitude in centimeters above mean sea level
     int sog;   // Speed over the ground from the GPS in meters / second times 100
     int cog;   // GPS Course over the Ground (2D) of the plane in degrees times 100
     /*
     DCM XpXe - rmat[0]
     DCM XpYe - rmat[1]
     DCM XpZe - rmat[2]
     DCM YpXe - rmat[3]
     DCM YpYe - rmat[4]
     DCM YpZe - rmat[5]
     DCM ZpXe - rmat[6]
     DCM ZpYe - rmat[7]
     DCM ZpZe - rmat[8]
     */
     int rmat[9];    
     byte udb_cpu_load; // Percentage of available cpu power that has been used over the last one second.
     int YawOffset;     // initial position offset
};

union __Buff_K
{
    struct _Buff_K I;// to use as integers or chars, little endian LSB first
    byte C[37];      // to use as bytes to send on char buffer
}Buff_K;

void K_GpsPos(void)
{/* GPS & IMU (DCM) parameters GPS-IMU -> HLS
     0      Header	 @
     1      Id		 0	ASCII	(not used here, just for compatibility)
     2      Cmd		 K 	ASCII
     3      CmdLen	 Num of bytes (bin) following (checksum included)
     4-7    Plane gps latitude shown as the number of degrees times 10 to the power 7 North of the equator. (negative means south).
     8-11   Plane gps longitude shown as the number of degrees times 10 to the power 7 East of the Greenwich Meridian
     12-15  Plane gps altitude in centimeters above mean sea level
     16-17  Speed over the ground from the GPS in meters / second times 100
     18-19  GPS Course over the Ground (2D) of the plane in degrees times 100
     20-21  DCM XpXe - rmat[0]
     22-23  DCM XpYe - rmat[1]
     24-25  DCM XpZe - rmat[2]
     26-27  DCM YpXe - rmat[3]
     28-29  DCM YpYe - rmat[4]
     30-31  DCM YpZe - rmat[5]
     32-33  DCM ZpXe - rmat[6]
     34-35  DCM ZpYe - rmat[7]
     36-37  DCM ZpZe - rmat[8]
     38     Percentage of available cpu power that has been used over the last one second.
     39-40  Yaw Offset, initial position offset
     */
    int Indx = RX_HEADER_LEN;  // Head length, number of characters in buffer before valid data

    // simulation. The requested value is looped back as a test
    Buff_K.I.lat=418588920;
    Buff_K.I.lon=125690950;
    Buff_K.I.alt=7800;
    Buff_K.I.cog=4700;
    Buff_K.I.sog=12300;
    Buff_K.I.YawOffset=29;
    // simulation
   /* 
      UDB4.yawAbs = joy.RX * 1.8; actually is computed from rmat
      UDB4.yawRel =  joy.RX * 1.8 - 45;  computed adding YawOffset to yawAbs 
      UDB4.pitch = -joy.LY * 0.9; actually is computed from rmat
      UDB4.roll = -joy.LX * 0.9;  actually is computed from rmat
   */   
    // simulation.
     
    TxBuff[++Indx]=Buff_K.C[3];      // lat_gps MSB first
    TxBuff[++Indx]=Buff_K.C[2];
    TxBuff[++Indx]=Buff_K.C[1];
    TxBuff[++Indx]=Buff_K.C[0];
    TxBuff[++Indx]=Buff_K.C[7];      // lon_gps MSB first
    TxBuff[++Indx]=Buff_K.C[6];
    TxBuff[++Indx]=Buff_K.C[5];
    TxBuff[++Indx]=Buff_K.C[4];
    TxBuff[++Indx]=Buff_K.C[11];     // alt_sl_gps MSB first
    TxBuff[++Indx]=Buff_K.C[10];
    TxBuff[++Indx]=Buff_K.C[9];
    TxBuff[++Indx]=Buff_K.C[8];
    TxBuff[++Indx]=Buff_K.C[13];     // sog_gpss MSB first
    TxBuff[++Indx]=Buff_K.C[12];
    TxBuff[++Indx]=Buff_K.C[15];     // cog_gpss MSB first
    TxBuff[++Indx]=Buff_K.C[14];
    TxBuff[++Indx]=Buff_K.C[17];     // rmat[0] MSB first
    TxBuff[++Indx]=Buff_K.C[16];
    TxBuff[++Indx]=Buff_K.C[19];     // rmat[1] MSB first
    TxBuff[++Indx]=Buff_K.C[18];
    TxBuff[++Indx]=Buff_K.C[21];     // rmat[2] MSB first
    TxBuff[++Indx]=Buff_K.C[20];
    TxBuff[++Indx]=Buff_K.C[23];     // rmat[3] MSB first
    TxBuff[++Indx]=Buff_K.C[22];
    TxBuff[++Indx]=Buff_K.C[25];     // rmat[4] MSB first
    TxBuff[++Indx]=Buff_K.C[24];
    TxBuff[++Indx]=Buff_K.C[27];     // rmat[5] MSB first
    TxBuff[++Indx]=Buff_K.C[26];
    TxBuff[++Indx]=Buff_K.C[29];     // rmat[6] MSB first
    TxBuff[++Indx]=Buff_K.C[28];
    TxBuff[++Indx]=Buff_K.C[31];     // rmat[7] MSB first
    TxBuff[++Indx]=Buff_K.C[30];
    TxBuff[++Indx]=Buff_K.C[33];     // rmat[8] MSB first
    TxBuff[++Indx]=Buff_K.C[32];
    TxBuff[++Indx]=Buff_K.C[34];     // udb_cpu_load()
    TxBuff[++Indx]=Buff_K.C[36];     // YawOffset MSB first
    TxBuff[++Indx]=Buff_K.C[35];     
 
    TxData('K', Indx+1);
}


/*-----------------------------------------------------------------------------*/
void S_Set(void)
{/* set speed & direction HLS -> IMU -> dsNav*/
   int VelDes; // mean desired speed mm/s
   int YawDes; // desired orientation angle (set point)(Degx10 0-3599)
   int Light[2];// headlights intensity
   int hPwrOff; // Power Off command from HLS to LLS
   
   linearize();
   VelDes = (linBuff[0] << 8) + (linBuff[1]);
   YawDes = (linBuff[2] << 8) + (linBuff[3]);
   Light[0] = linBuff[4];
   Light[1] = linBuff[5];
   hPwrOff = linBuff[6];
   
   // simulation. The requested value is looped back as a test
   Buff_b.I.VelInt[0] = VelDes;
   Buff_b.I.VelInt[1] = VelDes;
   Buff_b.I.VelInt[2] = VelDes;
   Buff_b.I.VelInt[3] = VelDes;
   Buff_b.I.ADCValue[0] = VelDes * 10;
   Buff_b.I.ADCValue[1] = VelDes * 10;
   Buff_b.I.ADCValue[2] = VelDes * 10;
   Buff_b.I.ADCValue[3] = VelDes * 10;
   Buff_b.I.PosXmes = 123.456789;
   Buff_b.I.PosYmes = 987.654321;
   Buff_b.I.stasis_err = 8;
   Buff_b.I.stasis_alarm = 9;

   if (hPwrOff > 0)
   {
     digitalWrite(LedG, LOW); 
   }
   else
   {
     digitalWrite(LedG, HIGH); 
   }
   analogWrite(LedY, (Light[0]));

   // simulation.
}

/*-----------------------------------------------------------------------------*/
// Buffer L_LLS
struct _Buff_L
{
     byte BatV[2]; // Left and Right battery voltage level
     byte Temp[2]; // Left and Right skulls temperature
     byte Obst[7]; // Obstacles
     byte lPwrOff; // Power Off command from LLS to HLS
};

union __Buff_L
{
    struct _Buff_L I;// to use as integers or chars, little endian LSB first
    byte C[12];  // to use as bytes to send on char buffer
}Buff_L;

void L_LLS(void)
{/*  values coming from Low Level Supervisor LLS -> HLS
     right now it's simulated by the same serial port but actually it will come from the LLS serial
     0      Header	 @
     1      Id		 0	ASCII	(not used here, just for compatibility)
     2      Cmd		 G 	ASCII
     3      CmdLen	 Num of bytes (bin) following (checksum included)
     4      BatV[0]      Left and Right battery voltage level
     5      BatV[1]
     6      Temp[0]      Left and Right skulls temperature
     7      Temp[1]
                         obstacle distance @ x degrees 
                         (http://www.guiott.com/QuadSonar/HR/LinoSonar.png)
     8      Obst[0]      LL Lef Left -52°
     9      Obst[1]      LC Left Center -27°
     10     Obst[2]      CL Center Left -12°
     11     Obst[3]      CC Center Center 0°
     12     Obst[4]      CR Center Right 12°
     13     Obst[5]      RC Right Center 27°
     14     Obst[6]      RR Right Right 52°
     15     lPwrOff      Switch Off from LLS to HLS -> 0 = PowerOn, 1 = PowerOff
     */
     
    int Indx = RX_HEADER_LEN;  // Head length, number of characters in buffer before valid data

     // simulation. The requested value variation is simulated with joysticks as a test
     Buff_L.I.BatV[0] = Joy[0]/10.24;
     Buff_L.I.BatV[1] = Joy[1]/10.24;
     Buff_L.I.Temp[0] = Joy[2]/10.24;
     Buff_L.I.Temp[1] = Joy[3]/10.24;
     Buff_L.I.Obst[0] = 0;
     Buff_L.I.Obst[1] = 1;
     Buff_L.I.Obst[2] = 2;
     Buff_L.I.Obst[3] = 3;
     Buff_L.I.Obst[4] = 4;
     Buff_L.I.Obst[5] = 5;
     Buff_L.I.Obst[6] = 6;
     Buff_L.I.lPwrOff  = SwitchOk;
     // simulation
      
     TxBuff[++Indx]=Buff_L.C[0];     // Battery level
     TxBuff[++Indx]=Buff_L.C[1];
     TxBuff[++Indx]=Buff_L.C[2];     // Temperature
     TxBuff[++Indx]=Buff_L.C[3];
     TxBuff[++Indx]=Buff_L.C[4];     // Obstacles
     TxBuff[++Indx]=Buff_L.C[5];
     TxBuff[++Indx]=Buff_L.C[6];
     TxBuff[++Indx]=Buff_L.C[7];
     TxBuff[++Indx]=Buff_L.C[8];
     TxBuff[++Indx]=Buff_L.C[9];
     TxBuff[++Indx]=Buff_L.C[10];
     TxBuff[++Indx]=Buff_L.C[11];    // Power Off

     TxData('L', Indx+1);
}

/*-----------------------------------------------------------------------------*/
void linearize(void)
{// bring the circular queue payload data in a linear buffer for a simpler parsing
  int ptr = RxPtrData;
  for (int i=0; i<RxCmdLen-1; i++)
  {
    if (++ptr >= MAX_BUFF) ptr = 0;
    linBuff[i] = RxBuff[ptr];
  }
}

