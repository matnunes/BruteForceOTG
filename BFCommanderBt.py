from PyQt5 import QtCore, QtWidgets, QtSerialPort
from PyQt5.QtCore import Qt, QSize, Qt, QProcess
from PyQt5.QtWidgets import *
import time
import serial
import sys

try:
    import Queue
except:
    import queue as Queue
import sys, time, serial

# ser = serial.Serial(
#     port='/dev/rfcomm0',
#     baudrate=9600,
#     parity=serial.PARITY_ODD,
#     stopbits=serial.STOPBITS_TWO,
#     bytesize=serial.SEVENBITS
# )

## https://iosoft.blog/pyqt-serial-terminal-code/

SER_TIMEOUT = 0.1                   # Timeout for serial Rx
RETURN_CHAR = "\n"                  # Char to be sent when Enter key pressed
PASTE_CHAR  = "\x16"                # Ctrl code for clipboard paste
baudrate    = 9600                # Default baud rate
portname    = "/dev/rfcomm4"                # Default port name
hexmode     = False                 # Flag to enable hex display

# Convert a string to bytes
def str_bytes(s):
    return s.encode('latin-1')
     
# Convert bytes to string
def bytes_str(d):
    return d if type(d) is str else "".join([chr(b) for b in d])
     
# Return hexadecimal values of data
def hexdump(data):
    return " ".join(["%02X" % ord(b) for b in data])
 
# Return a string with high-bit chars replaced by hex values
def textdump(data):
    return "".join(["[%02X]" % ord(b) if b>'\x7e' else b for b in data])

# Subclass QMainWindow to customize your application's main window
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Brute-force Commander Bluetooth")
        self.recvText = QTextEdit(readOnly=True)

        layout = QVBoxLayout()
        self.listDict = QListWidget()

        btnTemp = QPushButton("Temp")
        btnTemp.setFixedSize(120,45)
        btnTemp.clicked.connect(self.btnTempClicked)

        btnConn = QPushButton("Connect")
        btnConn.setFixedSize(120,45)
        btnConn.clicked.connect(self.btnConnClicked)

        btnOpenDict = QPushButton("Open dictionary")
        btnOpenDict.setFixedSize(120,45)
        btnOpenDict.clicked.connect(self.btnOpenDictClicked)

        btnStartBF = QPushButton("Brute-force start")
        btnStartBF.setFixedSize(120,45)
        btnStartBF.clicked.connect(self.btnStartBFClicked)

        btnStopBF = QPushButton("Brute-force stop")
        btnStopBF.setFixedSize(120,45)
        btnStopBF.clicked.connect(self.btnStopBFClicked)

        btnExit = QPushButton("Exit")
        btnExit.clicked.connect(self.btnExitClicked)
        btnExit.setFixedSize(80,45)

        self.setFixedSize(QSize(350, 240))

        # Set the central widget of the Window.
        layout.addWidget(self.listDict)
        layout.addWidget(self.recvText)
        layout.addWidget(btnTemp)
        layout.addWidget(btnConn)
        layout.addWidget(btnOpenDict)
        layout.addWidget(btnStartBF)
        layout.addWidget(btnStopBF)
        layout.addWidget(btnExit)
        layout.setAlignment(Qt.AlignHCenter)

        widget = QWidget()
        widget.setLayout(layout)
        self.setCentralWidget(widget)

        self.serth = SerialThread('/dev/rfcomm5', 9600)   # Start serial thread
        self.serth.start()

        self.serial = QtSerialPort.QSerialPort(
            '/dev/rfcomm4',
            baudRate=QtSerialPort.QSerialPort.Baud9600,
            readyRead=self.receiveBtData
        )

    def btnOpenDictClicked(self):
        fname = QFileDialog.getOpenFileName(self, 'Open dictionary', '', 'Text files (*.txt)')
        if fname != ('', ''):
            print(fname[0])
            data = []
            with open(fname[0]) as f:
                for line in f:
                    data += line.split()
            for i in range(len(data)):
                self.listDict.insertItem(i+1, data[i])
    def btnTempClicked(self):
        self.recvText = ""
        self.serth.ser_out("P")
        self.serth.ser_out("\r\n")
    def btnConnClicked(self):
        if not self.serial.isOpen():
            self.serial.open(QtCore.QIODevice.ReadWrite)
    def btnStartBFClicked(self):
        print("Start brute-force")
        if self.serial.isOpen():
            self.serial.write("B".encode())
            time.sleep(2)
            self.serial.write("8087".encode())
            self.serial.write("\r\n".encode())
            print("8087\r\n")
            time.sleep(30.5)
            self.serial.write("B".encode())
            time.sleep(2)
            self.serial.write("8088".encode())
            self.serial.write("\r\n".encode())
            print("8088\r\n")
    def btnStopBFClicked(self):
        print("Stop brute-force")
        ser.close()
    def btnExitClicked(self):
        print("Exit")
        self.serial.close()
        window.close()

    @QtCore.pyqtSlot()
    def receiveBtData(self):
        while self.serial.canReadLine():
            inMsg = self.serial.readLine().data().decode()
            inMsg = inMsg.rstrip('\r\n')
            self.recvText.append(inMsg)
    @QtCore.pyqtSlot()
    def sendBtData(self):
        self.serial.write(self.message_le.text().encode())

    # Display incoming serial data
    def display(s):
        print(textdump(str(s)))

# Thread to handle incoming & outgoing serial data
class SerialThread(QtCore.QThread):
    def __init__(self, portname, baudrate): # Initialise with serial port details
        QtCore.QThread.__init__(self)
        self.portname, self.baudrate = portname, baudrate
        self.txq = Queue.Queue()
        self.running = True
 
    def ser_out(self, s):                   # Write outgoing data to serial port if open
        self.txq.put(s)                     # ..using a queue to sync with reader thread
         
    def ser_in(self, s):                    # Write incoming serial data to screen
        print(s)
        self.recvText.append(s)
         
    def run(self):                          # Run serial reader thread
        print("Opening %s at %u baud" % (self.portname, self.baudrate))
        try:
            self.ser = serial.Serial(self.portname, self.baudrate, timeout=SER_TIMEOUT)
            time.sleep(SER_TIMEOUT*1.2)
            self.ser.flushInput()
        except:
            self.ser = None
        if not self.ser:
            print("Can't open port")
            self.running = False
        while self.running:
            s = self.ser.readline(self.ser.in_waiting or 1)
            if s:                                       # Get data from serial port
                self.ser_in(bytes_str(s))               # ..and convert to string
            if not self.txq.empty():
                txd = str(self.txq.get())               # If Tx data in queue, write to serial port
                self.ser.write(str_bytes(txd))
        if self.ser:                                    # Close serial port when thread finished
            self.ser.close()
            self.ser = None

app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()
