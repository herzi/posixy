#!/usr/bin/env node

var child_process = require('child_process');
var fs = require('fs');
var posixy;

try {
	posixy = require('posixy');
} catch (exception) {
	posixy = require('./build/Release/posixy');
}

posixy.mktemp(function (error, result) {
    var tmpdir = result.path;

	if (error) {
		throw error;
	}

	fs.mkdir(tmpdir, 0755, function (error) {
		if (error) {
			throw error;
		}

        // FIXME: technically we need even more mktemp() calls here
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
});

/* vim:set sw=4 et: */
