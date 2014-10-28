Cinder-Awesomium
================

Cinder block for the Awesomium web browser library.


Included are the Windows 1.7.0.5 and MacOSX 1.7.0 RC3 distributions of Awesomium, excluding their ```samples``` and ```wrappers``` folders. Please refer to the [Awesomium license](https://raw.github.com/paulhoux/Cinder-Awesomium/master/LICENSE.txt) for more information on licensing fees. Awesomium is NOT free software!


*Important note*: this block was created as an exercise in creating Cinder blocks. I am not an avid user of Awesomium, nor affiliated, least of all an expert. The block is provided as is. However, I will accept pull request that add to, or update, the functionality of this block.


#####Adding this block to Cinder
This block is meant to be used with version 0.8.5 of Cinder.

Cinder's tool for setting up empty projects, TinderBox, has been revamped and now supports a neat system for the management of plug-ins called Cinder Blocks. This Awesomium block supports this new feature. To add this block to your copy of Cinder, so it will be detected automatically by TinderBox:
* Open a command window (or Terminal)
* Switch to the disk containing the Cinder root folder, e.g.: ```d:```
* Browse to the Cinder root folder: ```cd path\to\cinder_master```
* Add the Awesomium block to the blocks folder: ```git clone https://github.com/paulhoux/Cinder-Awesomium.git blocks/Awesomium```

Alternatively, you can download the repository as a [ZIP-file](https://github.com/paulhoux/Cinder-Awesomium/zipball/master) and manually add the files to a "cinder_master\blocks\Awesomium" folder.


#####Creating a sample application using Tinderbox
The provided sample will show you how to use Awesomium in your application. To create and compile the sample:
* Run Cinder's TinderBox
* Select "Awesomium: Basic" from the Template popup menu
* Choose a name for the project
* Choose a location for the project
* Choose your environment(s) of choice
* Press "Next"
* Press "Finish"


#####Copyright notice
Copyright (c) 2010-2013, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and	the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
