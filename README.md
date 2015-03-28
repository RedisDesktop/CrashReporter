## Crash Reporter [![Build Status](https://travis-ci.org/RedisDesktop/CrashReporter.png?branch=master)](https://travis-ci.org/RedisDesktop/CrashReporter) [![Backlog](https://badge.waffle.io/RedisDesktop/CrashReporter.png)](https://waffle.io/redisdesktop/crashreporter)

![](http://glide.name/wp-content/uploads/2014/06/crashReporter.jpg)

### Usage

```
crashreporter dumpFileName [crashedApplicationPath]
```



### Supported API
Send Crash Report
```
POST /crash-report
```

#####Parameters
| Name | Type | Description |
|-------- |-------|---------------|
| product  | String | Crashed application name (`RedisDesktopManager`) |
| version  | String | Crashed application version (`0.7.7`) |
| platform  | String | OS name (Windows, Linux or OSX) |
| upload\_file\_minidump  | File | Google Breakpad minidump |

#####Response

If error occurred:

```
{"error" : "You use old version. Please download latest release <a href=\"http://redisdesktop.com/download\">0.8.0</a>"}
```

If minidump was accepted:

```
{"ok" : "Thank you!"}
```

###Server implementation example
https://github.com/RedisDesktop/redisdesktop.com/blob/master/app/controllers/crash-report.php

######Based on https://github.com/dschmidt/libcrashreporter-qt/
