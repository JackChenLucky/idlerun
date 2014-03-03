var idlerun = require('../index'),
	timeout = 5*1000,
	idlerunner,
	idleEvent;

console.log('Start');

setTimeout(function(){
	console.log('After: ', 1000);
}, 1000);

setTimeout(function(){
	console.log('After: ', timeout*2);
}, timeout*2);

setTimeout(function(){
	console.log('After: ', timeout*3);
}, timeout*3);

idlerunner = idlerun(timeout, function(a) {
	console.error('User Idle for: %d', a);
});
/*idleEvent = idlerunner.Emitter();

idleEvent.on('idle', function(a) {
	console.error('User Idle for: %d', a);
});*/

setTimeout(function(){
	console.log('After: ', timeout*4);
}, timeout*4);
