#!/usr/bin/env node

var child_process = require('child_process');
var fs = require('fs');
var posixy;

try {
	posixy = require('posixy');
} catch (exception) {
	posixy = require('./build/Release/posixy');
}

// FIXME: turn this shell call into JavaScript code
child_process.exec("mktemp -d posixy.XXXXXX", function (error, stdout, stderr) {
	if (error) {
		throw error;
	}

	var tmpdir = stdout.match(/^([A-Za-z0-9\.]+)\n$/);
	if (!tmpdir) {
		throw new Error('ooops! ' + stdout);
	}
	tmpdir = tmpdir[1];

	// FIXME: turn this shell call into JavaScript code
	child_process.exec("touch " + tmpdir + "/sliff", function (error, stdout, stderr) {
		if (error) {
			fs.rmdirSync(tmpdir);
			throw error;
		}

		posixy.link(tmpdir + "/sliff", tmpdir + "/sloff", function (error) {
			fs.unlinkSync(tmpdir + "/sliff");
			fs.unlinkSync(tmpdir + "/sloff");
			fs.rmdirSync(tmpdir);

			if (error) {
				throw error;
			} else {
				console.log('test passed.');
			}
		});
	});
});
