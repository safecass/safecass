// List of DB commands
// http://www.mongodb.org/display/DOCS/List+of+Database+Commands

db = db.getMongo().getDB("safety_framework");
db.createCollection("monitor");
