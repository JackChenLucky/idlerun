module.exports = function(timeout){
	var idlerun = require('./build/Release/idlerun.node')(timeout),
		events = require('events'),
		util = require('util');

	util.inherits(idlerun.Emitter, events.EventEmitter);
	return idlerun;
};
