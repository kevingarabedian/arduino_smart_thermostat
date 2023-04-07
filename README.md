This code reads the temperature from the DHT22 sensor, displays it on the LCD, and controls the relay based on the setpoint temperature. Connect the relay to the appropriate control wires for your HVAC system (W1 for heating) and make sure to connect the common wire (C) to the Arduino ground.

Please note that working with HVAC systems can be dangerous if you are not familiar with their wiring and operation. If you are unsure about any part of the process, consult a professional HVAC technician to ensure your safety and the proper functioning of your system.

In order to build a smart thermostat using an Arduino, you can use an Arduino Uno, a DHT22 temperature sensor, a relay module, and a 16x2 LCD display. The code below is a simple example of how to set up a basic thermostat to control your HVAC system. Since your heat pump is currently disconnected, this example focuses on controlling the forced air gas heating system.

You can modify and expand the code as needed, but this should serve as a good starting point.
