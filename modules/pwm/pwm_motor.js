// pwm.js
var pwm = require('pwm').create();

var device = 0;
var channel = 0;
var period = 20000000;
var duty_cycle = 1500000;
var ret = 0;
var cnt = 0;
var degree = [0, 45, 90, 135, 180];

console.log("open pwm!!");
var hnd = pwm.open(device, channel);
var idx=0;
var refreshIntervalID = setInterval(function(){	
	if(idx < degree.length){
		switch(degree[idx]){
			case 0 :
				duty_cycle = 1000000;
				break;
			case 45 :
				duty_cycle = 1500000;			
				break;
			case 90 :
				duty_cycle = 2000000;
				break;
			case 135 :
				duty_cycle = 2500000;
				break;
			case 180 :
				duty_cycle = 2900000;
				break;
			default :
				duty_cycle = 2000000;
				break;
		}		
		console.log("set degree:" + degree[idx]);
		ret = pwm.set_period(hnd, period);
		ret = pwm.set_duty_cycle(hnd, duty_cycle);
		ret = pwm.set_enabled(hnd, 1);		/* 0: disable, 1: enable */
		idx++;
	}	
	if(idx == degree.length){
		cnt++;
		idx = 0;
	}
	if(cnt == 10){
		clearInterval(refreshIntervalID);
		console.log("close pwm test module!!");
		//ret = pwm.set_enabled(hnd, 0);		/* 0: disable, 1: enable */
		ret = pwm.close(hnd);	
	}
}, 1000);
