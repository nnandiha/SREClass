var express = require('express');
var router = express.Router();
var mongoose = require('mongoose');

var Flag = mongoose.model('Flag');

var classID = 1;

router.get('/', function(req, res, next){
  Flag.find({'classID':classID}).sort({timestamp:-1}).limit(3).exec(function(err, flags){
    if(err){
      return next(err);
    }
    res.json(flags);
  });
});

module.exports = router;
