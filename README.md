# DRIVSTALLER #
Installs a driver as a service.

Generic standalone tool inspired by Windows Driver examples.


## Version ##
1.1.9  
Last changed: 26.04.2024


## REQUIREMENTS ##
- msbuild


## Compile ##
```bash
$devcmd> build.bat [/ds] [/r] [/d] [/dp] [/ep] [/b 32|64] [/pdb] [/rtl] [/pts <toolset>] [/h]
```
or
```bash
$devcmd> msbuild drivstaller.vcxproj [/p:Platform=x86|x64] [/p:Configuration=Debug|Release] [/p:RunTimeLib=Debug|Release] [/p:PDB=0|1] [/p:PlatformToolset=<v142|v143|WindowsApplicationForDrivers10.0>]
```

The PlatformToolset defaults to "v142", but may be changed with the `/pts` option.
"v142" is used for VS 2019 version, "v143" would be used in VS 2022, 
or you could also use "WindowsApplicationForDrivers10.0" with WDK10 installed.


## Usage ##

```bash
$ drivstaller [options] <driver>|<service> [options]
```

### Options ###
* /n Name of service for the /i option. 
     If not set, the name will be derived of the driver path.
     I.e. the driver base name without the .sys suffix.
* /i Install and start the driver defined by \<driver\> path.
* /u Uninstall and stop the driver defined by \<service\> name.
* /o Start the driver defined by \<service\> name.
* /x Stop the driver defined by \<service\> name.
* /c Check, if the \<service\> name already exists.
* /s Service start type. 
    * 0: Boot (started by the system loader) 
    * 1: System (started by the IoInitSystem)
    * 2: Auto (started automatically by the SCM)
    * 3: Demand (Default) (started by the SCM with a call to StartService, i.e. the /o parameter)
    * 4: Disabled
* /d A driver dependency. If more dependencies are needed, pass more /d options (<= 0x10) in the required order.
* /h Print help.

The /i, /u, /o, /x, /c options are exclusive.


## Run ##
### Examples ###
Install and start a driver:
```bash
$ drivstaller driver.sys /i /s 3
```
Install and start a driver with a custom service name:
```bash
$ drivstaller driver.sys /i /s 3 /n driverSvc
```

Remove and stop a driver:
```bash
$ drivstaller driver /u
```

Just start an already installed driver:
```bash
$ drivstaller driver.sys /o
```

Just stop an already installed driver:
```bash
$ drivstaller driver /x
```

Install with dependencies and custom service name
```bash
$ drivstaller driver.sys /i /n mydriver /d dependency1 /d dependency2
```

Check, if a driver service "driverSvc" for the "driver.sys" already exists
```bash
$ drivstaller /c /n driverSvc
```

Check, if a driver service "driver" for the "driver.sys" already exists
```bash
$ drivstaller /c driver.sys
$ drivstaller /c driver
$ drivstaller /c /n driver
```


### Runtime Errors ###
If a "VCRUNTIMExxx.dll not found Error" occurs on the target system, statically including runtime libraries is a solution.  
This is done by using the `/rtl` or `/p:RunTimeLib=Debug|Release` flag.


## COPYRIGHT, CREDITS & CONTACT ##
Published under [GNU GENERAL PUBLIC LICENSE](LICENSE).   

#### Author ####
- Henning Braun ([henning.braun@fkie.fraunhofer.de](mailto:henning.braun@fkie.fraunhofer.de)) 
