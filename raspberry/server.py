import numpy as np
from flask import Flask, request, jsonify, Response
from time import sleep
from flask_cors import CORS
import cv2
import ctypes
from ctypes import cdll, c_void_p, c_int
import threading
from edge_impulse_linux.image import ImageImpulseRunner
runner = ImageImpulseRunner('./model.eim')
model_info = runner.init()
print('Loaded runner for "' + model_info['project']['owner'] + ' / ' + model_info['project']['name'] + '"')
labels = model_info['model_parameters']['labels']

lib = cdll.LoadLibrary("./src/libwalle.so")


class Image(ctypes.Structure):
    _fields_ = [("addr", ctypes.POINTER(ctypes.c_ubyte)),
                ("width", ctypes.c_int),
                ("height", ctypes.c_int)]

class Color(ctypes.Structure):
    _fields_ = [("b", ctypes.c_ubyte),
                ("g", ctypes.c_ubyte),
                ("r", ctypes.c_ubyte)]

lib.sendData.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int]
lib.setup()

camera = cv2.VideoCapture(0)
camera.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
camera.set(cv2.CAP_PROP_FPS, 30)

app = Flask(__name__)
CORS(app)

state = (c_int * 3)()
connected = False
connect_time = 0

movement = [
    {'id': 0, 'label': 'Head'},
    {'id': 1, 'label': 'Neck'},
    {'id': 2, 'label': 'Left Eye'},
    {'id': 3, 'label': 'Right Eye'},
    {'id': 4, 'label': 'Left Track'},
    {'id': 5, 'label': 'Left Arm'},
    {'id': 6, 'label': 'Right Track'},
    {'id': 7, 'label': 'Right Arm'},
    {'id': 8, 'label': 'Bottom Neck'},
    {'id': 9, 'label': 'Tracks'},
    {'id': 10, 'label': 'Stop Tracks'}
    ]

    
def map_range(v, a, b, c, d):
       return (v-a) / (b-a) * (d-c) + c

def control_movement(angle, key, distance):
    if distance == 0:
        lib.updateServo(key, angle)
    else:
        lib.update(key, angle)
    


def gen_frames():
    while True:
        success, frame = camera.read()
        #frame = cv2.resize(frame, (320, 240))
        if not success:
            break
        else:
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            features, cropped = runner.get_features_from_image(frame)
            res = runner.classify(features)
            c = 5
            if "classification" in res["result"].keys():
                    print('Result (%d ms.) ' % (res['timing']['dsp'] + res['timing']['classification']), end='')
                    for label in labels:
                        score = res['result']['classification'][label]
                        print('%s: %.2f\t' % (label, score), end='')
                    print('', flush=True)
            
            elif "bounding_boxes" in res["result"].keys():
                for bb in res["result"]["bounding_boxes"]:
                    frame = cv2.rectangle(frame, (bb['x']*(c+3), bb['y']*c), (bb['x']*(c+3) + bb['width']*c, bb['y']*c + bb['height']*c), (255, 0, 0), 1)

            frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
            ret, buffer = cv2.imencode('.jpg', frame)
            new_frame = buffer.tobytes()
            yield (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n\r\n' + new_frame + b'\r\n')

#while True:
    #if not connected:
    #lib.walle()
    #else:
    #if time.time() - connect_time > 30:
def walle():
    lib.walle()


@app.route('/video', methods=['GET'])
def get_video():
    #global connected
    #if connected:
    return Response(gen_frames(),
            mimetype='multipart/x-mixed-replace; boundary=frame')
    #else:
        #return "Not connected", 204

@app.route('/control', methods=['POST'])
def get_values():
    #global connected
    #if connected:
    data = request.json
    angle = int(data['angle'])
    key = data['servo']
    dist = data['distance']
    control_movement(angle, key, dist)
    return 'Moved servo'

@app.route('/check')
def check_connect():
    #global connect_time
    #connect_time = time.time()
    #connected = True
    return jsonify(success=True)

@app.route('/options')
def get_options():
    return jsonify(movement)

threading.Thread(target=walle).start()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
