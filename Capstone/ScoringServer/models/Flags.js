var mongoose = require('mongoose');

var FlagSchema = new mongoose.Schema({
  username:String,
  challengeID:String,
  classID:Number,
  timestamp:Date
});

mongoose.model('Flag', FlagSchema);