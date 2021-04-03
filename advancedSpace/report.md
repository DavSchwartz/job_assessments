## Data Serialization Design
To ensure the user is able to consistently issue commands without mistakes, I made an enum for all possible commands. The user chooses commands by entering the corresponding enum number. The enums start at 1 because that is the default for Python. Data is passed from the server to the client in the form of a json. Jsons are easy to write, human readable, and are supported by the Python standard library. An excellent choice for quick prototyping when performance isn't a hard requirement.  

## Development Time  
This problem probably took around 12 hours for learning and development.  

## Build Instructions  
To build the c file, pass it to a compiler and include the "windows socket 2" library. This can be done with gcc on windows using the command:  
`$ gcc -o fsw /path/to/fsw.c -lws2_32`  
or by running the makefile with the following command in the project directory:  
`$ make`  
This file can also be built on Windows with the "Developer Command Prompt" with the command:  
`C:> cl \path\to\fsw.c /link ws2_32.lib`  
Instructions for using both the server and the client are in instructions.pdf.  

## Improvements to the Program  
The user has no indication of what is happening with the server on startup. The user doesn't know exactly why the server isn't responding to commands. It may be helpful for the server to send its state on startup to indicate it is busy restarting.  