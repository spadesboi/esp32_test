from matplotlib import pyplot as plt

string = "_0:0_1:83430_2:14587_3:1479_4:318_5:0_6:115_7:39_8:12_9:8_10:6_11:1_12:4_13:0_14:1_15:0_16:0_17:1_18:0_19:0_20:0_3_348946"

list = string.split("_")
a = []
for i in range(1, len(list)-2):
    x = list[i].split(":")
    a.append(int(x[1]))

end_time = int(list[len(list)-1])
avg_lat = int(list[len(list)-2])
b = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100]

fig = plt.figure(figsize = (20, 5))
plt.bar(b, a, color ='maroon', width = 1)

plt.xticks(b)
plt.xlabel("Latency in ms")
plt.ylabel("No. of packets")
plt.title("BENCH: Latency of packets with 100ms delay ESP32 with average latency: " + str(avg_lat) + "ms, end time: " + str(end_time) + "ms")
plt.show()