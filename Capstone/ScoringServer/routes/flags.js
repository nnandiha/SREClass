var express = require('express');
var router = express.Router();
var mongoose = require('mongoose');

var Flag = mongoose.model('Flag');

var classID = 1;

var allowedIPs = ['::ffff:127.0.0.1', '::ffff:10.54.32.149'];

router.get('/:limit', function(req, res, next){
	var limit = Number(req.params.limit);
	if(limit > 3 && allowedIPs.indexOf(req.connection.remoteAddress) < 0)
		limit = 3;
  Flag.find({'classID':classID}).sort({timestamp:-1}).limit(limit).exec(function(err, flags){
    if(err){
      return next(err);
    }
    res.json(flags);
  });
});

module.exports = router;
