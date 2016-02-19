#Scoring service for the SRE Capstone challenges.
#Run with the following parameters:
#SubmitFlag.py <username> <Challenge ID>

from pymongo import MongoClient
import sys
from datetime import datetime
from datetime import date

if len(sys.argv) != 4:
	print "Usage: %s <username> <Challenge ID> <difficulty>" % (sys.argv[0])
	sys.exit(2)
	
client = MongoClient('localhost', 27017)
db = client.sre

username = sys.argv[1]
challengeID = sys.argv[2]
points = 100 * int(sys.argv[3])
classID = 2

#Truncate to 16 characters
username = username[:16]

#whitelist of allowed characters
if username.strip('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890'):
	sys.exit(3)
if challengeID.strip('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890'):
	sys.exit(4)


query = {"username":username, "challengeID":challengeID, "classID":classID}
flag = {"$set":{"username":username, "challengeID":challengeID, "classID":classID, "points":points, "timestamp":datetime.now()}}
flags = db.flags
result = flags.update_one(query, flag, upsert=True)

if result.upserted_id:
	scores = db.scores
	score = scores.find_one({"username":username, "classID":classID})
	print score
	if score == None:
		scores.insert_one({"username":username, "classID":classID, "points":points, "timestamp":datetime.now()})
	else:
		score["points"] = score["points"] + points
		score["timestamp"] = datetime.now()
		scores.update_one({"username":username, "classID":classID}, {"$set":score})
	

sys.exit(0)