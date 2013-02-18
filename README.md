BinSlayer
=========

This program has been realized for my master thesis project: it allows accurate
comparison of binary executables.

Edit - 16/02/2013: I'm going go on the work on this project. My currently goals are:

- Upgrade to Dyninst8.0.
- Ease installation so that everybody can install BinSlayer easily.
- Add full support for Windows plateforms.
- Add full support for PE32 and PE64 file formats.
- Add support for Mac OS and probably for Mach-O file format.
- Improve code, documentation, ...

After having done that, I will publish the first public release of the tool.

=====================================================================================================================

Installation of BinSlayer on Linux 32 bits: *(edit 17/02/2013)*
-------------------------------------------
  
  For the time being I have only tested BinSlayer on Ubuntu 12.04 LTS. However, it should work fine theorically
on all 32 bits Linux palteforms. Later I will check this myself. Below are the steps to follow to make it work
on Ubuntu LInux 32 bits:

  - Clone the BinSlayer repository.
  - DyninstAPI 8.0 and BeaEngine dependancies are already provided, but if you need to recompile them, simply
go on their respective website and download the sources packages. Both are easy to compile.
  - You need to set up Qt in order to use BinSlayer. Then you have to specify the location of your Qt
installation in your current shell environment. For example:

    **export PATH=/usr/lib/lightdm/lightdm:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/opt/QtSDK/Desktop/Qt/4.8.1/gcc/bin/**

  - Boost is also required because of DyninstAPI. In the root directory of your BinSlayer installation, create a directory
called **boost** in the subdirectory **libs**. Then download a boost tarball and extract it in the **./libs/boost** directory
you have just created.
  - Then type in your command prompt: **cmake -D WITH_GUI=1 .**
  - Then type: **make**
  - Normally all should have been fine. Please note that I compile with the flag *-std=c++0x*. The binaries are located
in the 'output' folder. Note that you need to place the **libBinaryHelper.so** in the same directory from which you want to
run BinSlayer.
  - Later I will provide a shell script to do all these actions automatically for you so that you have only one
command to type in order to make BinSlayer work!

=====================================================================================================================

  A manual on how to use BinSlayer, what you can do with it, its limitations and what you cannot do with it will be
published with the first public release of the tool. I think this would be during the Summer. However I will update this
readme page frequently to let you know my progress on it.
  
    
    
