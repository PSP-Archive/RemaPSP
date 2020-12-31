RemaPSP is released under GPL. (other than blit.c which is under the BSD license)
The code is well documented.

RemaPSP is a good example of function hooking, I'm not sure if this works much different in 3.03OE but I suspect it doesn't.

Check main.h there are lots of things that can be configured in there, including the wifi play.

What is wifi play you ask? Well, when I made RemaPSP I added some functionality to control your psp over the wifi, however it would only work through the 1.5 BootstrapLoader and I couldn't be bothered writing a good windows client for it so I never released it.
The protocol is really simple, send a 4 byte udp packet to port 14567 on the PSP and it will be or'd into the pressed buttons value.
There is an example client in the client dir, it is incredibly simple however (only sends one button press)

Enjoy.
danzel - danzelatlocalhostdotgeekdotnz