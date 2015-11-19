#Scoring service for the SRE Capstone challenges.
#Run with the following parameters:
#SubmitFlag.py <username> <Challenge ID>

from pymongo import MongoClient
import sys
from datetime import datetime
from datetime import date

if len(sys.argv) != 3:
	print "Usage: %s <username> <Challenge ID>" % (sys.argv[0])
	sys.exit(1)
	
client = MongoClient('localhost', 27017)
db = client.sre

username = sys.argv[1]
challengeID = sys.argv[2]
classID = 1

#Do some validation. Whitelist would be better...
if '$' in username:
	sys.exit(1)
if '$' in challengeID:
	sys.exit(1)

query = {"username":username, "challengeID":challengeID, "classID":classID}
flag = {"username":username, "challengeID":challengeID, "classID":classID, "timestamp":datetime.now()}
flags = db.flags
flag_id = flags.update(query, flag, upsert=True)
sys.exit(0)