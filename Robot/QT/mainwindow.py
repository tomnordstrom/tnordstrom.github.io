# This Python file uses the following encoding: utf-8
import os
from pathlib import Path
import sys
import time
import subprocess

#os.environ["QT_IM_MODULE"] = "qtvirtualkeyboard"
from fs100 import FS100
from PySide6.QtWidgets import QApplication, QMainWindow, QDialog, QLabel, QGridLayout, QPushButton, QWidget, QStyleFactory,QLineEdit
from PySide6.QtCore import QThread, Qt, QFile, QObject, Signal, Slot
from PySide6.QtUiTools import QUiLoader
from functools import partial

from MyWidgets import MyQLineEdit

_robot = FS100('192.168.255.1')
shouldquit = 0
statusdict = dict()
var8069 = FS100.Variable(FS100.VarType.IO, 8069)

#var8069 = FS100.Variable(FS100.VarType.IO, 8069)

# Extend from QObject
blink = 1
gripperopen = 0
jobstarted = 0
#alarm example
#hold_externally': False, 'hold_by_cmd': False, 'alarming': True, 'error_occurring': False, 'servo_on': False}
#{'code': 6004, 'data': 1, 'type': 1, 'time': '2024/04/19 10:16', 'name': 'ESCAPE FROM CLAMPING ERROR(PFL)'}

#status {'step': False, 'one_cycle': True, 'auto_and_cont': False, 'running': False, 'guard_safe': False, 'teach': False, 'play': True, 'cmd_remote': False, 'hold_by_pendant': False, 'hold_externally': False, 'hold_by_cmd': False, 'alarming': False, 'error_occurring': False, 'servo_on': True}


class ReadStatus(QObject):
    finished = Signal()
    progress = Signal(int)

    def run(self):
        global _robot
        global shouldquit
        global statusdict
        global var8069
        """Long-running task."""
        while(shouldquit == 0):
            time.sleep(0.5)
            #print("reading")
            _robot.get_status(statusdict)
            #var8069 = FS100.Variable(FS100.VarType.IO, 8069)
            error = _robot.read_variable(var8069)
            #print(str(var8069.val))
            #print(statusdict)
            result = dict()
            _robot.read_alarm_info(4, result)
            #_robot.get_last_alarm(result)
            #print(result)
            self.progress.emit(1)
            if(shouldquit==1):
                print("dying")
                self.finished.emit()




class NumPadDialog(QDialog):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Virtual Numpad")
        self.label = QLabel("")
        self.label.setAlignment(Qt.AlignRight)
        self.label.setStyleSheet("font-size: 20px; border: 1px solid black; padding: 5px;")

        self.createNumPad()

        layout = QGridLayout()
        layout.addWidget(self.label, 0, 0, 1, 3)
        layout.addWidget(self.numPadWidget, 1, 0, 1, 3)  # Corrected line
        self.setLayout(layout)

    def createNumPad(self):
        self.numPadWidget = QWidget()
        self.numPad = QGridLayout(self.numPadWidget)
        buttons = [
            ("1", 0, 0), ("2", 0, 1), ("3", 0, 2),
            ("4", 1, 0), ("5", 1, 1), ("6", 1, 2),
            ("7", 2, 0), ("8", 2, 1), ("9", 2, 2),
            ("0", 3, 1), ("Clear", 3, 0), ("Enter", 3, 2)
        ]
        for text, row, col in buttons:
            button = QPushButton(text)
            button.clicked.connect(partial(self.buttonClicked, text))  # Using functools.partial
            self.numPad.addWidget(button, row, col)

    def buttonClicked(self, text):
        if text == "Clear":
            self.label.clear()
        elif text == "Enter":
            pass  # You can add functionality to handle "Enter" button click here
        else:
            self.label.setText(self.label.text() + text)

class MainWindow(QObject):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.load_ui()

        # Step 2: Create a QThread object
        self.thread = QThread()
        self.window.pushButton1.nummpad = NumPadDialog();
        # Step 3: Create a worker object
        self.readstatus = ReadStatus()
        self.readstatus.moveToThread(self.thread)
        # Step 5: Connect signals and slots
        self.thread.started.connect(self.readstatus.run)
        self.readstatus.finished.connect(self.thread.quit)
        self.readstatus.finished.connect(self.readstatus.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)
        self.readstatus.progress.connect(self.reportStatus)
                # Step 6: Start the thread
        self.thread.start()

        self.window.show()
    def reportStatus(self,arg):
        global jobstarted
        global blink
        if(statusdict["running"] == True):
            print("running")
        if(statusdict["running"] == False and jobstarted == 1):
            print("close gripper")
            returned_value = subprocess.call("/home/pi/machine/shmUtil -set 566 0", shell=True)
            time.sleep(0.2)
            self.startJob("PUTSTACK")
            time.sleep(0.5)
            jobstarted = 2
        if(statusdict["running"] == False and jobstarted == 2):
            print("open gripper")
            returned_value = subprocess.call("/home/pi/machine/shmUtil -set 566 1", shell=True)
            jobstarted = 0

        if(statusdict["play"] == True):
            self.window.label.setText("Play mode");
            self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; border-width: 3px; border-style: solid; border-radius: 8px; background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #fbfdfd, stop:0.5 green stop:1 #fbfdfd);");
        else:
            self.window.label.setText("Teach mode");
            self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: red; border-width: 3px; border-style: solid; border-radius: 8px;");
        if(var8069.val & 64 == 64):
            self.window.pushButtonForce.setStyleSheet("color: #FFFFFF; background-color: orange; border-radius: 16px");
            self.window.pushButtonForce.setText("Collision");
            if(blink):
                self.window.pushButtonForce.hide()
                blink = 0
            else:
                blink = 1
                self.window.pushButtonForce.show()
        else:
            self.window.pushButtonForce.setStyleSheet("color: #FFFFFF; background-color: green; border-radius: 16px");
            self.window.pushButtonForce.setText("Force \n monitor");


    def load_ui(self):
        loader = QUiLoader()
        loader.registerCustomWidget(MyQLineEdit)
        path = Path(__file__).resolve().parent / "form.ui"
        ui_file = QFile(path)
        ui_file.open(QFile.ReadOnly)

        # Load window manually
        self.window = loader.load(ui_file)
        ui_file.close()
        self.window.pushButton1.setText("Exit")
        self.window.pushButton1.clicked.connect(self.click)
        
        self.window.pushButton_2.setText("Run job")
        self.window.pushButton_2.clicked.connect(self.runjob)

        self.window.pushButton_3.setText("Left job")
        self.window.pushButton_3.clicked.connect(self.showKeyboard)

        self.window.pushButtonForce.setText("Force monitor")
        self.window.pushButtonForce.setStyleSheet("color: #FFFFFF; background-color: green; border-radius: 16px");

        self.window.label.setText("Not Ready");
        self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: green; border-width: 3px; border-style: solid; border-radius: 8px;");

        #self.window.lineEdit.focusInEvent.connect(self.showKeyboard)

        #self.nameInput = MatchBoxLineEdit()
        #self.window.setStyle("windows")
        # Now show the window
        self.window.setWindowFlag(Qt.FramelessWindowHint)

        self.window.show()
        self.window.showFullScreen()
        #print(QStyleFactory.keys())

    def showKeyboard(self,arg):
        global gripperopen
        if(gripperopen ==0):
            returned_value = subprocess.call("/home/pi/machine/shmUtil -set 566 1", shell=True)
            gripperopen = 1
        else:
            returned_value = subprocess.call("/home/pi/machine/shmUtil -set 566 0", shell=True)
            gripperopen = 0
        #self.window.pushButton1.nummpad.show()
    def runjob2(self,arg):
        global shouldquit
        shouldquit = 1
        self.thread.quit()
        self.thread.wait()

    def runjob3(self,arg):
        _robot.reset_alarm(FS100.RESET_ALARM_TYPE_ALARM)
        _robot.reset_alarm(FS100.RESET_ALARM_TYPE_ERROR)
        error = _robot.select_job(os.path.basename("LEFTTEST"))
        print(error)
        #self.window.label.setText(error);
        error = _robot.select_cycle(FS100.CYCLE_TYPE_ONE_CYCLE)
        print(error)
        error = _robot.switch_power(FS100.POWER_TYPE_SERVO, FS100.POWER_SWITCH_ON)
        print(error)
        error = _robot.play_job()
    #@Slot(int)
    def click(self,arg):
        global shouldquit
        shouldquit = 1
        self.thread.quit()
        self.thread.wait()
        exit()
        print("clicked");
    def startJob(self,jobtext):
        error = _robot.select_job(os.path.basename(jobtext))
        print(error)
        #self.window.label.setText(error);
        error = _robot.select_cycle(FS100.CYCLE_TYPE_ONE_CYCLE)
        print(error)
        error = _robot.switch_power(FS100.POWER_TYPE_SERVO, FS100.POWER_SWITCH_ON)
        print(error)
        error = _robot.play_job()

    def runjob(self,arg):
        global jobstarted

        result = dict()
        
        _robot.get_status(result)
        print(FS100.ERROR_SUCCESS)
        print(result)

        returned_value = subprocess.call("/home/pi/machine/shmUtil -set 566 1", shell=True)
         # Also test commands 0x87, 0x84, 0x83 and 0x86
        #'{}/{}'.format(test_dir, 'TEST_FS100.JBI')

        #_robot.show_text_on_pendant("Kim is in control!!!")
        _robot.reset_alarm(FS100.RESET_ALARM_TYPE_ALARM)
        _robot.reset_alarm(FS100.RESET_ALARM_TYPE_ERROR)
        #
        self.startJob("PICKSTACK")
        time.sleep(0.5)
        #error = _robot.select_job(os.path.basename("PICKSTACK"))
        #print(error)
        #self.window.label.setText(error);
        #error = _robot.select_cycle(FS100.CYCLE_TYPE_ONE_CYCLE)
        #print(error)
        #error = _robot.switch_power(FS100.POWER_TYPE_SERVO, FS100.POWER_SWITCH_ON)
        #print(error)
        #error = _robot.play_job()
        #print(error)
        jobstarted = 1

        #_robot.read_alarm_info(1, result)
        #print(result)
        result = dict()
        _robot.acquire_system_info(FS100.SystemInfoType.R1, result)
        print(result)


        var = FS100.Variable(FS100.VarType.IO, 2701)
        error = _robot.read_variable(var)
        print(error)
        print("")
        print("Read IO #{} -> {}".format("2701", var.val))
        error = _robot.write_variable(var)
        print(error)

        print("Write IO #{} <- {}".format("2701", var.val))

        TEST_IO_NUMBER = 8069
        TEST_IO_NUMBERIN = 8068
        var = FS100.Variable(FS100.VarType.IO, TEST_IO_NUMBER)
        error = _robot.read_variable(var)
        print("")
        print("Read IO #{} -> {}".format(TEST_IO_NUMBER, var.val))
      
if __name__ == "__main__":
    app = QApplication(sys.argv)
    #app.setStyle("macintosh")
    #app.setStyle("fusion")
    #app.setStyle("cleanlooks")
    #app.setStyle("Fusion")
    #app.setStyle("motif")
    with open("MacOS.qss", "r") as f:
        _style = f.read()
        app.setStyleSheet(_style)
    widget = MainWindow()
    sys.exit(app.exec())
