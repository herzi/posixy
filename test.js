var posixy;
try {
	posixy = require('posixy');
} catch (exception) {
	posixy = require('./build/Release/posixy');
}

posixy.link("sliff", "sloff", function (error) {
	if (error) {
		throw error;
	}
});
