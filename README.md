# DRIVSTALLER #
Installs a driver as a service using the .sys file.

Generic standalone tool inspired by Windows Driver examples.


## Version ##
1.1.0  
Last changed: 19.08.2020


## Compile ##
```batch
$devcmd> msbuild drivstaller.vcxproj [/p:Platform=x86|x64] [ /p:Configuration=Debug|Release] [/p:RunTimeLib=Debug|Release]
```

## Run ##
### Examples ###
Install and start a driver:
```batch
$ drivstaller a\driver.sys /i /s 3
```

Remove and stop a driver:
```batch
$ drivstaller a\driver.sys /u
```

Just start an already installed driver:
```batch
$ drivstaller a\driver.sys /o
```

Just stop an already installed driver:
```batch
$ drivstaller a\driver.sys /x
```
### Options ###
* /h Print help.
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

The /i, /u, /o, /x options are exclusive.


### Runtime Errors ###
If a "VCRUNTIMExxx.dll not found Error" occurs on the target system, statically including LIBCMT.lib is a solution.  
This is done by using the `/p:RunTimeLib=Debug|Release` flag.


## COPYRIGHT, CREDITS & CONTACT ##
Published under [GNU GENERAL PUBLIC LICENSE](LICENSE).   

#### Author ####
- Henning Braun ([henning.braun@fkie.fraunhofer.de](henning.braun@fkie.fraunhofer.de)) 
