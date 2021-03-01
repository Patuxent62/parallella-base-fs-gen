---------------------------------
# Patuxent-Parallella 2021.1 beta
---------------------------------

This repo supports patuxent-parallella image construction from basic components.
git@github.com:Patuxent62/parallella-base-fs-gen.git

## Notes

- Note: This is currently only for 7010 HDMI Desktop with mods for GPIO from Epiphany Cores
- Note: BrownDeer COPRTHR is broken and is not included in this image
- Note: Unlike the goal of the Parallella 2019.1 repo, this is currently not fully automated.
- Note: Qemu is used to build the armhf cross-platform filesystem in a chroot.
        Use of parallella hardware to complete the image construction is not required.


## Required gparted Partitions

(assumes a minimum 16 GB SD card, although an 8 GB card will do)

| Partition | File System | Mount Point | Label | Size | Flags |
| :-------------: | :-------: | :------- | :------: | :-----------: | :------: |
| /dev/mmcblk0p1 | fat16 | /boot | BOOT | 100.0 MiB | boot |
| /dev/mmcblk0p2 | ext4 | / |   | 14.5 GiB |   |


## Base Linux OS filesystem component source

- Ubuntu 18.04.5 from:

http://cdimage.ubuntu.com/ubuntu-base/releases/18.04.5/release/ubuntu-base-18.04.5-base-armhf.tar.gz

- ubuntu-base-18.04.5-base-armhf.tar.gz


## Image Components Summary:

- Image components in this repo are custom patuxent-parallella versions listed below:

| Component | Source Repo | Comments |
| :--------------------------------- | :--------------------------------------------- | :-------------------------------------- |
| ADI Kernel Image | git@github.com:Patuxent62/parallella-linux.git | Based on ADI 4.19.0 with modified ephiphany driver and xilinx support |
| devicetree.dtb (devicetree-2021-01.dtb/dts) | git@github.com:Patuxent62/parallella-linux.git | Updated to fix HDMI video, sound, and misc boot issues |
| parallella.bit.bin (parallella_e16_hdmi_egpio_7010.bit.bin) | git@github.com:Patuxent62/oh.git | Modified FPGA design for latest ADI HDMI IP and added support for GPIO accessibility from epiphany cores via AXI |
| epiphany sdk (/opt/adapteva/ filesystem branch) | git@github.com:Patuxent62/epiphany-sdk.git | Added extra debugging output to the SDK and toolchain |


--------------------------------------------------
# Patuxent-Parallella Ubuntu Image Creation (2021)
--------------------------------------------------

## Prequisite:

- Configure an Ubuntu 18.04.05 LTS system for patuxent-parallella Dev (amd64 system) according to:

ubuntu-devbox-parallella-2021-01.txt


## Instructions/Script for this Repo

- For image build instructions, see the file:

build-manual.txt

