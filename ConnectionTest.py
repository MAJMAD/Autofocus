from pipython.pidevice.interfaces.pisocket import PISocket
from pipython.pidevice.gcsmessages import GCSMessages
from pipython.pidevice.gcscommands import GCSCommands




#for i in range(1, 5000):
#    print("working" + str(i))
gateway = PISocket('169.254.7.154', 50000)
messages = GCSMessages(gateway)
gcs = GCSCommands(gcsmessage = messages)

print(gcs.qIDN())

gateway.close()