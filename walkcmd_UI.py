
#this code will also send commands to the raspberrypi3 utilizing UDP


import socket

#UDP_IP = "10.166.164.70" # IP of RPi when on Mason network
#UDP_IP = "192.168.2.4"
UDP_IP = "192.168.1.228"
#UDP_PORT = 5006
UDP_PORT = 5007

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP

while True:
  inp = raw_input("Type Command:") #taking input from the user

  #these keys are to move the servos
  if   inp == "w":
  	print("forward")
  	sock.sendto(inp, (UDP_IP, UDP_PORT))
  elif inp == "x":
	print("backward")
	sock.sendto(inp, (UDP_IP, UDP_PORT))
  elif inp == "d":
	print("right")
	sock.sendto(inp, (UDP_IP, UDP_PORT))
  elif inp == "a":
	print("left")
	sock.sendto(inp, (UDP_IP, UDP_PORT))
  elif inp == "s":
        print ("stopping")
	sock.sendto(inp, (UDP_IP, UDP_PORT))
  elif inp == "l":
        print ("right leg only")
	sock.sendto(inp, (UDP_IP, UDP_PORT))
  elif inp == "k":
        print ("left leg only")
	sock.sendto(inp, (UDP_IP, UDP_PORT))
  else:
	print("Input not recognized ")


