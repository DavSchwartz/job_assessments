## SERVER

To start the server, run a make command and then execute the resulting executable  
`$ make`  
`$ ./fsw.exe`  

To specify the IP address and port, either change the ADDRESS and PORT variables in the source, or pass "a" and/or "p" to the executable as options.  
`$ ./fsw.exe -a "192.168.1.17" -p 8080`  

The IP address should be associated with the machine running the code, probably the one given to it by the router/modem. The port must be in the range [1024, 49151]. The IP address and port must be identical on the client and server, or the client must be redirected to the server by a networking device.  


## CLIENT

To run the client, run gcs.py  
`$ python gcs.py`  

To specify the IP address and port, either change the ADDRESS and PORT variables in the source, or pass "a" and/or "p" to the executable as options.  
`$ python gsw.py -a "192.168.1.17" -p 8080`  

To send commands to the server, enter numbers corresponding to commands in the prompt. The server will respond with "Server says:"  

##### PROMPT
    invalid: 1
    safe mode enable: 2
    safe mode disable: 3
    show number of commands received: 4
    show number of safe modes: 5
    show up time: 6
    reset command counter:  7
    shutdown: 8

    So... what will it be, boss?
    Type a number: 

##### RESPONSE
    Server says:
    state -> Ready
