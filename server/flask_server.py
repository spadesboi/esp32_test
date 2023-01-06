from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/data', methods=['POST']) 
def data():
    data = request.get_data()
    print(data)
    return data
if __name__ == "__main__":
    app.run(host='0.0.0.0', port= 8080)