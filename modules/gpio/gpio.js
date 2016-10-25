// gpio.js
var gpio = require('gpio').create();

var pin = 22;	/* [gpio pin number] artik5: 135, artik10: 22 */
var dir = 1;	/* [gpio direction ] output: 1, input: 0 */
var val = 1;	/* default value */
var cnt = 1;
var hnd;
var ret;

/* gpio open */
console.log("open gpio!!");
hnd = gpio.open(pin);

/* set direction */
console.log("set direction!!");
ret = gpio.set_direction(hnd, dir);

/* write value */
var refreshIntervalID = setInterval(function(){
	console.log("write value(%d)!!", val);
	ret = gpio.write(hnd, val);
	val ^= 1;
	cnt++;
	if(cnt == 16){
		clearInterval(refreshIntervalID);
		console.log("close gpio !!");
		ret = gpio.close(hnd);
	}
}, 1000);