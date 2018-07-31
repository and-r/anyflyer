# AnyFlyer
Simple, but realistic flight simulator controlled by mouse. You can modify aerodynamics of your plane easily by script-like files.
## General info
The project was created, on the one hand: because of my passion for aviation, physics and programming. I wanted to create my own virtual world where any winged machine, defined in a few files, could fly and be controlled by user, even without a joystick.

On the other hand, as a beginner programmer, I wanted to train my C++ skills, learn operations in 3D world with vectors, quaternions and matrices while using a 3D library/engine (Irrlicht was chosen).

## Screenshot
![Screen](./screen.jpg)

## Technologies
* C++11
* Irrlicht 1.8.4
* Visual Studio 2017 / Windows SDK 10.0
* QT Creator / Linux Mint

## Setup
Windows, 64 bit systems: download and unpack .zip file from the repository main folder, run anyflyer64.exe. If it does not work, you may need to download and install the Visual Studio 2017 C++ redistributable (x64):

https://aka.ms/vs/15/release/vc_redist.x64.exe

but the 2015 version should also be sufficient

## How to use
After running .exe file, you are in Main Menu, from here you can start the simulation by choosing Fly, change the settings by Plan your flight, or exit application. When the simulation is started, you are at first in pause mode, which you can turn on/off by space key. You can always enter the Simulation Menu (either paused or unpaused) by pressing ESC. When the simulation goes on unpaused, you can operate the menu only by cursor keys/enter because mouse is used to move the stick. When pause is enabled, you can also use mouse pointer to click menu items. Full key/mouse function list you can find in: Simulation Menu/Keys and control description.

In the current version of the project, you have three aircraft available (however you can modify them and add new ones).
They are: TS-11 Iskra advanced trainer jet, powered paraglider (Paramotor) and Tomahawk missile. The most easy to fly is Iskra. When flying the Paramotor, use throttle carefully, default 75% from start will make it turn up side down and eventually stall. Controlling Tomahawk is not easy here, but I guess it would not be in reality either - mass of about 1300 kilograms and very small wings make it very prone to stall and spin. Anyway, Tomahawk is not designed to be controlled by a human with stick - the missile computer does the job.
### Manipulating files
You can alter general settings in settings.txt file. There are three typed of data: variable names in parentheses (), following them, but gapped with space variable values in brackets {}, and comments without brackets. 

Aircraft data, placed in aircraft directory, consist of three types of files: .dat files contain physical properties, they are arranged similarly to settings.txt., Files with .3ds extension are mesh files of 3D Studio type. There are also textures, one for each aircraft is minimum, each texture should correspond with one mesh in the .3ds file.

## Project status
The project is in an initial status, only basic functions work, there are many features to add. However, at this point, a question arises: What is the project's final goal, what and how should it be in the 1.0 version? Well, me, as it's creator, can not precisely answer it because my motivation was mainly just making it, not preparing to a certain set of features :)  

Anyway I can imagine functionalities that 'must' be included. First is release as Linux package. I think soon it will be ready. As for a simulator, most important feature is landing gear and landing. Among others, I can imagine:
* flaps
* propeller engines which generate torque
* new cameras (already two modes are implemented)

Yet another features depend on if, and in what direction I (or maybe also other contributors) would like to develop it.

## License
My work itself is licensed under MIT license, you can find it in LICENSE file in main repository folder.

Irrlicht, a 3D graphics engine which my application uses, is licensed under zlib/libpng (which allows free use both commercial and non-commercial).

Subsequently, Irrlicht Engine is based in part on the work of the Independent JPEG Group, the zlib and libpng.

## Contact
AnyFlyer is created by Andrzej Rzoska, feel free to contact me!

and_r@o2.pl

