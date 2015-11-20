var express = require('express');
var router = express.Router();
var mongoose = require('mongoose');

var Score = mongoose.model('Score');

router.get('/:limit', function(req, res, next){
  var limit = Number(req.params.limit);
  Score.find().sort([['points',-1], ['timestamp',1]]).limit(limit).exec(function(err, scores){
    if(err){
      return next(err);
    }
    res.json(scores);
  });
});

module.exports = router;
