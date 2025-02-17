import numpy as np
from flask import Flask, request, jsonify, Response
from time import sleep
from flask_cors import CORS
from imutils.video import VideoStream
import imutils
import cv2
import ctypes
from ctypes import cdll, c_void_p, c_int
import threading
from edge_impulse_linux.image import ImageImpulseRunner
runner = ImageImpulseRunner('./model_hum.eim')
model_info = runner.init()
labels = model_info['model_parameters']['labels']
lib = cdll.LoadLibrary("./src/libwalle.so")


class image(ctypes.Structure):
    _fields_ = [("addr", ctypes.POINTER(ctypes.c_ubyte)),
                ("width", ctypes.c_int),
                ("height", ctypes.c_int)]
lib.process_Image.argtypes = [ctypes.POINTER(image)]
#lib.updateHeadXY.argtypes = [ctypes.c_int, ctypes.c_int]

#lib.setup()
cx = int(640/96)
cy = int(480/96)

app = Flask(__name__)
CORS(app)

class VideoCapture:

    def __init__(self):
        self.cap = cv2.VideoCapture(0)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        self.cap.set(cv2.CAP_PROP_FPS, 32)
        self.x = 0
        self.y = 0
        self.fomo = False
        (self.grabbed, self.frame) = self.cap.read()
        t = threading.Thread(target=self._reader)
        t.daemon = True
        t.start()

    def _reader(self):
        while True:
            (self.grabbed, self.frame) = self.cap.read()
            if self.fomo:
                x,y = self.get_XY()
            
                if x and y is not 0:
                    print(x,y)
                    #lib.setHeadxy(x, y)
                    self.x = 0
                    self.y = 0
                #walle()
    
    def read(self):
        if self.fomo:
            res = run_fomo(self.frame)
            self.x, self.y = fomo_coords(res, 0.95, self.frame)
        return self.frame

    def get_XY(self):
        return self.x, self.y

cap = VideoCapture()

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
        lib.update(key, angle, distance)

def walle():
    lib.walle()

def gen_frames():
    while True:
        frame = cap.read()
        #res = run_fomo(frame)
        #x, y = fomo_coords(res, 0.90, frame)
        #lib.updateHeadXY(x,y)
        #print(x, y)
        #frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        #grid_block(frame)
        #frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
        ret, buffer = cv2.imencode('.jpg', frame)
        new_frame = buffer.tobytes()            
        yield (b'--frame\r\n'b'Content-Type: image/jpeg\r\n\r\n' + new_frame + b'\r\n')

def run_fomo(frame):
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    features, cropped = runner.get_features_from_image(frame)
    res = runner.classify(features)
    return res

def fomo_coords(res, prob, frame):
    x = y = 0
    if res is not None:
        if "bounding_boxes" in res["result"].keys():
            bx = by = bw = bh = area = 0
            for bb in res["result"]["bounding_boxes"]:
                if bb['value'] > prob:
                    found_area = bb['width'] * bb['height']
                    if area < found_area:
                        area = found_area
                        bx = bb['x']*cx
                        by = bb['y']*cy
                        bw = bb['width']*cx
                        bh = bb['height']*cy
                        x = int(bx + (bw/2))
                        y = int(by + (bh/2))
            if frame is not None:
                if x and y is not 0:
                    frame = cv2.circle(frame,(x,y), 10, (250,0,100), -1)#
                    cv2.rectangle(frame, (bx, by), (bx + bw, by + bh), (235, 150, 55), 1)
    return x, y

def grid_block(frame):
    c_img = image(frame.ctypes.data_as(ctypes.POINTER(ctypes.c_ubyte)), frame.shape[1], frame.shape[0])
    lib.process_Image(ctypes.byref(c_img))   

@app.route('/video', methods=['GET'])
def get_video():
    return Response(gen_frames(),
            mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/control', methods=['POST'])
def get_values():
    data = request.json
    angle = int(data['angle'])
    key = data['servo']
    dist = data['distance']
    control_movement(angle, key, dist)
    return 'Moved servo'

@app.route('/check')
def check_connect():
    return jsonify(success=True)

@app.route('/options')
def get_options():
    return jsonify(movement)

threading.Thread(target=walle).start()


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)