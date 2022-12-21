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

if data_acquisition == 0:
    print("Analysing data")
    with open(data_save_directory+file_name+".json","r") as readfile:
        read_data = json.load(readfile)
        read_data = np.array(read_data, object).T.tolist() # convert read data to nparray, use T attribute to transpose and convert back to list
    #     
    #     print(read_data)
        for i in range(3,1000):
            final_split = read_data[i].split('_')
            final


            if (avg_num!=1):
                adc_avg_list.append(read_data[i:i+avg_num])
            else:
                adc_avg_list.append(read_data[i])
#             adcValue.insert(i, read_data[i])            
    #         print(adcValue)
#             adcVoltage.insert(i, (float(adcValue[i]) / 1024.0) * 5)
#             currentValue.insert(i, (offsetVoltage - adcVoltage[i]) / sensitivity)
#             currentVal.insert(i, [adcValue, adcVoltage, currentValue])
    #         count+=1
    #         print(count)
            
        for i in adc_avg_list:
            sum = 0
            if (avg_num!=1):
                for j in i:
                    sum = sum + j  
                sum = sum/avg_num
            else:
                # for j in i:
                    sum = i
            adc_avg_sum_list.append(sum)
        print("Data analysing succesful.")
        with open(data_save_directory+file_name_2+"_with_avgsize_"+str(avg_num)+".json", "w") as outfile:
            json.dump(adc_avg_sum_list, outfile)
                
    print("Analysed data written to file: ", file_name_2, "_with_avgsize_"+str(avg_num))
    #sys.exit()

    # Adapt the plot as required everytime before running
    # to plot R1/R2/R3 values and states
    plt.plot(adc_avg_sum_list, color='r', label='ADC Value')
#     plt.plot(currentValue, color='b', label='R2 value')
    #plt.plot(read_data[5][:], color='y', label='R3 value')
    #plt.plot([x for x in range(0,n)], [x*100 for x in read_data[0][:]], color='r', label='R1 state')
    #plt.plot([x for x in range(0,n)], [x*200 for x in read_data[1][:]], color='b', label='R2 state')
    #plt.plot([x for x in range(0,n)], [x*300 for x in read_data[2][:]], color='y', label='R3 state')

    # to plot R1/R2/R3 thresholds
    # plt.plot(read_data[0][:], color='r', label='R1 thr')
    # plt.plot(read_data[1][:], color='b', label='R2 thr')
    # plt.plot(read_data[2][:], color='y', label='R3 thr')

    #plt.title(image_title)
    #plt.legend()
    plt.savefig(data_save_directory+file_name_2+"_with_avgsize_"+str(avg_num)+".png")
    plt.grid(True)
    plt.show()

    #try:
     #   while True:
      #      cmd = input("Enter command :")
       #     arduino_serial.write(cmd.encode())
    #except KeyboardInterrupt:
     #   print("KeyboardInterrupt has been caught. Program terminated.") 

