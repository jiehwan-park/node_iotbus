// i2c.js
var i2c = require('i2c').create();

var bus = 7;	// ARTIK5: 7, ARTIK10: 9
var addr = 0x23;
var reg = 0x10;
var buf = new Array(10);
var cnt = 0;

var hnd = i2c.init(bus);
var ret = i2c.set_address(hnd, addr);

// write Byte
buf[0] = reg;
ret = i2c.write(hnd, buf, 1);

var refreshIntervalID = setInterval(function(){
	// read Byte
	ret = i2c.read(hnd, buf, 2);
	var val = ((buf[0]<<8)|buf[1])/1.2;
	console.log("val = %d\n", val);
	
	cnt++;
	if(cnt == 15){
		clearInterval(refreshIntervalID);
		console.log("stop i2c module!!");
		ret = i2c.stop(hnd);
	}
}, 1000);
