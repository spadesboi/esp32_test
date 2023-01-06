from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/data', methods=['POST']) 
def data():
    data = request.get_data()
    data=str(data)+'\n'
    print(data)
    with open(r"espData.txt", "a") as file:
        file.write(data)
    return data
if __name__ == "__main__":
    app.run(host='0.0.0.0', port= 8080)