from pipython.pidevice.interfaces.piserial import PISerial
from pipython.pidevice.gcsmessages import GCSMessages
from pipython.pidevice.gcscommands import GCSCommands

gateway = PISerial("/dev/ttyUSB0", 460800)
messages = GCSMessages(gateway)
pidevice = GCSCommands(gcsmessage = messages)

print(pidevice.qIDN())

gateway.close()
