import paho.mqtt.client as mqtt
import sys, time
import smtplib as s
import RPi.GPIO as GPIO

MensagemRecebida = "OK"
flag_sentar = 1
abriu = 0

#definições
def sensor_ultra():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setwarnings(False)

    PIN_TRIGGER = 7
    PIN_ECHO = 11

    GPIO.setup(PIN_TRIGGER, GPIO.OUT)
    GPIO.setup(PIN_ECHO, GPIO.IN)

    GPIO.output(PIN_TRIGGER, GPIO.LOW)

    #print (" Aguardando o sensor estabilizar")

    time.sleep(2)

    #print (" Cálculo de distância ")

    GPIO.output(PIN_TRIGGER, GPIO.HIGH)

    time.sleep(0.00001)

    GPIO.output(PIN_TRIGGER, GPIO.LOW)

    while GPIO.input(PIN_ECHO)==0:
        pulse_start_time = time.time()
    while GPIO.input(PIN_ECHO)==1:
        pulse_end_time = time.time()

    pulse_duration = pulse_end_time - pulse_start_time
    distancia = round(pulse_duration * 17150, 2)
    print ("Distancia:",distancia,"cm")
    
    return distancia

def on_connect(client, userdata, flags, rc):
    print("[STATUS] Conectado ao broker, resultado de conexão: "+str(rc))
    
    #faz subscribe automatico no topico
    client.subscribe("MQTTprojetopythonCarolRecebeeee")

    
def on_message(client, userdata, msg):
    global MensagemRecebida
    
    MensagemRecebida = str(msg.payload.decode("utf-8"))    
   
    print(MensagemRecebida)
    
def email():
   

    #Dados da mensagem
    remetente = "tavares.carolinna@hotmail.com"
    destinatario = "ingrid.caroline25@gmail.com" #Pegar de um arquivo
    corpo = ''' O idoso tomou o remedio em: {}, {}.'''.format(time.strftime('%A, %d %b %Y'),time.strftime('%H:%M:%S'))
 
    #Pegando a senha no arquivo txt
    se = open('senha.txt','r')
    senha = se.read()

    #Fazendo o objeto do servidor SMTP
    server = s.SMTP('smtp.outlook.com',587)
    server.ehlo()
    server.starttls()

    #Colocando as credenciais
    server.login(remetente,senha)

    #Enviando e-mail
    server.sendmail(remetente,destinatario,corpo)
    print('E-mail enviado com sucesso!')

  
    time.sleep(1)

    
#Configurando comunicação MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message    
client.connect("broker.mqtt-dashboard.com", 1883, 60)
client.loop_start()

while 1:
    #Ler arquivo
    #Salvar horário de tomar o remédio
    #Salvar nome e número dos parentes
    #Salvar nível de mobilidade
    
    if MensagemRecebida == "ABRIU":
        print("Entrou")
        #Pegar o horário naquele momento
        email()
        print(''' O idoso tomou o remédio em: {}, {}.'''.format(time.strftime('%A, %d %b %Y'),time.strftime('%H:%M:%S')))
        time.sleep(0.5)
        client.publish("MQTTprojetopythonCarolEnviaaaa","Conferido")
        MensagemRecebida = "OK"
        #Ver se os horários batem
        #Mandar mensagem avisando que tomou o remédio. (Enviar confirmado de volta)
        #Setar a flag de remédio tomado

    tempo = time.ctime()
    tempo = tempo.split()
    if tempo[3] > "18:40:00":
        client.publish("MQTTprojetopythonCarolEnviaaaa","Perto")

        #Compara o tempo que sentou com o tempo de uma hora
    dist = sensor_ultra()    
    if dist < 10 and flag_sentar:
        tempo2 = time.ctime()
        tempo2 = tempo2.split()
        flag_sentar = 0 
        if tempo2[3] > "18:42:00":
            flag_sentar = 1
            print("LEVANTA E SE MEXE")
        
    
    
   
