import serial
import matplotlib.pyplot as plt
import time


ser = serial.Serial('/dev/cu.usbserial-1110', 115200, timeout=1) 
time.sleep(2)


timestamps = []
values1 = []
values2 = []

plt.ion()
fig, ax = plt.subplots()
line1, = ax.plot([], [], label="The encoder count for the right wheel", color="blue")
line2, = ax.plot([], [], label="The encoder count for the left wheel", color="orange")
ax.legend()
ax.set_xlim(0, 10)
ax.set_ylim(0, 100)
plt.title("Real-Time velocity Data")
plt.xlabel("Time (s)")
plt.ylabel("velocity")

start_time = time.time()

try:
    while True:
        line = ser.readline().decode('utf-8').strip()
        if line:
            print(line)

            #"900,92.57,90.00,0.00,5.47,"
            try:
                parts = line.split("\t")
                if len(parts) >= 1:
                    value1 = float(parts[0]) 
                    value2 = float(parts[1])
                    elapsed_time = time.time() - start_time


                    timestamps.append(elapsed_time)
                    values1.append(value1)
                    values2.append(value2)


                    line1.set_xdata(timestamps)
                    line1.set_ydata(values1)
                    line2.set_xdata(timestamps)
                    line2.set_ydata(values2)
                    ax.set_xlim(0, elapsed_time)
                    ax.set_ylim(0, max(max(values1), max(values2)) + 10)
                    plt.pause(0.01)
            except ValueError:
                print("Invalid data format")
except KeyboardInterrupt:
    print("Stopped by user")
    ser.close()

# 保存最终图表
plt.ioff()
plt.savefig("velocity_data_plot.png")
plt.show()
