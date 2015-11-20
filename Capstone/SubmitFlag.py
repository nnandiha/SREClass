#Scoring service for the SRE Capstone challenges.
#Run with the following parameters:
#SubmitFlag.py <username> <Challenge ID>

from pymongo import MongoClient
import sys
from datetime import datetime
from datetime import date

if len(sys.argv) != 4:
	print "Usage: %s <username> <Challenge ID> <difficulty>" % (sys.argv[0])
	sys.exit(1)
	
client = MongoClient('localhost', 27017)
db = client.sre

username = sys.argv[1]
challengeID = sys.argv[2]
points = 100 * int(sys.argv[3])
classID = 1

#Do some validation. Whitelist would be better...
if '$' in username:
	sys.exit(1)
if '$' in challengeID:
	sys.exit(1)
  
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