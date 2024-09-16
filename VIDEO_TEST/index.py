import cv2, threading
from time import sleep
import numpy as np

from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.preprocessing import image
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Flatten, Dense, Input, Dropout, BatchNormalization
from tensorflow.keras.metrics import Precision, Recall, CategoricalAccuracy
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.callbacks import ModelCheckpoint, EarlyStopping
from tensorflow.keras.models import load_model
from tensorflow.keras.preprocessing.image import img_to_array, load_img


currentFrame = None
currentState = ''
model = load_model('model.keras')

class_indices = {
    'green': 0,
    'red':   1,
    'yellow': 2
}


def getKeyFromValue(data, target):
    for key, value in data.items():
        if target == value:
            return key
    return None


def getTrafficSignal(frame):
    global model, class_indices
    
    frame = cv2.resize(frame, (200, 200))
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    imgArray = img_to_array(frame)                # Converter a imagem para um array
    imgArray = np.expand_dims(imgArray, axis=0)   # Adicionar uma dimensão extra para o batch
    imgArray /= 255.0                             # Normalizar os pixels para o intervalo [0, 1]
    
    matrix = model.predict(imgArray)
    prediction = np.argmax(matrix, axis=1)[0] 
    return getKeyFromValue(class_indices, prediction)


def handleDetection():
    global currentFrame, currentState

    if currentFrame is None:
        return
    
    currentState = getTrafficSignal(currentFrame)


def handleThread():
    print('starting thread')

    while True:
        handleDetection()
        sleep(0.1)
    

def startThread():
    thread = threading.Thread(target=handleThread)
    thread.daemon = True
    thread.start()


def main():
    global currentFrame, currentState

    startThread()
    video = cv2.VideoCapture('video.mp4')

    if not video.isOpened():
        return print('error reading video file')

    while True:
        ret, frame = video.read()

        if not ret or frame is None:
            break
        
        currentFrame = cv2.resize(frame, (800, 600))
        cv2.putText(currentFrame, currentState, (100, 100), cv2.FONT_HERSHEY_COMPLEX, 2, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.imshow('detection', currentFrame)

        if cv2.waitKey(25) & 0xFF == ord('q'):
            break

    video.release()
    cv2.destroyAllWindows()
    print('video ended')




if __name__ == '__main__':
    main()

