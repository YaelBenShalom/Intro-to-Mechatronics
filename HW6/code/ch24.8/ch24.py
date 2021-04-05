# chapter 24 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
ser = serial.Serial('/dev/ttyUSB0',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

ser.write(b'0.2 0.1\n') # Kp Ki

sampnum = 0
read_samples = 10
ADCval = []
ref = []
pwm = []
while read_samples > 1:
    data_read = ser.read_until(b'\n',50)
    data_text = str(data_read,'utf-8')
    data = list(map(int,data_text.split()))

    if(len(data)==4):
        read_samples = data[0]
        ADCval.append(data[1])
        ref.append(data[2])
        pwm.append(data[3])
        sampnum = sampnum + 1

# plot it
import matplotlib.pyplot as plt 
t = range(len(ADCval)) # time array

fig, ax = plt.subplot(2, 1)

ax[0].plot(t,ADCval,'r*-',t,ref,'b*-')
ax[0].ylabel('value')
ax[0].xlabel('sample')
ax[1].plot(t,pwm,'g*-')
ax[1].ylabel('pwm value')
ax[1].xlabel('sample')
plt.show()

# be sure to close the port
ser.close()