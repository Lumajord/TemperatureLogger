# Temperature logger with Arduino  
Causes the Arduino to wake up every five minutes and read the temperature, humidity, and time and then saves the data to an SD card and returns to sleep.  
Uses the Low [Power library](https://github.com/rocketscream/Low-Power) for sleep.  
Uses the Real Time Clock DS3231 for time measurement and the DHT22 AM2302 sensor for temperature/humidity measurements.  

The project also contains a python script to evaluate the data.

<img src="https://github.com/Lumajord/TemperatureLogger/blob/main/datalogger.png" width="720">

## How to use
Flash the .ino file to the microcontroller and supply it with power.
Take the SD card out and load the data to a computer. Evaluate said data with the python script.
