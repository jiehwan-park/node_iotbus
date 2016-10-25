# node_iotbus
node addon for capi-iotbus

The iotbus source code have been built properly, before you build node_iotbus.
To build iotbus, please refer the below site.
- https://github.com/tizen-artik/iotbus.git


* Build the node_iotbus source code

$ git clone https://github.com/tizen-artik/node_iotbus.git
$ git clone https://github.com/tizen-artik/iotbus.git
$ gbs build -A armv7l --include-all


* Install rpm files to target device

root:~> rpm -Uvh --nodeps --force /tmp/capi-iotbus-*.rpm
root:~> rpm -Uvh --nodeps --force /tmp/node-iotbus-*.rpm
root:~> sync


* Run node js (ex. i2c.js)

# Copy i2c.js file to target device using sdb push command.
# i2c.js is located in modules/i2c directory.owner:/usr/tmp> node i2c.js 
[Host]
XXX@XXX:~/node_iotbus$ sdb push modules/i2c/i2c.js /usr/tmp/

# Run i2c.js file in the target.
[Target]
owner:/usr/tmp> node i2c.js 

