import socket
import os
import time

host = '192.168.2.111'
port = 12345
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)

client.connect((host, port))
heart = "1".encode("utf-8")

while True:
    client.send(heart)
    choice = client.recv(1)
    print(choice)
    # question
    if choice == b'0':
        os.system("sudo ./display 0")

    # no with
    elif choice == b'1':
        with open("log.txt","a+") as f:
            f.write("no mask： " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '\n')
        os.system("sudo ./display 1")
        
    # with
    elif choice == b'2':
        with open("log.txt","a+") as f:
            f.write("has mask： " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '\n')
        os.system("sudo ./display 2")

    else:
        print("choice err")
        #exit(1)
	


