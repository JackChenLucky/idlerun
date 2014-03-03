idlerun
=======

Node.js Native Addon to report system Idle time based on Keyboard &amp; Mouse Input.

## Usage

Make sure node-gyp is installed globally: `npm install node-gyp -g`

Checkout this Repo and run `npm install`

Run `node-gyp rebuild` after making changes to C++ code.

Currently emitting events on the main node process.

``` javascript
var idlerun = require('idlerun');
var timeout = 15 * 60 * 1000;

process.on('idle', function(a) {
	console.error('User Idle');
});

process.on('active', function(a) {
	console.error('User Active');
});

idlerun(timeout, function(a) {
	console.error('User Idle for: %d', a);
});

```

See example in test/test.js. Run with `node test/test`
