/*-------------------------------------------------------------------------------
Copyright 2013 Guido Ottaviani
guido@guiott.com
acts as a tester with a simple Arduino compatible board exchanging data with the protocol:
http://www.guiott.com/Rino/CommandDescr/Protocol.htm
containing all the basic functionality of this kind of protocol.
It can also be used as a template to port the protocol on other languages.
The program is structured as a three layer system.
The first layer uses an aynchronous task (event, ISR or whatever) to get bytes from 
the serial port. These bytes are queued in a circular buffer and the serial task takes 
care of updating write pointer to the next free room.
When there are new bytes in buffer the second layer task, scheduled in the main loop, 
gets them and starts the analisys in a Finite State Machine checking the correctenss 
of the message packet.
When the packet is over and verified it passes to the third layer parser that performs
the scheduled actions according to the commnad code.
*/

#include <Time.h>
#include <Bounce.h>

//#define DEBUG_MODE // If defined outputs some data on serial for debug

long Bps = 57600;           // serial port speed
const int MAX_BUFF = 256;   // buffer size
int RxPtrIn = 0;            // circular queue pointer read
int RxPtrOut = 0;           // outgoing bytes read by Rx function
int RxStatus = 0;           // index for command decoding FSM status
char RxBuff[MAX_BUFF];      // circular buffer
char TxBuff[MAX_BUFF];

int RX_HEADER_LEN = 3;	    // command string header length (byte)

unsigned long Timeout=50;   // timeout in ms
long StartTime;             // the moment the packet starts receiving

int Joy[4];
int JoyIndx=0;
int Dummy; // Dummy buffer for analog reading
int led = 13;
int LedR= 10;
int LedG= 9;
int LedY= 11;
int Switch=12;
int Push=8;
boolean SwitchOk=0;
boolean PushOk=0;
Bounce SwitchBounce = Bounce (Switch, 25);
Bounce PushBounce = Bounce (Push, 25);

int RxPtrStart = 0;         // message packet starting pointer in queue
int RxPtrEnd = 0;           // message packet ending pointer in queue
int RxPtrData = 0;          // pointer to first data in queue
int RxCmdLen = 0;
/*==============================================================================*/
void setup() 
{
  pinMode(LedR, OUTPUT);  // LED red
  pinMode(LedG, OUTPUT);  // LED green
  pinMode(LedY, OUTPUT);  // LED yellow
  pinMode(Switch, INPUT);// bistable switch
  pinMode(Push, INPUT);  // momentary push button 

  // initialize serial:
  Serial.begin(Bps);
  // timeout in ms as a function of comm speed and buffer size
  Timeout = (int)((MAX_BUFF * 10000.0) / Bps); 
  
  setTime(11,26,0,10,8,2013); // just for test debug
  
#ifdef DEBUG_MODE
  Timeout=2000; // need to increase timeout because output requires more time
#endif
}

/*==============================================================================*/
void loop() 
{
  if (RxPtrIn != RxPtrOut) RxData(); // at least one character in circular queue
  if (RxStatus == 99)                
  {// the message packet is complete and verified
  	 Parser();      // decode ready message packet 
  }
  else if (RxStatus > 0) 
  {// a new message packet is coming
  	if((millis()-StartTime) > Timeout)
        {// if the packet is not complete within Timeout ms -> error
          RxError(1);
        }
  }
  
  SwitchBounce.update();
  PushBounce.update();

  SwitchOk = SwitchBounce.read();
  PushOk = PushBounce.read();

  digitalWrite(LedR, SwitchOk); 

  Joystick();
}

/*==============================================================================*/

void serialEvent() 
{/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = Serial.read(); 
    RxBuff[RxPtrIn] = inChar;
    RxPtrIn ++;	                      // fill-up the circular queue
    if (RxPtrIn>=MAX_BUFF) RxPtrIn=0; // reset circolar queue
  }
}

