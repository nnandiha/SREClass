#Scoring service for the SRE Capstone challenges.
#Run with the following parameters:
#SubmitFlag.py <username> <Challenge ID>

from pymongo import MongoClient
import sys
from datetime import datetime
from datetime import date

if len(sys.argv) != 3:
	print "Usage: %s <username> <Challenge ID>" % (sys.argv[0])
	exit(-1)
	
client = MongoClient('localhost', 27017)
db = client.sre

username = sys.argv[1]
challengeID = sys.argv[2]
classID = 1

#Do some validation. Whitelist would be better...
if '$' in username:
	exit(-1)
if '$' in challengeID:
	exit(-1)

flag = {"username":username, "challengeID":challengeID, "timestamp":datetime.now(), "classID":classID}
flags = db.flags
flag_id = flags.insert_one(flag).inserted_id
#print flag_id
exit(0)