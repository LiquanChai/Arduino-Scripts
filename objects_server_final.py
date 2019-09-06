# pylint: disable=no-member
# changed logic - use sensor instead of buttons

from flask import Flask, jsonify, request
import sqlite3
import os

app = Flask(__name__)


@app.before_first_request
def before_first_request():
    if os.path.isfile("objects.db"): # check if the database already exists
        # if exists, delete it
        os.remove("objects.db")
        print("old database is being removed")
    # create a new database
    conn = sqlite3.connect("objects.db")
    c = conn.cursor()
    
    # object_id can be 0, 1 or 2; 
    # current_state can be either 'up' or 'down'; 
    c.execute('CREATE TABLE objects_states (object_id INTEGER, current_state TEXT)')
    # initialise the entries
    c.execute("INSERT INTO objects_states VALUES (0, 'up')")
    c.execute("INSERT INTO objects_states VALUES (1, 'up')")
    c.execute("INSERT INTO objects_states VALUES (2, 'up')")
    conn.commit()
    c.close()
    conn.close()
    print("database initialised successfully")
    
@app.route("/", methods =["GET", "POST"])
def check_behaviour():
    # read entry from the database
    current_states = ""
    
    with sqlite3.connect("objects.db") as conn:
        c = conn.cursor()
        c.execute("SELECT * FROM objects_states")
        current_states = dict(c.fetchall())
    
    # which object is checking?
    object_id = None
    if request.method == 'POST':
        print (request.is_json)
        content = request.get_json()
        object_id = content["id"] # is it an int?
    else:
        object_id = 0 # for get/test purpose, assume it is an int

    # if current one is up, check if any other devices are down
    if current_states[object_id] == "up":
        other_objects_states = [v for k, v in current_states.items() if k != object_id]
        if "down" in other_objects_states:
            return jsonify({"behaviour": "abnormal"}) # turn to abnormal
        else:
            return jsonify({"behaviour": "normal"}) # keep normal
    else:
        return jsonify({"behaviour": "keep"}) # object do nothing

@app.route("/update_state", methods =["POST"]) # up -> down; down -> up
def update_state():
    # which object is sending the post? what content shall we update?
    print (request.is_json)
    content = request.get_json()
    object_id = content["id"]
    object_state = content["state"]
    
    # update the state of the object
    with sqlite3.connect("objects.db") as conn:
        c = conn.cursor()
        c.execute("UPDATE objects_states SET current_state =? WHERE object_id =?", (object_state, object_id,))
        conn.commit() # save commit
        c.close()
        return ""

if __name__ == "__main__":   
    # run the app
    app.run(debug=True)


