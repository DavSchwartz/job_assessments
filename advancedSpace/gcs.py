import socket
from enum import IntEnum
import json
import argparse

# Enum of available commands
class Command(IntEnum):
    Undefined = 1
    SafeModeEnable = 2
    SafeModeDisable = 3
    ShowNumCommands = 4
    ShowNumSafeModes = 5
    ShowUpTime = 6
    ResetCommandCounter = 7
    Shutdown = 8
MAX_COMMAND_NUM = 9

# defaalt IP address to connect to
ADDRESS = '127.0.0.1'
# default port to connect to
PORT = 8080

# user prompt to request input
PROMPT = (
    "\n"
    "invalid: " + str(int(Command.Undefined)) + "\n"
    "safe mode enable: " + str(int(Command.SafeModeEnable)) + "\n"
    "safe mode disable: " + str(int(Command.SafeModeDisable)) + "\n"
    "show number of commands received: " + str(int(Command.ShowNumCommands)) + "\n"
    "show number of safe modes: " + str(int(Command.ShowNumSafeModes)) + "\n"
    "show up time: " + str(int(Command.ShowUpTime)) + "\n"
    "reset command counter:  "+ str(int(Command.ResetCommandCounter)) + "\n"
    "shutdown: " + str(int(Command.Shutdown)) + "\n"
    "\n"
    "So... what will it be, boss?\n"
    "Type a number: "
)

# check if a string is an int
def IsInt(s):
    try: 
        int(s)
        return True
    except ValueError:
        return False

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument("-a", type=str, help="IP address to connect to")
parser.add_argument("-p", type=int, help="Port to connect to")
args = parser.parse_args()
if args.a:
    ADDRESS = args.a
if args.p:
    PORT = args.p

# connect to server and issue commands
print("Just wait a hot second, my dude.")
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((ADDRESS, PORT))
    print("Alright, alright, cool. Connection established. YOU'RE IN!")
    while True:
        command = input(PROMPT)
        if (IsInt(command) and (int(command) >= 0) and (int(command) <= MAX_COMMAND_NUM)):
            s.sendall(bytes(command, 'utf-8'))

            data = s.recv(1024).decode("utf-8")
            data = json.loads(data)
            print("\nServer says:")
            for key in data:
                print(key, '->', data[key])
            print("")

            if (Command(int(command)) == Command.Shutdown):
                break
        else:
            print("\nHmm, no. Did I say that was an option?")

print("This is Bravo Six, going dark.")
