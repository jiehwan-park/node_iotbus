// pwm.js
var pwm = require('pwm').create();

var device = 0;
var channel = 0;
var default_period = 1 * 1000;
var default_duty_cycle = 1 * 1000 / 100;
var duty_cycle = 0;
var period = 0;
var test_idx = 0;
var ret = 0;
var cnt = 0;
var repeat = 0;

console.log("open pwm!!");
var hnd = pwm.open(device, channel);

ret = pwm.set_period(hnd, default_period);
ret = pwm.set_duty_cycle(hnd, default_duty_cycle);
ret = pwm.set_enabled(hnd, 1);		/* 0: disable, 1: enable */
console.log("set duty_cycle!!");

test_idx = default_period;
var refreshIntervalID = setInterval(function(){
	cnt++;
	if(cnt <= 10){		
		/* set duty cycle */
		ret = pwm.set_duty_cycle(hnd, test_idx);
		/* get period */
		period = pwm.get_period(hnd);
		/* get duty cycle */
		duty_cycle = pwm.get_duty_cycle(hnd);
		//console.log("[%d] period:%d/%d, duty_cycle:%d/%d", cnt, period, hnd.period, duty_cycle, hnd.duty_cycle);
		test_idx -= 50;
	}	
	if(cnt > 10){
		test_idx == 0;
		/* set duty cycle */
		ret = pwm.set_duty_cycle(hnd, test_idx);
		/* get period */
		period = pwm.get_period(hnd);
		/* get duty cycle */
		duty_cycle = pwm.get_duty_cycle(hnd);
		//console.log("[%d] period:%d/%d, duty_cycle:%d/%d", cnt, period, hnd.period, duty_cycle, hnd.duty_cycle);
		test_idx += 50;
	}
	if(cnt == 21){
		cnt = 0;
		test_idx = default_period;
		repeat += 1;
	}
	if(repeat == 3){
		clearInterval(refreshIntervalID);
		console.log("close pwm test module!!");
		ret = pwm.set_enabled(hnd, 0);		/* 0: disable, 1: enable */
		ret = pwm.close(hnd);	
	}
}, 300);
