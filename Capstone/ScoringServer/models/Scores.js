var mongoose = require('mongoose');

var ScoreSchema = new mongoose.Schema({
  username:String,
  classID:Number,
  points:Number,
  timestamp:Date
});

mongoose.model('Score', ScoreSchema);