# solar-cam-firmware

![An image of the camera](https://github.com/floftus-at-csm/solar-cam-firmware/blob/main/DSC02843_brightness_upped.png)
##### Table of Contents  
1. [List of Hardware Components](#list_of_components)  
2. [Parts](#parts)  
 1.[The Body](#body)
 2.[The Case](#case)
## List of Hardware Components
<a name="list_of_components"/>
Camera: 
* WP-DC800 Camera Case 
* ESP32-Cam with 8MB PSRAM
* DFRobot Solar Power Manager 5V
* Micro SD Card
* 5V 1W Solar Panel
* Rechargeable 3.7V Li-ion Battery 3000mAh
* Battery level circuit
* 3D Printed Parts
  * Lens holder
  * Internal housing
  * Solar panel holder
Image processor:
* Raspberry Pi Zero 2 W
* 6V 1W Solar Panel
* DF Robot Solar charge manager 
* Case

## Parts
<a name="parts"/>
### The Body 
<a name="body"/>
The body of the camera is a waterproof case from an old point-and-shoot canon camera. 
My original intention with the project was to re-use old hardware. I detail the struggles I had with this in the re-use section. My hardware is fitted using 3D printed parts that have been modelled around the case.

After trying and failing to reverse engineer the firmware of old point-and-shoot and video cameras I decided to use my fallback option: an ESP32-Cam (which I detail in the next section) with my own powering system. None of this hardware is weatherproof and so a case was required. Designing a weatherproof camera case from scratch would require designing the body around the parts, creating the body (e.g. 3D printing, CNC’ing + moulding), creating a waterproof seal, then testing the body for waterproofing and iterating on the design. That would require a lot of time, energy and resources. So, I found an alternative. 

Luckily there is an abundance of existing cases for cameras that people in general don’t want anymore: waterproof cases for obsolete (or at least aging) digital cameras. These are mostly from point-and-shoot cameras that create lower quality images than many of today’s smartphones.

### The Case
<a name="case"/>
The case I used is called a WP-DC800. At the time of writing, these bodies go for between £5-15. There are many others that you might be able to find. I chose this one because the cameras it is designed for are larger than the sum of my parts and so the interior is relatively roomy. This gave me space for my amateur 3D design. 

I couldn’t find a set of technical drawings for the case or for the cameras it was designed for, so I had to map it out myself. If anyone knows of any it would be great to hear from you. Likewise, if anyone has a quick and reliable method for mapping out complex 3D objects it would be great to hear from you too.

When I was designing how to attach my hardware to the cases, my main considerations were:
* how to attach the solar panel to the outside 
* how big the battery was
* how to fix my lens 

The benefits of this case were:
* some of buttons are removable which creates holes you can use to fix elements to the outside of the case 
* waterproofing 
* the case is relatively large so the components fit 

If you are considering using a different case that is any smaller you’ll probably want to use a smaller battery. Some are designed as flat cuboids like the ones in mobile phones which would be easier to fit into a smaller case.
When I get time I'll try to design the 3D components for different second hand cases.


### The Solar Holder
<a name="solar_holder"/>
The main change between this camera and the one that this case was designed for is how its powered. This camera is charged as it goes so that it doesn’t need to be attached to mains. I chose to use solar power, which I detail in the power section. I chose a cheap, small and low-power panel. 
You could use any other panel, just change the show of the top of the solar panel holder to match yours. 
You can use a sealant to make the panel holder waterproof.
You could, for example, salvage a panel from an unused or broken garden light.

### The Interior Housing
<a name="interior_housing"/>
The microcontroller (ESP32-Cam), power manager, battery and battery level circuit are connected via a 3D printed part, which connects to the housing via some internal fixtures. I opted to use a 3D printed module and a small breadboard to allow for some flexibility. In the future it would make sense to move this to a PCB
There is also a 3D printed lens holder that fixes the len in place. I have designed these for two lenses, the standard and fish-eye OV2640 lenses.
I intend to add at least one button and one RGB LED to the circuit to make it clearer what mode the camera is in. This button needs to line up with one of the exterior buttons

### The Camera Module
<a name="camera_module"/>
After failed attempts at re-using existing hardware I opted to use an ESP32-Cam. I chose this module because:

#### Cost
£8-15

#### Power
<a name="camera_power" />
An ESP32-Cam is a microcontroller, meaning (in simple terms) it only runs one script at a time. Compared to a single board computer like a Raspberry Pi, which is running an entire operating system (lots of scripts running in the background) this module draws much less current and so is lower power.
In active mode it will draw between 160 and 260mA (0.240A) at 5V so between 0.8 and 1.3W (https://lastminuteengineers.com/esp32-sleep-modes-power-consumption/ This includes using the wifi, radio and bluetooth)
In partially active mode, which the camera is in for the majority of its processing it will draw around 0.8mA (0.0008A) so 0.004W
The ESP32-Cam also has a deep sleep mode during which the main CPU is shut down and only an UltraLowPower Coprocessor runs. This allows it to draw only around 10µA (0.000010A). I have the device running at 5V so the power consumption is 0.0000050W


##### Comparison 1: A Raspberry Pi Zero 2 W
This board is at the smallest end of single-board computers. 
When stressed, it will draw around 550mA (0.550A), so around 2.75W and around 240mA (0.240A) in idle mode

##### Comparison 2: Canon 800D
https://ffseb.files.wordpress.com/2017/10/800dpoweruse.pdf
This device will use between 120 and 365mA in standard modes at 8V so between 1 and 3W, and 550 - 990 mA in continuous shooting mode, so between 4.4 and 9.9W
A timelapse, which is an equivalent to the processes done on my cameras draws between 0.83 and 2.55W


#### Image Sensor 
##### OV2640 Sensor
1600 x 1200 px
This is the main ‘payout’ of the ESP32-Cam. The image sensor is fairly tiny. As I discuss in the image-making process this limitation can be viewed in some sense beneficially, reducing the storage size and forcing more creative ways of processing and displaying the images. 

There is potential to ‘upgrade’ to a ESP32 board that uses the OV5640 sensor (2592 x 1944 https://www.waveshare.com/wiki/OV5640_Camera_Board_(C)) I haven’t used this yet but it should work.


##### Comparison 1: Canon 800D
6000 x 400 px

##### Comparison 2: Iphone 11
4000 x 3000 px 

#### Lens
160 Degrees Fish-eye Lens
£5

Standard OV2640 lens
£3


#### Open-Source Firmware and GPIO Pins
All of the code needed to use the ESP32-Cam is open-source. The camera is also fully programmable. This means we can create versions of this firmware and create custom image creation processes.
The GPIO pins mean we can control the device from a sensor, like a microphone or distance sensor.
For example, experimenting with the function that moves the image sensor readings into temporary memory.
The firmware I have written uses Espressif’s ESP32-Cam library. Details of this firmware are in the manual but the general gist of it is like this:
* The camera has a settings mode where it is accessible via a Wifi connected device such as a mobile phone or a laptop. You can use a website I’ve designed to adjust the settings,  check the result of the settings and then choose what imaging mode you’d like to use
* A series of imaging modes, some that use timers, some using sensors, some for multiple connected cameras, some for one individual camera. The imaging modes correspond to different image processes. Currently they’re 1-1 but they could easily be 1-many or many-many
* Turns off when the battery is too low

Wifi and Bluetooth Capabilities
This allows for the possibility of ‘connecting up’ multiple cameras. I intend to use this to do some collaborative photographs, where multiple cameras and camera users collaborate to create photographs together.
This also allows us to control the camera from another device, like a smart phone or another ESP32. This can include:
Changing what image process to do
Changing the settings for the sensor (exposure, brightness, contrast etc.)
Controlling the camera shutter from another device (say a device connected to an underground microphone)
Creating time-based processes that are not just timelapses or panoramas
This also allows us to reprogram the device without dismantling the device. We can add new image processes, or change settings via Wifi. This is done via Over The Air (OTA) programming. 


### Power
<a name="power"/>
My aim was to ensure this camera never needs to be connected to mains power. It is powered by one small 5V 1W solar panel that charges a 3.7v 3000mAh battery via a power manager board (made by DFRobot). 
For the power requirements of the camera see:
[ESP32-CAM Power Usage](#camera_power) 

#### Solar Panel
The solar panel is very cheap and is essentially generic. You could replace it with a larger power panel, but I’d recommend keeping the voltage at 5V so that it works with the board. It would be fun to salvage a solar panel from another device rather than getting something new. If you have one you'd like to use, feel free to let me know to see if I can help. If you use a different panel to mine, you’ll need to modify the Solar Panel holder 3D model. 

#### Power Manager
The power manager (https://wiki.dfrobot.com/Solar_Power_Manager_5V_SKU__DFR0559): 
allows the battery to be charged while the camera is in use
the new version also has protection circuits which is very important, since lithium-ion batteries can be very dangerous if not handled properly (e.g. never store them completely full)
Steps up the voltage from the battery (3.7V) to 5V with a steady current draw up to 1A. The ESP32-Cam runs at 5V and has a max draw of around 260mA so this board is perfect.
Maximises the efficiency of the solar panel with MPPT
It’s a really great board (thanks to Marguerite for showing it to me!). I tried to design a similar circuit for myself and it was pretty tricky, this board handles a lot of electronics.

#### Battery
The battery is a rechargeable lithium-ion battery. Ecologically, these are extremely problematic and are in almost all new tech today. My aim is to design the firmware to maximise the longevity of the battery. The larger the amp hours the longer the camera will last without resorting to deep sleep. I am yet to experiment with smaller battery sizes but I hope to in the future.
I would also like to experiment with salvaging batteries from devices that are not used to their limit. For example, Elf Bar vapes are made with rechargeable batteries that are never recharged.

