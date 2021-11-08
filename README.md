# xenomai-ros2-20sim pipeline

Repository for the master Thesis of Bram Meijer. Creating a pipeline for integrating 20-sim models into real-time and ROS2 with the help of xenomai.

## Xenomai installation

These steps will help you through the installation of xenomai on the raspberry pi 4. The kernel that is used is the 4.19.86 linux kernel. Patches from [Tantham-h](https://github.com/thanhtam-h/rpi4-xeno3) are used to help the installation and make the USB work through the PCIE-bus that is added in the Raspberry pi 4. 

**Requirements:**
- Raspberry pi 4
- 16gb Micro-SD card + reader
- Computer with Ubuntu
- Wifi/LAN connection 

### Directory initialization

1. We start by pulling the linux repository and the patches on the host computer.
    ```console
    host@ubuntu:~$ git clone git://github.com/raspberrypi/linux.git linux-rpi-4.19.86-xeno3
    ``` 

(if git is not installed, install it by using `sudo apt-get updates` and then `sudo apt-get install git`)

2. Then we enter the directory and reset the git repository to 
    ```console
    host@ubuntu:~$ cd linux-rpi-4.19.86-xeno3
    host@ubuntu:~/linux-rpi-4.19.86-xeno3$ git reset --hard c078c64fecb325ee86da705b91ed286c90aae3f6
    host@ubuntu:~/linux-rpi-4.19.86-xeno3$ cd ..
    ```

3. Then we create a linked folder for easy access.
    ```console
    host@ubuntu:~$ ln -s linux-rpi-4.19.86-xeno3 linux
    ``` 

4. Download the xenomai tar.bz2 and extract it. Also create a linked folder for the xenomai installation.
    ```console
    host@ubuntu:~$ wget https://xenomai.org/downloads/xenomai/stable/xenomai-3.1.tar.bz2
    host@ubuntu:~$ tar - xjvf xenomai-3.1.tar.bz2
    host@ubuntu:~$ ln -s xenomai-3.1 xenomai
    ```

5. Download the patches into xeno3-patches
    ```console
    host@ubuntu:~$ mkdir xeno3-patches && cd xeno3-patches
    host@ubuntu:~/xeno3-patches$ wget https://github.com/thanhtam-h/rpi4-xeno3/blob/master/scripts/ipipe-core-4.19.82-arm-6-mod-4.49.86.patch
    host@ubuntu:~/xeno3-patches$ wget https://github.com/thanhtam-h/rpi4-xeno3/blob/master/scripts/pre-rpi4-4.19.86-xenomai3-simplerobot.patch
    host@ubuntu:~/xeno3-patches$ cd ..

## Patching linux with xenomai

1. Patch linux with the pre-patch from Tantham-h
    ```console
    host@ubuntu:~$ cd linux
    host@ubuntu:~/linux$ patch -p1 <../xeno3-patches/pre-rpi4-4.19.86-xenomai3-simplerobot.patch
    ```
    (If the patch returns with error, just ignore)

2. Patch linux with xenomai
    ```console
    host@ubuntu:~/linux$ ../xenomai/scripts/prepare-kernel.sh --linux=./ --arch=arm --ipipe=../xeno3-patches/ipipe-core-4.19.82-arm-6-mod-4.49.86.patch
    ```

## Building linux

1. Get the default config of the BCM2711 into the linux directory
    ```console
    host@ubuntu:~/linux$ make -j8 O=build ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
    ```
2. Set the menuconfig to the right settings
    ```console
    host@ubuntu:~/linux$ make -j8 O=build ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
    ```
    Edit the following variables:

    Kernel features --->

        Timer frequency 1000Hz

    General setup --->

        (-v72-xeno3) Local version - append to kernel release

    CPU power management --> CPU Frequency scaling -->

        [] CPU Frequency scaling

    Memory Managament options --->

        [] Allow for memory compaction

    Kernel hacking --->
        
        [] KDGB: kernel debugger

    Xenomai/cobalt ---> Drivers ---> Real-time GPIO drivers --->

        [*] GPIO controller

    Xenomai/cobalt ---> Drivers ---> Real-time IPC drivers -->

        [*] RTIPC protocol familty

3. Build the linux kernel, edit the -jX variable so that 'X' is: (your number of CPU's - 1). 
    ```console 
    host@ubuntu:~/linux$ make –jX O=build ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bzImage modules dtbs
    ```
    (This can take some time, so get coffee or tea...)

## installing the linux kernel on the Micro-SD card

1. Create a new raspbian image on the micro-SD card with the [Pi imager](https://www.raspberrypi.org/software/). Choose the "Raspberry PI OS 32-Bit lite" version.

    Also create a shortcut to the identifier for the PI4 ARM version of the kernel: 
    ```console
    host@ubuntu:~/Linux$ KERNEL=kernel7l
    ```

2. In terminal, locate the fresh installed SD card and see which mounts points are created:\
    ```console
    host@ubuntu:~/Linux$ lsblk
    sdb
        sdb1
        sdb2
    ```
    with sdb1 being the FAT (boot) partition, and sdb2 being the ext4 filesystem (root) partition.

3. Create mount points for both these partitions and mount the sd card to them.
    ```console
    host@ubuntu:~/Linux$ mkdir mnt
    host@ubuntu:~/Linux$ mkdir mnt/fat32
    host@ubuntu:~/Linux$ mkdir mnt/ext4
    host@ubuntu:~/Linux$ sudo mount /dev/sdb1 mnt/fat32
    host@ubuntu:~/Linux$ sudo mount /dev/sdb2 mnt/ext4   
    ```
4. Install the modules of the build linux system and install them on the root partition.
    ```console
    host@ubuntu:~/Linux$ sudo env PATH=$PATH make O=build ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=mnt/ext4 modules_install    
    ```
5. Install the kernel image, bts packets and the overlays into the boot partition.
Also the original kernel image is saved, in the case that the kernel does not boot.
    ```console
    host@ubuntu:~/Linux$ sudo cp mnt/fat32/$KERNEL.img mnt/fat32/$KERNEL-backup.img
    host@ubuntu:~/Linux$sudo cp arch/arm/boot/zImage mnt/fat32/$KERNEL.img
    host@ubuntu:~/Linux$sudo cp arch/arm/boot/dts/*.dtb mnt/fat32/
    host@ubuntu:~/Linux$sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/fat32/overlays/
    ```
6. Edit the neccessary boot configs and cmdline such that the pi can boot, also add the ssh access file.
    ```console
    host@ubuntu:~/Linux$ touch mnt/fat32/ssh
    host@ubuntu:~/Linux$ sudo nano mnt/fat32/cmdline.txt
    ```
    Add:
    `dwc_otg.fiq_enable=0 dwc_otg.fiq_fsm_enable=0 dwc_otg.nak_holdoff=0 isolcpus=0,1 xenomai.supported_cpus=0x3` at the end of the FIRST line.
    ```console
    host@ubuntu:~/Linux$ sudo nano mnt/fat32/config.txt
    ```
    Add somewhere in the beginning: 
    ```
    total_mem=3072
    ```
7. Unmount the SD card and insert it into the Raspberry pi and power it up with the ethernet cable attached to the host computer.
    ```console
    host@ubuntu:~/Linux$sudo umount mnt/fat32
    host@ubuntu:~/Linux$sudo umount mnt/ext4
    ```

8. Boot the raspberry pi and ssh into it, check the linux kernel with `uname -r`

## Installing the xenomai libraries on the Raspberry pi 

1.  Also on the host PC, build the xenomai libraries.
    ```console
    host@ubuntu:~$ cd xenomai
    host@ubuntu:~/xenomai$ ./scripts/bootstrap
    host@ubuntu:~/xenomai$ ./configure --host=arm-linux-gnueabihf --enable-smp --with-core=cobalt
    host@ubuntu:~/xenomai$ make
    host@ubuntu:~/xenomai$ sudo make install
    host@ubuntu:~/xenomai$ tar -cjvf rpi4-xeno3-deploy.tar.bz2 /usr/xenomai
    ```

2. Copy the constructed tar.bz2 file to your raspberry pi.

    ```console
    host@ubuntu:~/xenomai$ scp rpi4-xeno3-deploy.tar.bz2 pi@<raspberry pi IP address> 
    ```
    (if you do not know the pi IP addresss check it using a ipscanner)
3. Switch to the raspberry pi (SSH into the raspberry pi)
    ```console
    host@ubuntu:~/xenomai$ ssh pi@<raspberry pi IP address>
    ```
4. Inside the raspberry pi, unpack the tar.bz2 and copy it to /usr
    ```console
    pi@raspberrypi:~$ cd /usr
    pi@raspberrypi:~$ sudo tar -xjvf rpi4-xeno3-deploy.tar.bz2 -C /
5. Create a config file and add the xenomai libraries to it.
    ```console
    pi@raspberrypi:~$ sudo nano /etc/ld.so.conf.d/xenomai.conf
    ```
    And add:
    ```
    /usr/local/lib
    /usr/xenomai/lib
    ```
6. Load the config and reboot, then run some tests and you are done!
    ```console
    pi@raspberrypi:~$ sudo ldconfig
    pi@raspberrypi:~$ sudo reboot
    ```

    ```console
    pi@raspberrypi:~$ sudo /usr/xenomai/bin/xeno-test
    pi@raspberrypi:~$ sudo /usr/xenomai/bin/latency
    ```

## Setting up the xenomai-ROS2 library pipeline

1. TODO

