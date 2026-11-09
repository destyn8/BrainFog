`import paho.mqtt.client as mqtt
import json
import ssl 
broker = "a14qnyirupq00n-ats.iot.ap-south-1.amazonaws.com"
port = 8883
topic  = "eeg/input"
rootCert = "AmazonRootCA1.pem"
deviceCert = "device-cert.crt"
privateKey = "private.pem.key"
client = mqtt.Client(client_id="EEG_LODA")
client.tls_set(rootCert,certfile=deviceCert,keyfile=privateKey,cert_reqs=ssl.CERT_REQUIRED,tls_version=ssl.PROTOCOL_TLSv1_2)
def callback(client,userdata,message):
    payload = json.loads(message.payload.decode("utf-8"))
    integer = payload["eeg_value"]
    print(integer)
client.on_message = callback
client.connect(broker,port=port)
client.subscribe(topic) 
client.loop_forever()

