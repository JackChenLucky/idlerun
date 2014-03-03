module.exports = function(timeout, idleCB){
	var idlerun = require('./build/Release/idlerun.node')(timeout, idleCB),
		events = require('events'),
		util = require('util');

	util.inherits(idlerun.Emitter, events.EventEmitter);
	return idlerun;
};
