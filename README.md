# smart-hotel

 Project abstract:
Stage 1: (first time configuration stage)
The receptionist can insert unlimited rooms “huge number of rooms may require connecting external RAM”. The receptionist PC must send the number of each room via UART “char by char with no spaces” starting with the total number of rooms and attaching ‘R’ before each number. At the end of the stages send ‘D’.
Example: If you want 4 rooms with numbers (22,96,5,2020)  send “4R22R96R5R2020D”. 

Stage 2: (running time stage)
Each room has 3 statuses:  0  free.    1  occupied.     2  Room Cleaning
By default, all rooms are on status 0.
The receptionist can change the statues of any room anytime and can set the password of any room any time.
On this stage, the receptionist should enter a command. First send the room number.
If the receptionist wants to set a password send ‘P’ then send the new password.
If the receptionist wants to set a status send ‘S’ then send the new status.
Finally, the receptionist must send ‘D’ to end the command.
On this stage, the receptionist can send any number of commands.
Examples : “4P1234S1D” , “4S2D” , “4P5555D” , “4S0P0000D” ,”4S0S2P9999S1D”
Each room should be attached to a solenoid Lock and keypad which must be connected to the microcontroller. For simplicity, assume that there is only one keypad connected on a room with number 4. 
The guest can enter a password anytime, but the solenoid Lock will be unlocked only in case of the room has status 1 and the entered password matches the password which is saved by the receptionist. 
