var idlerun = require('../index'),
	timeout = 5*1000,
	idlerunner = new idlerun(timeout).Emitter();

idlerunner.on('idle', function(a) {
	console.error('User Idle for: %d', a);
});
