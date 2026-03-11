# e-Angel  

by Lilli (https://github.com/foreverearthmover) and Valeria (https://github.com/c-valeria-mj)

<img src="media/images/eAngel.png" alt="angel tamagotchi" width="710" height="510">

### Navigation

```
/docs
  /concept
  /circuits
  /diagrams
/code
  /demos
    demo1.ino
  /src
    angel_firmware.ino
/enclosure
  sketches/
  models/
/media
  circuits/
  demo_video/
  images/
  videos/
README.md
```

| Item                               | Description                                                              |
|------------------------------------|--------------------------------------------------------------------------|
| [docs](./docs)                     | Documentation for our digital pet.                                       |
| [code](./code)                     | Code for the tamagotchi is here.                                         |
| [enclosure](./enclosure)           | This is where you can find information about the tamagotchi case.        |  
| [media](./media)                   | All photos and videos as well as bitmaps used can be found here.         |

### Project overview  
In this project we worked on a pocket guardian angel in the form of a tamagotchi that reacts to button presses and enters different states based on interactions with the user.  

### Dependencies  
Open your Arduino IDE and go to Sketch > Include Library > Manage Libraries, install these:  
Adafruit_GFX Library  
Adafruit_SSD1306 Library  
CuteBuzzerSounds Library

#### Hardware list  
- Arduino Nano
- 4x Mini Breadboard
- Jumper Wires
- 1x 10K Resistor
- 1x Tilt Switch
- 3x Buttons
- 0.96" I2C OLED Display (SSD1306 Chip)
- Passive Buzzer
- USB to Mini-USB cable for Nano

#### Wiring diagrams  
<img src="media/circuits/final_schematic.JPG" alt="Wiring diagram" width="300" height="200">

#### Photos of prototypes  

#### Usage  
Download "ino_code.ino" and "bitmaps.h" from the [final_code](./code/final_code) folder and upload both files in your Arduino IDE. The files must be in the same directory.
