from flask import Flask, Response, request
import os
import json
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

uploads_dir = './uploads'
os.makedirs('uploads', exist_ok=True)

@app.route('/ping', methods=['GET'])
def default():
    # http://192.168.0.113:5000
    res = Response({'status': 'Ok'})
    res.status_code = 200
    return res

@app.route('/upload', methods=['POST'])
def add_user():
    print('got something')
    
    try:
        file_ = request.files['']
        file_.save(os.path.join(uploads_dir,'rec.wav'))
        body = json.dumps({'ok': True, 'msg': 'File successfuly saved.'})
        res = Response(body)
        res.status_code = 200
    except Exception as e:
        body = json.dumps({'ok': False, 'msg': 'File was not saved', 'error': f'{e}'})
        res = Response(body)
        res.status_code = 500
    return res
    


if __name__ == '__main__':
    app.run()