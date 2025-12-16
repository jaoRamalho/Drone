import cv2
import mediapipe as mp
import math
import serial, time

# --- serial ---
ser = serial.Serial('COM11', 9600)
ultimo_gesto = None
t_ultimo_envio = 0
DELAY = 0.5

mpHands = mp.solutions.hands
mpDrawing = mp.solutions.drawing_utils

hands = mpHands.Hands(
    max_num_hands=1,
    min_detection_confidence=0.7,
    min_tracking_confidence=0.7
)

tol = 0.05

def distanciaX(a, b): return abs(a.x - b.x)
def distanciaY(a, b): return abs(a.y - b.y)
def distanciaZ(a, b): return abs(a.z - b.z)
def distanciaRelativa(a, b):
    return math.sqrt(distanciaX(a, b)**2 + distanciaY(a, b)**2 + distanciaZ(a, b)**2)

def escalaMao(handLandmarks):
    l = handLandmarks.landmark
    return distanciaRelativa(l[0], l[9]) + 1e-9

def tolEscalada(handLandmarks, fator=1.0):
    s = escalaMao(handLandmarks)
    return tol * (0.5 + 3.0*s) * fator

def angulo_com_vertical(dx, dy):
    return math.degrees(math.atan2(abs(dx), max(1e-9, -dy)))

def dedoDobrado(handLandmarks, tipIdx, baseIdx):
    tip  = handLandmarks.landmark[tipIdx]
    base = handLandmarks.landmark[baseIdx]
    m    = tolEscalada(handLandmarks, 1.0)
    return (distanciaRelativa(tip, base) < m) or (tip.y > base.y - m*0.5)

def dedoEstendidoY(handLandmarks, tipIdx, baseIdx):
    tip  = handLandmarks.landmark[tipIdx]
    base = handLandmarks.landmark[baseIdx]
    m    = tolEscalada(handLandmarks, 1.0)
    return (base.y - tip.y) > m

def polegarEstendidoParaCima(handLandmarks):
    tip = handLandmarks.landmark[4]
    ip  = handLandmarks.landmark[3]
    mcp = handLandmarks.landmark[2]
    min_len = tolEscalada(handLandmarks, 2.2)
    long_enough = (distanciaRelativa(tip, ip) > min_len) or (distanciaRelativa(tip, mcp) > 1.2*min_len)
    dx, dy = (tip.x - ip.x), (tip.y - ip.y)
    ang = angulo_com_vertical(dx, dy)
    verticalish = ang <= 50
    margin = tolEscalada(handLandmarks, 0.8)
    high_enough = tip.y < min(ip.y, mcp.y) - margin
    return long_enough and verticalish and high_enough

def polegarEstendidoParaBaixo(handLandmarks):
    l   = handLandmarks.landmark
    tip = l[4]
    ip  = l[3]
    mcp = l[2]
    idx_mcp = l[5]
    min_len = tolEscalada(handLandmarks, 1.8)
    long_enough = (distanciaRelativa(tip, ip)  > 0.9*min_len) or (distanciaRelativa(tip, mcp) > min_len)
    dx, dy = (tip.x - ip.x), (tip.y - ip.y)
    ang = angulo_com_vertical(dx, dy)
    verticalish_down = ang >= 120 
    m_y1 = tolEscalada(handLandmarks, 0.5)
    m_y2 = tolEscalada(handLandmarks, 0.3)
    below_joint = (tip.y > max(ip.y, mcp.y) + m_y1)
    below_index = (tip.y > idx_mcp.y + m_y2)
    return long_enough and (verticalish_down or (below_joint and below_index))

def detectarJoinha(handLandmarks):
    return (polegarEstendidoParaCima(handLandmarks) and
            dedoDobrado(handLandmarks, 8,5) and
            dedoDobrado(handLandmarks,12,9) and
            dedoDobrado(handLandmarks,16,13) and
            dedoDobrado(handLandmarks,20,17))

def detectarJoinhaInvertido(handLandmarks):
    return (polegarEstendidoParaBaixo(handLandmarks) and
            dedoDobrado(handLandmarks,8,5) and
            dedoDobrado(handLandmarks,12,9) and
            dedoDobrado(handLandmarks,16,13) and
            dedoDobrado(handLandmarks,20,17))

def detectarPaz(handLandmarks):
    m_sep = tolEscalada(handLandmarks, 1.5)
    a = handLandmarks.landmark[8]
    b = handLandmarks.landmark[12]
    separado = distanciaRelativa(a, b) > m_sep
    return (dedoEstendidoY(handLandmarks,8,5) and
            dedoEstendidoY(handLandmarks,12,9) and
            separado and
            dedoDobrado(handLandmarks,16,13) and
            dedoDobrado(handLandmarks,20,17))

def detectarMaoAberta(handLandmarks):
    return (dedoEstendidoY(handLandmarks,8,5) and
            dedoEstendidoY(handLandmarks,12,9) and
            dedoEstendidoY(handLandmarks,16,13) and
            dedoEstendidoY(handLandmarks,20,17) and
            not polegarEstendidoParaCima(handLandmarks) and
            not polegarEstendidoParaBaixo(handLandmarks))

def detectarMaoFechada(handLandmarks):
    ttip = handLandmarks.landmark[4]
    imcp = handLandmarks.landmark[5]
    perto = distanciaRelativa(ttip, imcp) < tolEscalada(handLandmarks, 1.2)
    return (dedoDobrado(handLandmarks,8,5) and
            dedoDobrado(handLandmarks,12,9) and
            dedoDobrado(handLandmarks,16,13) and
            dedoDobrado(handLandmarks,20,17) and
            perto)

cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    rgbFrame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgbFrame)

    gesto_id = None

    if results.multi_hand_landmarks:
        for handLandmarks in results.multi_hand_landmarks:
            mpDrawing.draw_landmarks(frame, handLandmarks, mpHands.HAND_CONNECTIONS)

            if detectarJoinha(handLandmarks):
                gesto_id = 1
                cv2.putText(frame,"JOINHA",(50,50),cv2.FONT_HERSHEY_SIMPLEX,1,(0,255,0),2)
            elif detectarJoinhaInvertido(handLandmarks):
                gesto_id = 2
                cv2.putText(frame,"JOINHA_INV",(50,50),cv2.FONT_HERSHEY_SIMPLEX,1,(0,255,255),2)
            elif detectarPaz(handLandmarks):
                gesto_id = 3
                cv2.putText(frame,"PAZ",(50,50),cv2.FONT_HERSHEY_SIMPLEX,1,(255,0,0),2)
            elif detectarMaoAberta(handLandmarks):
                gesto_id = 4
                cv2.putText(frame,"MAO_ABERTA",(50,50),cv2.FONT_HERSHEY_SIMPLEX,1,(0,200,255),2)
            elif detectarMaoFechada(handLandmarks):
                gesto_id = 5
                cv2.putText(frame,"MAO_FECHADA",(50,50),cv2.FONT_HERSHEY_SIMPLEX,1,(255,0,255),2)

    # ---- envio controlado ----
    if gesto_id is not None:
        now = time.time()
        if gesto_id != ultimo_gesto and (now - t_ultimo_envio) >= DELAY:
            ser.write(bytes([gesto_id]))
            ultimo_gesto = gesto_id
            t_ultimo_envio = now

    cv2.imshow("DeteccaoDeGestos1Mao", frame)
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
