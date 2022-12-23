import json
import time
import serial
# import RPi.GPIO as GPIO
import json
import sys
from matplotlib import pyplot as plt
import numpy as np

data_acquisition = 1 # 1 --> acquisition on, 0 --> no acquisition, only visualization of saved data
n = 1000 # no of lines to be read

write_data = []
file_name = "B1_test_raw"
file_name_2 = "B1_test_analysed"
data_save_directory = "C:\\github\\esp32_test\\data_visualization\\"
#image_title = file_name + " Raw/State Data"
image_title = file_name
currentVal = []
adcValue = []
currentValue = []
adcVoltage = []
offsetVoltage = 2.5
sensitivity = 0.1
# count = 0


if data_acquisition == 1:
# Data Acquisition
    arduino_serial = None  # serial object
    arduino_connection_status: int = 0  # 1 if Arduino Serial connected, else 0
    arduino_serial_counter: int = 0  # counter to count the number of messages sent by arduino so far
    arduino_received_frame: str = ''
    # <editor-fold desc="Setup Serial communication with Arduino ...">
    while not arduino_connection_status:
        try:
            arduino_serial = serial.Serial('COM3', 115200, timeout=1)
            arduino_connection_status = 1
            arduino_serial.reset_input_buffer()
            print("Arduino connected...")
        except:
            print("Arduino not connected. Retrying...")
            time.sleep(1)
    while arduino_serial_counter < 1300: 
        
        #try:
            if arduino_serial.in_waiting > 0:
                arduino_received_frame = arduino_serial.read_until().decode('utf-8').rstrip()
#                 line = arduino_received_frame.rstrip("\r2")
                line = arduino_received_frame
                if line != "":
                    write_data.insert(arduino_serial_counter, line)   #prev for ref: [int(x) for x in line]
                    arduino_serial_counter = arduino_serial_counter + 1  
                    print(str(arduino_serial_counter) + " : " + arduino_received_frame)
        #except:
         #   print(str(arduino_serial_counter) + " : Error")
              
    print("Raw data acquisition successful. Values received: ", n)
    with open(data_save_directory+file_name+".json", "w") as outfile:
        json.dump(write_data, outfile)
    print("Raw data written to file.")
#     arduino_serial.close()

#------------------visualizing data---------------------

image_title = "ESP32 EXP1a  (Wired PC) "
count_arr = []
time_to_ping = []
dropped_packet=[]
lastcount=0
avglat=0
if data_acquisition == 0:
    print("Analysing data")
    with open(data_save_directory+file_name+".json","r") as readfile:
        read_data = json.load(readfile)
        read_data = np.array(read_data, object).T.tolist()
        print(len(read_data))
        for i in range(20,len(read_data)):
            final_split = read_data[i].split('_')
            count_arr.append(int(final_split[0]))
            time_to_ping.append(int(final_split[2]))
            if int(final_split[0])== lastcount:
                print("count repeated",final_split[0])
            lastcount=int(final_split[0])
            if int(final_split[2])==0:
                dropped_packet.append(int(final_split[0]))
            avglat+=int(final_split[2])

        print("Data analysing succesful.")
        print(count_arr,len(count_arr))
    fig, (axs1,axs2) = plt.subplots(2)
    axs1.plot(time_to_ping, color='b', label='ping_time')
    # axs1.ylim([0,50])


    axs1.set_title(image_title)
    slots=[0,2,5,10,15]
    axs2.hist(time_to_ping,slots)
    print("totoal number of packets lost    ",len(dropped_packet) ," total packet loss %  ", len(dropped_packet)/n , "dropped counts are    ", dropped_packet, "average latency ",avglat/len(time_to_ping))
    #plt.legend()
    plt.savefig(data_save_directory+file_name_2+".png")
    plt.grid(True)
    plt.show()