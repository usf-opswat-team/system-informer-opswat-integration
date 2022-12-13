[![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=for-the-badge&color=blue)](https://opensource.org/licenses/MIT)
## Integration of MetaDefender Cloud into System Informer
A capstone Computer Science and Engineering project by students at the University of South Florida, Fall 2022.
This integration allows you to use MetaDefender Cloud features such as single process scanning, continuous scanning off all processes, and the ability to see threat reports for running process displayed via System Informer.

## Contributors
- Jaylen Brown
    - [Github](https://github.com/JMarshalB)
    - [LinkedIn](https://www.linkedin.com/in/jaylen-brown-6a70511a3/) 
- Christopher Greenland
    - [Github](https://github.com/cgreenland)
- Eric Kemmer
    - [GitHub](https://github.com/Airick73)
    - [LinkedIn](https://www.linkedin.com/in/erickemmer/)
- Chuong Le
    - [GitHub](https://github.com/chuongle1994)
    - [LinkedIn](https://www.linkedin.com/in/chuong-le-aab0a217b/)
- Sebastian Rivera
    - [GitHub](https://github.com/01sebar)
    - [LinkedIn](https://www.linkedin.com/in/sebastianriv/)

https://metadefender.opswat.com/

## Setting up MetaDefender Cloud
    1. Register for an API key at https://metadefender.opswat.com/
    1. Right click a process 
    2. Select send to -> MetaDefender
    3. You will be promoted to input your API key
    4. Input your API key and select "OK" 
    5. System Informer can now perform MetaDefender Cloud features

## MetaDefender Cloud features
    1. Single process scanning
        a. Right click a process
        b. Select send to -> MetaDefender
    2. Threat report 
        a. Right click a process
        b. Select send to -> View MetaDefender report 
    3. All process scanning
        a. Navigate to the Tools menu option 
        b. Select online checks -> Enable MetaDefender Cloud Scanning
        c. This will prompt you to restart System Informer
        d. Upon restart MetaDefender Cloud will scan the process 
           displayed in batches

<img align="left" src="SystemInformer/resources/systeminformer.png" width="128" height="128"> 

## System Informer

A free, powerful, multi-purpose tool that helps you monitor system resources, debug software and detect malware. Brought to you by Winsider Seminars & Solutions, Inc.

[Project Website](https://systeminformer.sourceforge.io/) - [Project Downloads](https://systeminformer.sourceforge.io/downloads.php)

## System requirements

Windows 7 or higher, 32-bit or 64-bit.

## Features

* A detailed overview of system activity with highlighting.
* Graphs and statistics allow you quickly to track down resource hogs and runaway processes.
* Can't edit or delete a file? Discover which processes are using that file.
* See what programs have active network connections, and close them if necessary.
* Get real-time information on disk access.
* View detailed stack traces with kernel-mode, WOW64 and .NET support.
* Go beyond services.msc: create, edit and control services.
* Small, portable and no installation required.
* 100% [Free Software](https://www.gnu.org/philosophy/free-sw.en.html) ([MIT](https://opensource.org/licenses/MIT))


## Building the project

Requires Visual Studio (2022 or later).

Execute `build_release.cmd` located in the `build` directory to compile the project or load the `SystemInformer.sln` and `Plugins.sln` solutions if you prefer building the project using Visual Studio.

You can download the free [Visual Studio Community Edition](https://www.visualstudio.com/vs/community/) to build the System Informer source code.

See the [build readme](./build/README.md) for more information or if you're having trouble building.

## Enhancements/Bugs


Please use the [GitHub issue tracker](https://github.com/winsiderss/systeminformer/issues)
for reporting problems or suggesting new features.


## Settings

If you are running System Informer from a USB drive, you may want to
save System Informer's settings there as well. To do this, create a
blank file named "SystemInformer.exe.settings.xml" in the same
directory as SystemInformer.exe. You can do this using Windows Explorer:

1. Make sure "Hide extensions for known file types" is unticked in
   Tools > Folder options > View.
2. Right-click in the folder and choose New > Text Document.
3. Rename the file to SystemInformer.exe.settings.xml (delete the ".txt"
   extension).

## Plugins

Plugins can be configured from Options > Plugins.

If you experience any crashes involving plugins, make sure they
are up to date.

Disk and Network information provided by the ExtendedTools plugin is
only available when running System Informer with administrative
rights.
