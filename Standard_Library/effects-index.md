# Standard Effects Index (Informative)

```
Pure              // no effects
AllAlloc          = alloc.*
AllIO             = fs.* + net.* + io.write
SafeIO            = fs.read + fs.write + alloc.heap
NetworkIO         = net.* + alloc.heap
FileIO            = fs.read + fs.write + alloc.heap
ConsoleIO         = io.write
NoAlloc           = !alloc.*
NoIO              = !(fs.* + net.* + io.write)
Deterministic     = !(time.* + random)
GameTick          = alloc.temp + !alloc.heap + !alloc.region + NoIO
RealTime          = !alloc.heap + NoIO
WebService        = net.* + fs.read + fs.write + alloc.heap
DatabaseOps       = fs.* + alloc.heap + !net.*
```
