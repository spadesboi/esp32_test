import socket
import time
localip="192"
localPort = 20001
UDPServerSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM )
# UDPServerSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

UDPServerSocket.bind(("192.168.0.200", localPort))
count=0

timeout_start = time.time()
while True:
    bytesAddressPair = UDPServerSocket.recvfrom(100)
    message = bytesAddressPair[0]
    address = bytesAddressPair[1]
    message=message.decode()
    clientMsg = str(message)
    # clientIP = "Client IP Address:{}".format(address)
    print(address,clientMsg)
    # print(clientMsg.encode())
    # print(clientIP)   
    # print("count",count)
    # count+=1  
    # if count > 1000:
    #     UDPServerSocket.sendto(str.encode("start"), address)
    # else:
     
    UDPServerSocket.sendto(clientMsg.encode(), address)
   
# import socket
# UDP_IP = "192.168.2.113" # The IP that is printed in the serial monitor from the ESP32
# SHARED_UDP_PORT = 4210
# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Internet  # UDP
# sock.connect((UDP_IP, SHARED_UDP_PORT))
# def loop():
#     while True:
#         data = sock.recv(2048)
#         print("data",data)
# if __name__ == "__main__":
#     count=0
#     while True:
#         text="sending count ",count
#         if count>=1000:
#             text="end"
#         text=str(text)
#         sock.send(text.encode())
#         count+=1
        
            
#     loop()