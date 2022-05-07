# MECHENG313---Traffic-Lights
ENGGEN 313 ASSIGNMENT 1: <br>
Programming ATMEGA328p microcontroller to control behaviour of LED lights

**task1** <br>
This is a basic traffic light system whereby the lights switch from red to green, green to yellow and yellow to red in a repeating manner every one second.


**task2** <br>
Here, the traffic light operates in two modes: default and configuration mode. In default mode, the traffic light will alternate from red to green, green to yellow, and yellow to red in a repeating manner every n seconds, where n is set in the configuration mode. There is also a fourth white light which remains constantly on during this cycle in default mode. To enter configuration mode, the user must press a tactile push button, and the mode change will occur once the traffic light hits red. Once in configuration mode, the red light will remain on, and the white light will flash n times in one second, and remain off for two seconds. The value of n ranges between 1-4 and can be set via manual turning of a potentiometer which is translated to ADC input into the circuit (with n=1 and n=4 at either extremes, and n=2 and n=3 spaced equally between). To exit configuration mode, the user will press the tactile push button again.

**task3** <br>
This task estimates the speed of vehicles by simulating passing traffic between two light barriers which are a known distance apart. The light barriers are called LB1 and LB2, and a car passing each respective barrier is simulated via two separate push buttons on the circuit. When the LB1 button is pressed, the red light will flash once quickly. Similarly when the LB2 button is pressed, the green light will flash once quickly. The time elapsed between each button breach is recorded, and velocity (which is converted to units of km/h) is estimated using the formula v = d*t,  where d = 20m.
The speed estimate is demonstrated using a PWM signal on an oscilloscope with a pulse period of 1s. Here the PWM duty cycle is directly proportional to the speed estimate, e.g. 10km/h is represented by a 10% signal duty cycle, 20km/h is 20%, etc. If a car is estimated to be  travelling at >= 100km/h, then  the signal duty cycle is set at 100%. The PWM will only change once both lights have been breached.


**task4** <br>
This task simulates a red-light camera. The traffic light cycles from red to green, green to yellow, and yellow to red in a repeating manner every 1 second. If a car, simulated by a tactile push button, passes through when the red LED is on, this triggers the camera. Upon the breach of the system, the white LED is programmed to blink twice at a duty cycle of 50% and pulse period of 250 ms. Every time a car gets caught by the red light camera, this is visualised via a PWM signal shown on an oscilloscope with a pulse period of 1 second. The PWM signal duty cycle is directly proportional to the amount of cars that breached the red-light camera; e.g. 10 cars correlate to a 10% PWM signal duty cycle, 20 cars correlate to 20%, etc. If there are >= 100 cars that have breached the light, the duty cycle will be set to 100%. This PWM visualisation is updated every time a new car breaches the red light.
