# EthercatMotorExample
This is an example of controlling an EtherCat motor using the soem Raspberry PI library.

I worked on EtherCat motors for a time at a company.  They were trying to use the SOEM library and
a Respberry PI 4 to control a motor.  In the end they abandoned the project.  I have taken what I have figured
out and made this example of controlling an EtherCat motor.

I couldn't find an example of controlling a motor using EtherCat out on the net, so I put this together.
Maybe this will be helpful for someone else (maybe not).

The motor is controlled using profile position mode (where you tell the motor to goto a position and it does it without
being synchronized with any other motor or from the control).

This is only a basic example and has only been tested on 1 motor (the motor had design problems making it hard to figure things
out on, so there could be problems with this example as well).

## What you need
You need to have a Raspberry PI 4 with a SPI W5500 connected up to it and an EtherCat motor.

See http://www.simplerobot.net for examples of how to connect up the W5500 and use the SOEM library.

I will only go into using this example and assume you have correctly connected the hardware and got the SOEM library working.

I have included some instructions for SOEM-W5500-rpi just to make things easier, but you should look at the SOEM
library for details (https://github.com/thanhtam-h/soem-w5500-rpi).

## Step by Step instructions

I assume you have already connected the motor as the first (and only) EtherCat device, connected the W5500 chip, and have
your Raspberry PI 4 ready to go (and that it has access to the internet).

### SD card

We start by making a standard RPI SD card and then changing it for our needs.

 * Download the Raspberry Pi Imager from https://www.raspberrypi.com/software
 * Enabled SSH (setting your login and password)
 * Select the Raspberry PI OS 32-bit (must be 32-bit)
 * Make the SD card
 * Put the SD card in the RPI and boot
 * Login to RPI

#### Update to the latest OS files and try on SPI
 * "sudo apt update"
 * "sudo apt dist-upgrade"
 * add "dtparam=spi=on" to your /boot/config.txt (echo "dtparam=spi=on" >>/boot/config.txt as root)

#### Change the kernel to one that will work with SOEM and the W5500 chip
 * "cd ~"
 * "git clone https://github.com/thanhtam-h/rpi4-xeno3.git"
 * "cd rpi4-xeno3/prebuilt"
 * "chmod +x deploy.sh"
 * "./deploy.sh"
 * reboot

#### Building modules for the new kernel
 * Login to RPI
 * "cd /usr/src/linux-headers-4.19.86-v7l-ipipe"
 * "sudo make -i modules_prepare"

**There WILL be errors, don't worry about it :)**

 * Add "dwc_otg.fiq_enable=0 dwc_otg.fiq_fsm_enable=0 dwc_otg.nak_holdoff=0" to the end of /boot/cmdline.txt
 * Add "isolcpus=0,1 xenomai.supported_cpus=0x3" to the end of /boot/cmdline.txt
 * Add "total_mem=3072" to the **TOP** of /boot/config.txt
 * reboot

#### Installing the EthercatMotorExample
 * Login to RPI
 * "git clone https://github.com/TheBeef/EthercatMotorExample.git"
 * "cd EthercatMotorExample"

#### Installing the SOEM-W5500-RPI
 * "cd ~/EthercatMotorExample"
 * "git clone -b rpi4-xenomai-3 https://github.com/thanhtam-h/soem-w5500-rpi.git"
 * "cd soem-w5500-rpi"
 * "chmod +x *.sh"
 * "./build.sh"

##### Test SOEM-W5500-RPI
 * "cd test/slaveInfo"
 * "sudo ./slaveinfo wiz"

#### Build the example

