from flask import Flask, Response, request
import os
from flask_cors import CORS

app = Flask(__name__)
CORS(app)


uploads_dir = './uploads'
os.makedirs('uploads', exist_ok=True)


@app.route('/ping', methods=['GET'])
def index():
    res = Response('Server is running...')
    res.status_code = 200
    return res

idx = 0
@app.route('/upload', methods=['POST'])
def add_user():
    print('File recieved')
    global idx
    idx += 1
    try:
        with open(os.path.join(uploads_dir,f'record_{idx}.wav'), 'wb') as f:
            f.write(request.data)
        res = Response()
        res.content_length = 200
        res.status_code = 200
    except Exception as e:
        idx -= 1
        res = Response()
        res.content_length = - 1
        res.status_code = 500
    return res
    

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8008)