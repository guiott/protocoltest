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