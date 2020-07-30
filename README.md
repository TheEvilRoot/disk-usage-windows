# DU

DU is disk usage utility for Windows written in C++. Inspired by unix's `du` utility.

# Usage

```bash

% du [-s] [-h] path [...other paths] 

```

Simply run `du.exe` from command line and specify files or directories to get its disk usage.

```bash

% du.exe C:/Users/TheEvilRoot/.gradle C:/Users/TheEviLRoot/Documents/

```

### Parameters

#### -s - silent

When parameter `-s` is specified in command line arguments, program will not print any log into stdout and stderr. 
There's only summary values will be printed (**without *Summary* title**)

#### -h - use suffixes

By default `du` prints file/directory sizes in bytes. If `-h` is specified, file sizes will be printed in user-frienly format, 
like `10.5K` or `1.3G`. 