# DRIVSTALLER #
Installs a driver as a service.

Generic standalone tool inspired by Windows Driver examples.


## Version ##
1.1.3  
Last changed: 29.09.2021


## REQUIREMENTS ##
- msbuild
- [wdk]

**Remarks**  
The .vcxproj file is using `WindowsApplicationForDrivers10.0` as the `PlatformToolset`, which leads to smaller builds. 
If the WDK is not installed, the `PlatformToolset` may be changed to `v142` and it should compile without errors.


## Compile ##
```bash
$devcmd> msbuild drivstaller.vcxproj [/p:Platform=x86|x64] [ /p:Configuration=Debug|Release] [/p:RunTimeLib=Debug|Release]
```

## Run ##
### Examples ###
Install and start a driver:
```bash
$ drivstaller a\driver.sys /i /s 3
```

Remove and stop a driver:
```bash
$ drivstaller a\driver.sys /u
```

Just start an already installed driver:
```bash
$ drivstaller a\driver.sys /o
```

Just stop an already installed driver:
```bash
$ drivstaller a\driver.sys /x
```
### Options ###
* /n Name of service. If not set, it will be derived of the driver path.
* /i Install and start the driver.
* /u Unistall and stop the driver.
* /o Start the driver.
* /x Stop the driver.
* /s Service start type. 
	* 0: Boot (started by the system loader) 
	* 1: System (started by the IoInitSystem)
	* 2: Auto (started automatically by the SCM)
	* 3: Demand (Default) (started by the SCM with a call to StartService, i.e. the /o parameter)
	* 4: Disabled
* /h Print help.

The /i, /u, /o, /x options are exclusive.


### Runtime Errors ###
If a "VCRUNTIMExxx.dll not found Error" occurs on the target system, statically including LIBCMT.lib is a solution.  
This is done by using the `/p:RunTimeLib=Debug|Release` flag.


## COPYRIGHT, CREDITS & CONTACT ##
Published under [GNU GENERAL PUBLIC LICENSE](LICENSE).   

#### Author ####
- Henning Braun ([henning.braun@fkie.fraunhofer.de](henning.braun@fkie.fraunhofer.de)) 
