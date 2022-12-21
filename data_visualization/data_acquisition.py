import json
import time
import serial
# import RPi.GPIO as GPIO
import json
import sys
from matplotlib import pyplot as plt
import numpy as np

data_acquisition = 0 # 1 --> acquisition on, 0 --> no acquisition, only visualization of saved data
n = 1000 # no of lines to be read

write_data = []
file_name = "B1_data1_raw"
file_name_2 = "B1_data1_analysed"
data_save_directory = "C:\\Users\\samar\\OneDrive\\Desktop\\esp_test_time\\"
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
            arduino_serial = serial.Serial('COM6', 115200, timeout=1)
            arduino_connection_status = 1
            arduino_serial.reset_input_buffer()
            print("Arduino connected...")
        except:
            print("Arduino not connected. Retrying...")
            time.sleep(1)
    while arduino_serial_counter < n: 
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

image_title = "ESP32 packet send and receive time"
count_arr = []
time_to_ping = []
if data_acquisition == 0:
    print("Analysing data")
    with open(data_save_directory+file_name+".json","r") as readfile:
        read_data = json.load(readfile)
        read_data = np.array(read_data, object).T.tolist() # convert read data to nparray, use T attribute to transpose and convert back to list
    #     print(read_data)
        for i in range(3,1000):
            final_split = read_data[i].split('_')
            count_arr.append(int(final_split[0]))
            time_to_ping.append(int(final_split[2]))

        print("Data analysing succesful.")
    #sys.exit()

    # Adapt the plot as required everytime before running
    # to plot R1/R2/R3 values and states
    # plt.plot(count_arr, color='r', label='Packet')
    plt.plot(time_to_ping, color='b', label='ping_time')
    plt.ylim([0,50])
#     plt.plot(currentValue, color='b', label='R2 value')
    #plt.plot(read_data[5][:], color='y', label='R3 value')
    #plt.plot([x for x in range(0,n)], [x*100 for x in read_data[0][:]], color='r', label='R1 state')
    #plt.plot([x for x in range(0,n)], [x*200 for x in read_data[1][:]], color='b', label='R2 state')
    #plt.plot([x for x in range(0,n)], [x*300 for x in read_data[2][:]], color='y', label='R3 state')

    # to plot R1/R2/R3 thresholds
    # plt.plot(read_data[0][:], color='r', label='R1 thr')
    # plt.plot(read_data[1][:], color='b', label='R2 thr')
    # plt.plot(read_data[2][:], color='y', label='R3 thr')

    plt.title(image_title)
    #plt.legend()
    plt.savefig(data_save_directory+file_name_2+".png")
    plt.grid(True)
    plt.show()