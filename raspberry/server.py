import numpy as np
from flask import Flask, request, jsonify, Response
from time import sleep
from flask_cors import CORS
import cv2
import ctypes
from ctypes import cdll, c_void_p, c_int

lib = cdll.LoadLibrary("./src/libwalle.so")

class Image(ctypes.Structure):
    _fields_ = [("addr", ctypes.POINTER(ctypes.c_ubyte)),
                ("width", ctypes.c_int),
                ("height", ctypes.c_int)]

class Color(ctypes.Structure):
    _fields_ = [("b", ctypes.c_ubyte),
                ("g", ctypes.c_ubyte),
                ("r", ctypes.c_ubyte)]
#lib.modify_image.argtypes = (ctypes.POINTER(Image), ctypes.c_int, ctypes.c_int, Color)
#lib.run_impulse.argtypes = [ctypes.POINTER(Image), Color]
#lib.run_impulse.restype = ctypes.c_int32
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
    {'id': 9, 'label': 'Tracks'}
    ]


def control_movement(angle, key, distance):
    lib.sendData(angle, distance, key)

#def run_impulse(img, box_color):
    #lib.run_impulse.argtypes = [ctypes.POINTER(Image), Color]
    #lib.run_impulse.restype = ctypes.c_int32

    #ret = lib.run_impulse(ctypes.byref(img), box_color)
    #if ret != 0:
        #print(f"Error running impulse: {ret}")
        #return None

    #return ret

def gen_frames():
    while True:
        success, frame = camera.read()
        #frame = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        #frame = cv2.resize(frame, (320, 240))
        if not success:
            break
        else:
            #img = Image(frame.ctypes.data_as(ctypes.POINTER(ctypes.c_ubyte)), frame.shape[1], frame.shape[0])
    
            block_size = 10
            spacing = 15
            color = Color(205, 155, 235) # White color
            #lib.modify_image(ctypes.byref(img), block_size, spacing, color)
            #box_color = Color(0, 255, 0)  # Green
            #print(lib.run_impulse(ctypes.byref(img), color))
            #print(run_impulse(img, box_color))

            ret, buffer = cv2.imencode('.jpg', frame)

            new_frame = buffer.tobytes()
            yield (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n\r\n' + new_frame + b'\r\n')

#while True:
    #if not connected:
    #lib.walle()
    #else:
    #if time.time() - connect_time > 30:



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

#threading.Thread(target=walle).start()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
