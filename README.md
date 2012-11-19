# Posixy Extensions for Node.js

[![Build Status](https://secure.travis-ci.org/herzi/posixy.png)](http://travis-ci.org/herzi/posixy)

This package will provide you with some utilities that you might already know from Node.js, but which behave a bit more reliable.

## Installation

    npm install posixy

## API reference

### posixy.link(linkTarget, linkLocation, callback)

* linkTarget: the new link should point to that file
* linkLocation: the location of the new link to create
* callback: a function which will be called after creating the link. The function will be invoked with one argument: null or an error.

Create a hard link at linkLocation which points to the file at linkTarget. Unlike [fs.link()](http://nodejs.org/api/fs.html#fs_fs_link_srcpath_dstpath_callback), posixy.link() will fail for an already existing linkLocation.
