node_iotbus : node addon for capi-iotbus
============================

To build node_iotbus properly, the iotbus source code [iotbus](https://github.com/tizen-artik/iotbus.git) have been built previously.

## Build the node_iotbus source code

```bash
$ git clone https://github.com/tizen-artik/node_iotbus.git
$ git clone https://github.com/tizen-artik/iotbus.git
$ gbs build -A armv7l --include-all
```

## Install rpm files to target device

```bash
root:~> rpm -Uvh --nodeps --force /tmp/capi-iotbus-*.rpm
root:~> rpm -Uvh --nodeps --force /tmp/node-iotbus-*.rpm
root:~> sync
```

## Run node js (ex. i2c.js)
* Copy i2c.js file to target device using sdb push command.
* i2c.js is located in modules/i2c directory.owner:/usr/tmp> node i2c.js 
* Run i2c.js file in the target.

```bash
[Host]
XXX@XXX:~/node_iotbus$ sdb push modules/i2c/i2c.js /usr/tmp/

[Target]
owner:/usr/tmp> node i2c.js 
```
