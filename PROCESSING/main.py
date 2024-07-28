import cv2
import threading

# Variável global para sinalizar quando parar a captura de vídeo
stop = False

# Função para desenhar no frame
def draw_on_frame(frame):
    # Aqui você pode adicionar qualquer desenho ou processamento que desejar
    cv2.putText(frame, 'OpenCV Multithreading Example', (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
    cv2.rectangle(frame, (100, 100), (200, 200), (255, 0, 0), 2)

# Função para capturar vídeo em tempo real
def capture_video():
    global stop
    cap = cv2.VideoCapture(0)
    
    if not cap.isOpened():
        print("Error: Could not open video stream.")
        return

    while not stop:
        ret, frame = cap.read()
        if not ret:
            break
        
        # Criar um thread para desenhar no frame
        draw_thread = threading.Thread(target=draw_on_frame, args=(frame,))
        draw_thread.start()
        draw_thread.join()  # Esperar até que o desenho esteja completo
        
        cv2.imshow('Video', frame)
        
        # Parar a captura de vídeo se a tecla 'q' for pressionada
        if cv2.waitKey(1) & 0xFF == ord('q'):
            stop = True
            break

    cap.release()
    cv2.destroyAllWindows()

# Iniciar a captura de vídeo
capture_video()
