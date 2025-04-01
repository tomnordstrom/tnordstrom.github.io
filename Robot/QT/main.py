
# This Python file uses the following encoding: utf-8
import os
from pathlib import Path
import sys
import time
import subprocess

#os.environ["QT_IM_MODULE"] = "qtvirtualkeyboard"
from fs100 import FS100
from PySide2.QtWidgets import QApplication, QMainWindow, QDialog, QLabel, QGridLayout, QPushButton, QWidget, QStyleFactory,QLineEdit
from PySide2.QtCore import QThread, Qt, QFile, QObject, Signal, Slot
from PySide2.QtUiTools import QUiLoader
from PySide2 import QtGui
from functools import partial

from MyWidgets import MyQLineEdit

robotenabled = 0
#iolist = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)
iolist = [0] * 21
oldiolist = [0] * 21

analog1on = False

iolistin = [0] * 20
oldiolistin = [0] * 20
oldconvfull = -1
IO_DIGOUT_START = 701
RT_INT_START = 1000
IO_DIGIN_START=              RT_INT_START + 1
IO_ANAIN_START=              RT_INT_START + 43
ErrorList = []
#0-9
analog1 = 0
analog2 = 0
analog3 = 0
analog4 = 0
lastwasknown = False
ErrorList.append("No error")
ErrorList.append("Emergency stop")
ErrorList.append("EBM communication failed")
ErrorList.append("EBM firmware update failed")
ErrorList.append("Wrong version of EBM firmware")
ErrorList.append("EBM digital output overload, please restart the machine")
ErrorList.append("Internal USB stick is corrupt. Settings will not be saved.")
ErrorList.append("Robot is not in run mode")
ErrorList.append("Robot alarm")
ErrorList.append("Pallet is full")
# 10 - 19
ErrorList.append("")
ErrorList.append("Too many stacks on conveyor. Remove all stacks.")
ErrorList.append("Conveyor full")
ErrorList.append("Stack is stuck at conveyor full sensor")
ErrorList.append("Stacks are too close at conveyor full sensor")
ErrorList.append("")
ErrorList.append("")
ErrorList.append("")
ErrorList.append("")
ErrorList.append("")
# 20 - 29
ErrorList.append("")
ErrorList.append("Stack at input of picking station, please remove")
ErrorList.append("Stack at left sensor of picking station, please remove")
ErrorList.append("Stack at right sensor of picking station, please remove")
ErrorList.append("Stack in picking station, please remove")
ErrorList.append("Stack is stuck in picking station")
ErrorList.append("Stack height sensor can't find the stack, please adjust")

INFO_ERROR=                   RT_INT_START + 152
RB_MODE=                      RT_INT_START + 300
RB_MODE_TEACH= 0
RB_MODE_RUN=   1
RB_ALARM=                     RT_INT_START + 301
RB_COLLISION=                 RT_INT_START + 302

RB_ALARM=                     RT_INT_START + 301
RB_HEARTBEAT=                 RT_INT_START + 303
RB_COMMAND_RESPONSE=          RT_INT_START + 310
RB_RESPONSE_NONE=      0
RB_RESPONSE_EXECUTING= 1
RB_RESPONSE_FINISHED=  2
RB_RESPONSE_FAILED=    3
RB_FINGER_RELEASE=            RT_INT_START + 311
RB_EXECUTE_STATE=             RT_INT_START + 304
RB_COMMAND=                   RT_INT_START + 350
RB_COMMAND_NONE=          0
RB_COMMAND_GOTO_HOME=     1
RB_COMMAND_GOTO_PICKING=  2
RB_COMMAND_GOTO_PALLET=   3
RB_COMMAND_CONTINUE=      4
RB_COMMAND_RESET_ALARM=   5
RB_COMMAND_GOTO_TRANSPORT=   6

CFG_ENABLE_ROBOT=             608

RB_TOOL=                      RT_INT_START + 351
RB_XPOS=                      RT_INT_START + 352
RB_YPOS=                      RT_INT_START + 353
RB_ZPOS=                      RT_INT_START + 354
RB_DIRECTION=                 RT_INT_START + 355

RB_HOLD=                      RT_INT_START + 356

INFO_STACK_HEIGHT=          RT_INT_START + 156
INFO_RUNNING=               RT_INT_START + 150
RT_CONVFULL=                RT_INT_START + 159

VALUE_DISABLED=             0
VALUE_STOPPED=              1
VALUE_STARTED=              2
VALUE_SAVING=               3



row_1_start=                1
row_2_start=                row_1_start+10
row_3_start=                row_2_start+10

rbPosX=0
rbPosY=0
rbPosZ=0
rbPosTool=0

_robot = FS100('192.168.255.1')

shouldquit = 0
statusdict = dict()
var8069 = FS100.Variable(FS100.VarType.IO, 8069)
varRBInt = FS100.Variable(FS100.VarType.INTEGER, 0)
varexeStateRBInt = FS100.Variable(FS100.VarType.INTEGER, 1)
varPos = FS100.Variable(FS100.VarType.ROBOT_POSITION, 1)
varPos2 = FS100.Variable(FS100.VarType.ROBOT_POSITION, 2)

CMD_PICKSTACK = 8
CMD_PUTSTACK = 9
CMD_START = 1
CMD_STOP = 2
CMD_SETUP = 5
CMD_GOTOIO = 13
CMD_EXIT = 5
CMD_CANCEL = 10
CMD_RESETROBOT = 15
CMD_CONTINUE_ERR = 17
CMD_CONTINUE = 18
CMD_ABORT = 19
CMD_GOHOME = 20
CMD_CLEAR_PALLET_DIALOG=     21
CMD_GOTO_TRANSPORT=     22
#CMD_GOHOLD = 20
#var8069 = FS100.Variable(FS100.VarType.IO, 8069)

togglebuttonList = []
iobuttonList = []
row1 = []
row2 = []
row3 = []
oldrow1height = [0] * 10
oldrow2height = [0] * 10
oldrow3height = [0] * 10

memState = 2125
oldstate = 0
shmemState = 0

state_ready = 0
state_error = 1
state_setup = 2
state_io = 3
state_collision = 6
state_changepallet = 7
# Extend from QObject
blink = 1
gripperopen = 0
jobstarted = 0
checkwhenfinished = 0
#alarm example
#hold_externally': False, 'hold_by_cmd': False, 'alarming': True, 'error_occurring': False, 'servo_on': False}
#{'code': 6004, 'data': 1, 'type': 1, 'time': '2024/04/19 10:16', 'name': 'ESCAPE FROM CLAMPING ERROR(PFL)'}

#status {'step': False, 'one_cycle': True, 'auto_and_cont': False, 'running': False, 'guard_safe': False, 'teach': False, 'play': True, 'cmd_remote': False, 'hold_by_pendant': False, 'hold_externally': False, 'hold_by_cmd': False, 'alarming': False, 'error_occurring': False, 'servo_on': True}

def startJob(jobtext):
    global checkwhenfinished
    global _robot
    print("starting job")
    error = _robot.select_job(os.path.basename(jobtext))
    print(error)
    #self.window.label.setText(error);
    error = _robot.select_cycle(FS100.CYCLE_TYPE_ONE_CYCLE)
    print(error)
    error = _robot.switch_power(FS100.POWER_TYPE_SERVO, FS100.POWER_SWITCH_ON)
    print(error)
    error = _robot.play_job()
    #RB_RESPONSE_EXECUTING= 1
    #RB_RESPONSE_FINISHED=  2
    #RB_RESPONSE_FAILED=    3
    if(error ==0):
        checkwhenfinished = 1
    else:
        print("send failed")
        writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_FAILED)

def readshint(vilken):
    #print("readshint")
    cmd = "/home/pi/machine/shmUtil -read "+ str(vilken)
    #print(cmd)
    return int(subprocess.check_output(cmd, shell=True))

def writeshint(vilken,vad):
    cmd = "/home/pi/machine/shmUtil -write "+ str(vilken) + " " + str(vad)
    #print(cmd)
    return int(subprocess.call(cmd, shell=True))
def updatePallet():
    global row1,row2,row3
    global oldrow1height,oldrow2height,oldrow3height
    global row_1_start
    #print("updatepallet")

    for a in range(5):
        height = readshint(row_1_start+a)
        if(height != oldrow1height[a]):
            oldrow1height[a] = height
            if(height > 0):
                row1[a].setText(str(height))
                row1[a].show()
            else:
                row1[a].setText("0")
                row1[a].hide()
    for a in range(5):
        height = readshint(row_2_start+a)
        if(height != oldrow2height[a]):
            oldrow2height[a] = height
            if(height > 0):
                row2[a].setText(str(height))
                row2[a].show()
            else:
                row2[a].setText("0")
                row2[a].hide()
    for a in range(5):
        height = readshint(row_3_start+a)
        if(height != oldrow3height[a]):
            oldrow3height[a] = height
            if(height > 0):
                row3[a].setText(str(height))
                row3[a].show()
            else:
                row3[a].setText("0")
                row3[a].hide()

def updateIO():

    global togglebuttonList,analog1,analog2,analog3,analog4
    global iobuttonList
    global iolist, oldiolist
    global iolistin, oldiolistin
    #iolist = (0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0)
    #iolist[5] = 1
    #togglebuttonList[1].setDown(True)
    #togglebuttonList[2].setDown(True)

    #shared_Mem->integer.Value[IO_ANAIN_START + i*2+1] = ((tmpAnaIn / 1000) << 16) + (tmpAnaIn % 1000) * 10;
    analog1 = readshint(IO_ANAIN_START)
    analog2 = readshint(IO_ANAIN_START+2)
    analog3 = readshint(IO_ANAIN_START+4)
    analog4 = readshint(IO_ANAIN_START+6)

    for a in range(21):
        if(iolist[a] != oldiolist[a]):
            togglebuttonList[a].setDown(iolist[a])
            writeshint(IO_DIGOUT_START+(a-1),iolist[a])
            oldiolist[a] = iolist[a];

    #for (i = 1; i <= 28; i++)
    #   {
    #      bool input = ebmGetInput(0,i);
    #      if (input != shared_Mem->integer.Value[IO_DIGIN_START + i - 1])
    #      {
    #         shared_Mem->integer.Value[IO_DIGIN_LAST] = i;
    #         shared_Mem->integer.Value[IO_SOUND] = 1;
    #      }
    #      shared_Mem->integer.Value[IO_DIGIN_START + i - 1] = (int)input;
    #   }
    try:
        for a in range(20):
            iolistin[a] = readshint(IO_DIGIN_START+(a))
            print(iolistin[a])
            if(oldiolistin[a] != iolistin[a]):
                if(iolistin[a] == 1):
                    iobuttonList[a].setStyleSheet("color: #FFFFFF; border-color: grey; background-color: red; border-width: 3px; border-style: solid; border-radius: 8px;");
                else:
                    iobuttonList[a].setStyleSheet("color: #000000; border-color: grey; background-color: 0xFFFFFF; border-width: 3px; border-style: solid; border-radius: 8px;");
                oldiolistin[a] = iolistin[a]
    except:
        print("Something went wrong")

class ReadStatus(QObject):
    finished = Signal()
    progress = Signal(int)

    def run(self):
        global shmemState
        global rbPosX, rbPosY, rbPosZ, rbPosTool
        global checkwhenfinished
        global jobstarted
        global _robot
        global shouldquit
        global statusdict
        global var8069
        global varRBInt
        global varPos
        mystate = 1
        myhold = False
        error = 1
        """Long-running task."""
        #här sätta RB_COMMAND_RESPONSE till 0 om cmd är 0
        #print(_robot)
        while(error == 1):
            _robot = FS100('192.168.255.1')
            time.sleep(1)
            error = _robot.read_variable(varRBInt)
            print(error)


        _robot.read_variable(varRBInt)
        varRBInt.val = 88
        _robot.write_variable(varRBInt)

        while(shouldquit == 0):

            time.sleep(0.01)
            shmemState = readshint(memState)
            updatePallet()

            if(readshint(RB_COMMAND) == 0):
                writeshint(RB_COMMAND_RESPONSE,0)
            _robot.get_status(statusdict)
            #var8069 = FS100.Variable(FS100.VarType.IO, 8069)
            error = _robot.read_variable(var8069)
            if(statusdict["play"] == True):
                writeshint(RB_MODE, RB_MODE_RUN)
            else:
                writeshint(RB_MODE, RB_MODE_TEACH)
            if(var8069.val & 64 == 64):
                writeshint(RB_COLLISION, 1)
                if(readshint(RB_COMMAND_RESPONSE) != 0):
                    writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_FAILED)
            else:
                writeshint(RB_COLLISION, 0)
            result = dict()
            #_robot.read_alarm_info(4, result)

            #print(result)
            _robot.get_last_alarm(result)
            #print(result)
            writeshint(RB_ALARM, result["code"])
            #print(str(var8069.val))
            #print(RB_COMMAND)
            #readshint(1100)
            if(shmemState == state_io):
                updateIO()
            error = _robot.read_variable(varexeStateRBInt)
            writeshint(RB_EXECUTE_STATE,varexeStateRBInt.val)
            error = _robot.read_variable(varRBInt)
            if(readshint(RB_HOLD) == 1):
                if(myhold == False):
                    _robot.switch_power(FS100.POWER_TYPE_HOLD, FS100.POWER_SWITCH_ON)
                myhold = True
            else:
                if(myhold == True):
                    _robot.switch_power(FS100.POWER_TYPE_HOLD, FS100.POWER_SWITCH_OFF)
                    _robot.switch_power(FS100.POWER_TYPE_SERVO, FS100.POWER_SWITCH_ON)

                if(varRBInt.val != 88 and myhold == True):
                    errortest = _robot.play_job()
                myhold = False
            if(checkwhenfinished == 1):
                #print("readrbint")
                if(varRBInt.val == 77):
                    #print("running")
                    hej=1
                    #running
                if(varRBInt.val == 88):
                    writeshint(RB_COMMAND_RESPONSE, RB_RESPONSE_FINISHED)
                    print("fisnished")
                    checkwhenfinished = 0
                    #finished
                #print(varRBInt.val)
            if(readshint(RB_COMMAND) == RB_COMMAND_GOTO_PICKING and readshint(RB_COMMAND_RESPONSE) == 0):
                print("picking")
                varRBInt.val = 77
                _robot.write_variable(varRBInt)
                time.sleep(0.1)
                writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_EXECUTING)
                rbPosX = readshint(RB_XPOS)*1000
                rbPosY = readshint(RB_YPOS)*1000
                rbPosZ = readshint(RB_ZPOS)*1000
                rbPosTool = readshint(RB_DIRECTION)*10000

                print(rbPosX,rbPosY,rbPosZ,rbPosTool)
                error = _robot.read_variable(varPos)
                varPos.val['pos'] = (rbPosX, rbPosY, rbPosZ+250000, 0, 0, rbPosTool, 0, 0)
                error = _robot.write_variable(varPos)
                jobstarted = 1
                time.sleep(0.1)
            if(readshint(RB_COMMAND) == RB_COMMAND_GOTO_PALLET and readshint(RB_COMMAND_RESPONSE) == 0):
                print("Putting")
                varRBInt.val = 77
                _robot.write_variable(varRBInt)
                time.sleep(0.1)
                writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_EXECUTING)
                rbPosX = readshint(RB_XPOS)*1000
                rbPosY = readshint(RB_YPOS)*1000
                rbPosZ = readshint(RB_ZPOS)*1000
                rbPosTool = readshint(RB_DIRECTION)*10000

                print(rbPosX,rbPosY,rbPosZ,rbPosTool)
                error = _robot.read_variable(varPos)
                error = _robot.read_variable(varPos2)
                varPos.val['pos'] = (rbPosX, rbPosY, rbPosZ, 0, 0, rbPosTool, 0, 0)
                varPos2.val['pos'] = (rbPosX, rbPosY, rbPosZ+250000, 0, 0, rbPosTool, 0, 0)
                error = _robot.write_variable(varPos)
                error = _robot.write_variable(varPos2)

                jobstarted = 2
                #time.sleep(0.1)
            if(readshint(RB_COMMAND) == RB_COMMAND_GOTO_HOME and readshint(RB_COMMAND_RESPONSE) == 0):
                print("home")
                varRBInt.val = 77
                _robot.write_variable(varRBInt)
                time.sleep(0.1)
                writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_EXECUTING)

                jobstarted = 4
                #time.sleep(0.1)
            if(readshint(RB_COMMAND) == RB_COMMAND_GOTO_TRANSPORT and readshint(RB_COMMAND_RESPONSE) == 0):
                print("rbb transport")
                varRBInt.val = 77
                _robot.write_variable(varRBInt)
                time.sleep(0.1)
                writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_EXECUTING)

                jobstarted = 6
                #time.sleep(0.1)
            if(readshint(RB_COMMAND) == RB_COMMAND_CONTINUE):
                print("before Continue")

            if(readshint(RB_COMMAND) == RB_COMMAND_CONTINUE and readshint(RB_COMMAND_RESPONSE) == 0):
                print("Continue")
                time.sleep(0.1)
                writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_EXECUTING)
                #time.sleep(0.1)
                jobstarted = 3
            if(readshint(RB_COMMAND) == RB_COMMAND_RESET_ALARM):
                _robot.reset_alarm(FS100.RESET_ALARM_TYPE_ALARM)
                _robot.reset_alarm(FS100.RESET_ALARM_TYPE_ERROR)

            #print('returned value:', readshint(1100))
            #_robot.get_last_alarm(result)
            if(jobstarted == 1):
                if(lastwasknown == True):
                    time.sleep(0.2)
                    startJob("PICKSTACKSHORT")
                else:
                    startJob("PICKSTACK")
                #time.sleep(0.2)
                lastwasknown = False
                jobstarted = 0
            if(jobstarted == 2):
                #time.sleep(0.1)
                startJob("PUTSTACK")
                #time.sleep(0.2)
                lastwasknown = False
                jobstarted = 0
            if(jobstarted == 3):
                _robot.switch_power(FS100.POWER_TYPE_HOLD, FS100.POWER_SWITCH_OFF)
                print("hej")
                print(readshint(RB_COMMAND_RESPONSE))

                if(varRBInt.val != 88):
                    errortest = _robot.play_job()
                    print(errortest)
                #RB_RESPONSE_EXECUTING= 1
                #RB_RESPONSE_FINISHED=  2
                #RB_RESPONSE_FAILED=    3

                jobstarted = 0
            if(jobstarted == 4):
                #time.sleep(0.1)
                startJob("KNOWN")
                #time.sleep(0.2)
                jobstarted = 0
                lastwasknown = True
                #if(errortest != 0):
                #    print("send failed")
                #    writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_FAILED)
            if(jobstarted == 5): #HOLD
                print("hold by 5")
                error = _robot.switch_power(FS100.POWER_TYPE_HOLD, FS100.POWER_SWITCH_ON)
                print(error)
                jobstarted = 0
            if(jobstarted == 6):
                print("rb transport")
                #time.sleep(0.1)
                startJob("HOMETRANSPORT")
                #time.sleep(0.2)
                jobstarted = 0
                lastwasknown = True
                #if(errortest != 0):
                #    print("send failed")
                #    writeshint(RB_COMMAND_RESPONSE,RB_RESPONSE_FAILED)
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
        global oldiolistin
        global robotenabled
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

        self.window.setCursor(Qt.BlankCursor)
        robotenabled = readshint(CFG_ENABLE_ROBOT)
        print("button")
        print(robotenabled)
        #writeshint(CFG_ENABLE_ROBOT,0)
        self.window.label_2.setText("")
        if(robotenabled == 0):
            self.window.label_enable.setText("Enable Robot")
            self.window.pushButton_disable.setIcon(QtGui.QIcon("/home/pi/toggle_switch_icon_512_off.png"));
        else:
            self.window.label_enable.setText("Disable Robot")
            self.window.pushButton_disable.setIcon(QtGui.QIcon("/home/pi/toggle_switch_icon_512_on.png"));

        for a in range(5):
            oldrow1height[a] = -1
            oldrow2height[a] = -1
            oldrow3height[a] = -1

        for a in range(20):
            oldiolistin[a] = -1
        self.window.show()
    def reportStatus(self,arg):
        global shmemState, oldstate
        global _robot
        global jobstarted
        global blink
        global analog1,analog2,analog3,analog4
        global lastwasknown
        global convfull, oldconvfull

        #updatePallet()
        convfull = readshint(RT_CONVFULL)
        if(convfull != oldconvfull):
            if(convfull == 1):
                self.window.label_convfull.show()
                self.collision.label_convfull.show()
            else:
                self.window.label_convfull.hide()
                self.collision.label_convfull.hide()
        olfconvfull = convfull



        self.window.stack_height.setText(str(readshint(INFO_STACK_HEIGHT))+" mm")
        self.window.stack_height.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: grey; border-width: 3px; border-style: solid; border-radius: 8px;")
        run_state=readshint(INFO_RUNNING)
        if(run_state == VALUE_STARTED):
            self.window.label.setText("Started");
            self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; border-width: 3px; border-style: solid; border-radius: 8px; background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #fbfdfd, stop:0.5 green stop:1 #fbfdfd);");
        if(run_state == VALUE_STOPPED):
            self.window.label.setText("Stopped");
            self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; border-width: 3px; border-style: solid; border-radius: 8px; background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #fbfdfd, stop:0.5 red stop:1 #fbfdfd);");
        if(run_state == VALUE_DISABLED):
            self.window.label.setText("Disabled");
            self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; border-width: 3px; border-style: solid; border-radius: 8px; background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #fbfdfd, stop:0.5 grey stop:1 #fbfdfd);");
        if(run_state == VALUE_SAVING):
            self.window.label.setText("Saving");
            self.window.label.setStyleSheet("color: #000000; border-color: grey; border-width: 3px; border-style: solid; border-radius: 8px; background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #fbfdfd, stop:0.5 yellow stop:1 #fbfdfd);");

        if(shmemState == state_io):
            self.iotest.analog1.setText(str(analog1))
            self.iotest.analog2.setText(str(analog2))
            self.iotest.analog3.setText(str(analog3))
            self.iotest.analog4.setText(str(analog4))
        if(shmemState != oldstate):
            if(shmemState == state_io):
                self.iotest.showFullScreen()
                self.window.hide()
                self.error.hide()
                self.setup.hide()
                self.collision.hide()
                self.changepallet.hide()
            if(shmemState == state_changepallet):
                self.changepallet.showFullScreen()
                self.iotest.hide()
                self.window.hide()
                self.error.hide()
                self.setup.hide()
                self.collision.hide()
            if(shmemState == state_ready):
                self.window.showFullScreen()
                self.error.hide()
                self.iotest.hide()
                self.setup.hide()
                self.collision.hide()
                self.changepallet.hide()
            if(shmemState == state_setup):
                self.setup.showFullScreen()
                self.window.hide()
                self.iotest.hide()
                self.error.hide()
                self.collision.hide()
                self.changepallet.hide()
            if(shmemState == state_error):

                self.error.label.setText(ErrorList[readshint(INFO_ERROR)]);
                self.error.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: red; border-width: 3px; border-style: solid; border-radius: 8px;")
                self.error.showFullScreen()
                self.window.hide()
                self.iotest.hide()
                self.setup.hide()
                self.collision.hide()
                self.changepallet.hide()
            if(shmemState == state_collision):
                self.collision.label.setText("Collision detected");
                self.collision.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: orange; border-width: 3px; border-style: solid; border-radius: 8px;")
                self.collision.showFullScreen()
                self.window.hide()
                self.iotest.hide()
                self.error.hide()
                self.setup.hide()
                self.changepallet.hide()

            oldstate = shmemState

        if(statusdict["running"] == True):
            hej = 1
            #print("running")
        if(statusdict["running"] == False):# and jobstarted == 1):
            #print("close gripper")
           hej=2


        if(statusdict["play"] == True):
        #    self.window.label.setText("Run mode");
        #    self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; border-width: 3px; border-style: solid; border-radius: 8px; background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #fbfdfd, stop:0.5 green stop:1 #fbfdfd);");
            self.iotest.label.setText("Run mode");
            self.iotest.label.setStyleSheet("color: #FFFFFF; border-color: grey; border-width: 3px; border-style: solid; border-radius: 8px; background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #fbfdfd, stop:0.5 green stop:1 #fbfdfd);");
        else:
        #    self.window.label.setText("Teach mode");
        #    self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: red; border-width: 3px; border-style: solid; border-radius: 8px;");
            self.iotest.label.setText("Teach mode");
            self.iotest.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: red; border-width: 3px; border-style: solid; border-radius: 8px;");
        if(var8069.val & 64 == 64):

            self.window.pushButtonForce.setStyleSheet("color: #FFFFFF; background-color: orange; border-radius: 16px");
            self.window.pushButtonForce.setText("Collision");
            if(blink):
                #self.window.pushButtonForce.hide()
                blink = 0
            else:
                blink = 1
                #self.window.pushButtonForce.show()
        else:
            self.window.pushButtonForce.setStyleSheet("color: #FFFFFF; background-color: green; border-radius: 16px");
            self.window.pushButtonForce.setText("Force \n monitor");

    def load_ui(self):
        loader = QUiLoader()
        loader.registerCustomWidget(MyQLineEdit)
        path = Path(__file__).resolve().parent / "form.ui"
        ui_file = QFile("form.ui")#path)
        ui_file.open(QFile.ReadOnly)

        # Load window manually
        self.window = loader.load(ui_file)
        ui_file.close()

        ui_file = QFile("iotest.ui")#path)
        ui_file.open(QFile.ReadOnly)
        # Load window manually
        self.iotest = loader.load(ui_file)
        ui_file.close()

        ui_file = QFile("setup.ui")#path)
        ui_file.open(QFile.ReadOnly)
        # Load window manually
        self.setup = loader.load(ui_file)
        ui_file.close()

        ui_file = QFile("error.ui")#path)
        ui_file.open(QFile.ReadOnly)
        # Load window manually
        self.error = loader.load(ui_file)
        ui_file.close()

        ui_file = QFile("collision.ui")#path)
        ui_file.open(QFile.ReadOnly)
        # Load window manually
        self.collision = loader.load(ui_file)
        ui_file.close()

        ui_file = QFile("changepallet.ui")#path)
        ui_file.open(QFile.ReadOnly)
        # Load window manually
        self.changepallet = loader.load(ui_file)
        ui_file.close()

        #self.error.pushButton_exit.setText("Continue")
        #self.error.pushButton_exit.clicked.connect(self.ContinueErr)

        self.window.label_2.setStyleSheet("color: #FFFFFF; background-color: white; border-radius: 0px");

        self.window.label_convfull.setStyleSheet("color: #FFFFFF; background-color: grey; border-radius: 16px");
        self.collision.label_convfull.setStyleSheet("color: #FFFFFF; background-color: grey; border-radius: 16px");
        self.window.pushButton1.setText("Exit")
        self.window.pushButton1.clicked.connect(self.click)
        self.window.pushButton1.hide()

        self.window.pushButton_2.setText("Pick stack")
        self.window.pushButton_2.clicked.connect(self.PickStack)
        self.window.pushButton_2.hide()

        self.window.pushButton_3.setText("Put stack")
        self.window.pushButton_3.clicked.connect(self.PutStack)
        self.window.pushButton_3.hide()

        self.window.pushButton_home.setText("Go Home")
        self.window.pushButton_home.clicked.connect(self.GoHome)
        self.window.pushButton_home.hide()

        self.window.pushButton_clear.setText("Change Pallet")
        self.window.pushButton_clear.clicked.connect(self.GoClear)
        self.window.pushButton_clear.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: grey; border-radius: 16px");

        self.window.pushButton_start.setText("Start")
        self.window.pushButton_start.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: green; border-radius: 16px");
        self.window.pushButton_start.clicked.connect(self.Start)

        self.window.pushButton_stop.setText("Stop")
        self.window.pushButton_stop.setStyleSheet("color: #FFFFFF; background-color: red; border-radius: 16px");
        self.window.pushButton_stop.clicked.connect(self.Stop)

        self.error.label_error.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: red; border-width: 3px; border-style: solid; border-radius: 8px;");

        self.error.pushButton_start.setText("Start")
        self.error.pushButton_start.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: green; border-radius: 16px");
        self.error.pushButton_start.clicked.connect(self.Start)

        self.error.pushButton_stop.setText("Stop")
        self.error.pushButton_stop.setStyleSheet("color: #FFFFFF; background-color: red; border-radius: 16px");
        self.error.pushButton_stop.clicked.connect(self.Stop)

        self.setup.pushButton_iotest.setText("I/O Test")
        self.setup.pushButton_iotest.clicked.connect(self.IOTest)

        #self.setup.pushButton_transport.setText("Transport")
        #self.setup.pushButton_transport.clicked.connect(self.GoTransport)
        self.iotest.pushButton_transport.setText("Transport")
        self.iotest.pushButton_transport.clicked.connect(self.GoTransport)

        self.iotest.pushButton_exit.setText("Exit")
        self.iotest.pushButton_exit.clicked.connect(self.exitIO)

        self.iotest.pushButton_run1.clicked.connect(self.GoAnalog1)
        self.iotest.pushButton_run1.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: grey; border-radius: 16px");
        self.iotest.pushButton_run2.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: grey; border-radius: 16px");
        self.iotest.pushButton_run3.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: grey; border-radius: 16px");
        self.iotest.pushButton_run4.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: grey; border-radius: 16px");

        self.setup.pushButton_exit.setText("Ok")
        self.setup.pushButton_exit.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: green; border-radius: 16px");
        self.setup.pushButton_exit.clicked.connect(self.OkSave)

        self.setup.pushButton_cancel.setText("Cancel")
        self.setup.pushButton_cancel.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: red; border-radius: 16px");
        self.setup.pushButton_cancel.clicked.connect(self.Cancel)

        self.window.pushButton_setup.setText("Setup")
        self.window.pushButton_setup.setStyleSheet("color: #FFFFFF; background-color: grey; border-radius: 16px");
        self.window.pushButton_setup.clicked.connect(self.Gosetup)

        #self.window.pushButton_stack_1.setGeometry(150,150,100,50)


        self.window.label_2.setStyleSheet("color: #000000; border-color: white; background-color: white; border-width: 3px; border-style: solid; border-radius: 16px;")

        self.window.pushButton_disable.setText("h")
        self.window.pushButton_disable.setStyleSheet("color: #000000; border-color: white; background-color: white; border-radius: 0px");
        self.window.pushButton_disable.clicked.connect(self.GoDisable)
        #self.window.pushButton_disable.
        #rMyIcon = QtGui.QPixmap("/home/pi/toggle_switch_icon_512_on.png")
        #self.window.pushButton_disable.setIcon(QtGui.QIcon(rMyIcon))
        self.window.pushButton_disable.setIcon(QtGui.QIcon("/home/pi/toggle_switch_icon_512_off.png"));
        #self.window.pushButton_disable.set

        self.window.pushButton_hold.setText("Hold")
        self.window.pushButton_hold.clicked.connect(self.GoHold)
        self.window.pushButton_hold.hide()

        self.window.pushButton_cont.setText("Continue")
        self.window.pushButton_cont.clicked.connect(self.continue_col)
        self.window.pushButton_cont.hide()
        self.window.pushButtonForce.setText("Force monitor")
        self.window.pushButtonForce.setStyleSheet("color: #FFFFFF; background-color: green; border-radius: 16px");
        self.window.pushButtonForce.hide()

        self.collision.pushButtonForce.setText("Continue")
        self.collision.pushButton_abort.setText("Abort")
        self.collision.pushButton_abort.clicked.connect(self.abort)
        self.collision.pushButtonForce.clicked.connect(self.continue_col)
        self.collision.pushButtonForce.setStyleSheet("color: #FFFFFF; background-color: orange; border-radius: 16px");
        self.collision.pushButton_abort.setStyleSheet("color: #FFFFFF; background-color: red; border-radius: 16px");

        self.changepallet.pushButton_yes.setText("Yes")
        self.changepallet.pushButton_abort.setText("Abort")
        self.changepallet.pushButton_abort.clicked.connect(self.abort)
        self.changepallet.pushButton_yes.clicked.connect(self.continue_col)
        self.changepallet.pushButton_yes.setStyleSheet("color: #FFFFFF; background-color: green; border-radius: 16px");
        self.changepallet.pushButton_abort.setStyleSheet("color: #FFFFFF; background-color: red; border-radius: 16px");
        #self.window.label.setText("Not Ready");
        self.changepallet.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: blue; border-width: 3px; border-style: solid; border-radius: 8px;");

        self.window.pushButtonForce.clicked.connect(self.resetCobot)

        self.window.label.setText("Starting up");
        self.window.label.setStyleSheet("color: #FFFFFF; border-color: grey; background-color: red; border-width: 3px; border-style: solid; border-radius: 8px;");

        self.iotest.pushButton_1.clicked.connect(lambda x:self.toggleIO(x,1))
        self.iotest.pushButton_2.clicked.connect(lambda x:self.toggleIO(x,2))
        self.iotest.pushButton_3.clicked.connect(lambda x:self.toggleIO(x,3))
        self.iotest.pushButton_4.clicked.connect(lambda x:self.toggleIO(x,4))
        self.iotest.pushButton_5.clicked.connect(lambda x:self.toggleIO(x,5))
        self.iotest.pushButton_6.clicked.connect(lambda x:self.toggleIO(x,6))
        self.iotest.pushButton_7.clicked.connect(lambda x:self.toggleIO(x,7))
        self.iotest.pushButton_8.clicked.connect(lambda x:self.toggleIO(x,8))
        self.iotest.pushButton_9.clicked.connect(lambda x:self.toggleIO(x,9))
        self.iotest.pushButton_10.clicked.connect(lambda x:self.toggleIO(x,10))
        self.iotest.pushButton_11.clicked.connect(lambda x:self.toggleIO(x,11))
        self.iotest.pushButton_12.clicked.connect(lambda x:self.toggleIO(x,12))
        self.iotest.pushButton_13.clicked.connect(lambda x:self.toggleIO(x,13))
        self.iotest.pushButton_14.clicked.connect(lambda x:self.toggleIO(x,14))
        self.iotest.pushButton_15.clicked.connect(lambda x:self.toggleIO(x,15))
        self.iotest.pushButton_16.clicked.connect(lambda x:self.toggleIO(x,16))
        self.iotest.pushButton_17.clicked.connect(lambda x:self.toggleIO(x,17))
        self.iotest.pushButton_18.clicked.connect(lambda x:self.toggleIO(x,18))
        self.iotest.pushButton_19.clicked.connect(lambda x:self.toggleIO(x,19))
        self.iotest.pushButton_20.clicked.connect(lambda x:self.toggleIO(x,20))
        #self.iotest.pushButton_1.clicked.connect(self.toggleIO)

        #self.window.lineEdit.focusInEvent.connect(self.showKeyboard)

        #self.nameInput = MatchBoxLineEdit()c
        #self.window.setStyle("windows")
        # Now show the window
        togglebuttonList.append(self.iotest.pushButton_1)
        togglebuttonList.append(self.iotest.pushButton_1)
        togglebuttonList.append(self.iotest.pushButton_2)
        togglebuttonList.append(self.iotest.pushButton_3)
        togglebuttonList.append(self.iotest.pushButton_4)
        togglebuttonList.append(self.iotest.pushButton_5)
        togglebuttonList.append(self.iotest.pushButton_6)
        togglebuttonList.append(self.iotest.pushButton_7)
        togglebuttonList.append(self.iotest.pushButton_8)
        togglebuttonList.append(self.iotest.pushButton_9)
        togglebuttonList.append(self.iotest.pushButton_10)
        togglebuttonList.append(self.iotest.pushButton_11)
        togglebuttonList.append(self.iotest.pushButton_12)
        togglebuttonList.append(self.iotest.pushButton_13)
        togglebuttonList.append(self.iotest.pushButton_14)
        togglebuttonList.append(self.iotest.pushButton_15)
        togglebuttonList.append(self.iotest.pushButton_16)
        togglebuttonList.append(self.iotest.pushButton_17)
        togglebuttonList.append(self.iotest.pushButton_18)
        togglebuttonList.append(self.iotest.pushButton_19)
        togglebuttonList.append(self.iotest.pushButton_20)

        iobuttonList.append(self.iotest.radioButton_1)
        iobuttonList.append(self.iotest.radioButton_2)
        iobuttonList.append(self.iotest.radioButton_3)
        iobuttonList.append(self.iotest.radioButton_4)
        iobuttonList.append(self.iotest.radioButton_5)
        iobuttonList.append(self.iotest.radioButton_6)
        iobuttonList.append(self.iotest.radioButton_7)
        iobuttonList.append(self.iotest.radioButton_8)
        iobuttonList.append(self.iotest.radioButton_9)
        iobuttonList.append(self.iotest.radioButton_10)
        iobuttonList.append(self.iotest.radioButton_11)
        iobuttonList.append(self.iotest.radioButton_12)
        iobuttonList.append(self.iotest.radioButton_13)
        iobuttonList.append(self.iotest.radioButton_14)
        iobuttonList.append(self.iotest.radioButton_15)
        iobuttonList.append(self.iotest.radioButton_16)
        iobuttonList.append(self.iotest.radioButton_17)
        iobuttonList.append(self.iotest.radioButton_18)
        iobuttonList.append(self.iotest.radioButton_19)
        iobuttonList.append(self.iotest.radioButton_20)

        row1.append(self.window.pushButton_stack_1)
        row1.append(self.window.pushButton_stack_2)
        row1.append(self.window.pushButton_stack_3)
        row1.append(self.window.pushButton_stack_4)
        row1.append(self.window.pushButton_stack_5)

        row2.append(self.window.pushButton_stack_6)
        row2.append(self.window.pushButton_stack_7)
        row2.append(self.window.pushButton_stack_8)
        row2.append(self.window.pushButton_stack_9)
        row2.append(self.window.pushButton_stack_10)

        row3.append(self.window.pushButton_stack_11)
        row3.append(self.window.pushButton_stack_12)
        row3.append(self.window.pushButton_stack_13)
        row3.append(self.window.pushButton_stack_14)
        row3.append(self.window.pushButton_stack_15)

        #togglebuttonList[1] = self.iotest.pushButton_2
        #togglebuttonList[2] = self.iotest.pushButton_3
        #(self.iotest.pushButton_1,self.iotest.pushButton_2,self.iotest.pushButton_3)
        self.window.setWindowFlag(Qt.FramelessWindowHint)
        self.window.show()
        self.window.showFullScreen()

        self.iotest.setWindowFlag(Qt.FramelessWindowHint)
        self.iotest.hide()
        self.error.setWindowFlag(Qt.FramelessWindowHint)
        self.error.hide()
        self.setup.setWindowFlag(Qt.FramelessWindowHint)
        self.setup.hide()


    def setup(self,arg):
        writeshint(2126,CMD_SETUP)

    def toggleIO(self,arg,b=5):
        global togglebuttonList
        global iolist
        print("toggle"+str(b))
        if(iolist[b] == 0):
            iolist[b] = 1
            print("on"+str(b))
        else:
            iolist[b] = 0
            print("off"+str(b))

        #self.sender().setChecked(True)
        #self.sender().setDown(True)
        #self.sender().check(True)
        #self.iotest.pushButton_1.setDown(True)
        #self.iotest.pushButton_2.setDown(True)
        #self.iotest.pushButton_2.check(True)

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

    def PickStack(self,arg):
        print("pickstack")
        writeshint(2126,CMD_PICKSTACK)
    def PutStack(self,arg):
        print("putstack")
        writeshint(2126,CMD_PUTSTACK)
    def Start(self,arg):
        print("start")
        writeshint(2126,CMD_START)
    def Stop(self,arg):
        print("stop")
        writeshint(2126,CMD_STOP)
    def IOTest(self,arg):
        print("iotest")
        writeshint(2126,CMD_GOTOIO)
        self.iotest.showFullScreen()
        self.window.hide()
    def resetCobot(self,arg):
        writeshint(2126,CMD_RESETROBOT)
    def OkSave(self,arg):
        print("Oksave")
        writeshint(2126,CMD_EXIT)
    def Cancel(self,arg):
        writeshint(2126,CMD_CANCEL)
        print("cancel")
    def ContinueErr(self,arg):
        writeshint(2126,CMD_CONTINUE_ERR)
    def exitIO(self,arg):
        print("iotest")
        writeshint(2126,CMD_EXIT)

        #self.window.showFullScreen()
        #self.iotest.hide()
    def Gosetup(self,arg):
        print("setup")
        writeshint(2126,CMD_SETUP)
    def abort(self,arg):
        print("abort")
        writeshint(2126,CMD_ABORT)
    def continue_col(self,arg):
        global jobstarted
        jobstarted = 3 #ta bort
        print("continue col")
        writeshint(2126,CMD_CONTINUE)
    def GoHome(self,arg):
        print("Home")
        writeshint(2126,CMD_GOHOME)
    def GoClear(self,arg):
        print("Clear pallet")
        writeshint(2126,CMD_CLEAR_PALLET_DIALOG)
    def GoHold(self,arg):
        global jobstarted
        print("Hold")
        jobstarted = 5 #ta bort
        #writeshint(2126,CMD_GOHOLD)
    def GoTransport(self,arg):
        print("Transport")
        writeshint(2126,CMD_GOTO_TRANSPORT)
    def GoAnalog1(self,arg):
        global analog1on
        writeshint(747,15)
        #TOGGLEBUTTON, "Run",  "751",
        if(analog1on == True):
            self.iotest.pushButton_run1.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: grey; border-radius: 16px");
            writeshint(751,0)
            analog1on = False
        else:
            writeshint(751,1)
            self.iotest.pushButton_run1.setStyleSheet("color: #FFFFFF; font-size: 34; background-color: green; border-radius: 16px");
            analog1on = True

    def GoDisable(self,arg):
        global robotenabled
        hej = 1
        if(robotenabled == 1):
            print("disable")
            robotenabled = 0
            writeshint(CFG_ENABLE_ROBOT,0)
            self.window.label_enable.setText("Enable Robot")
            self.window.pushButton_disable.setIcon(QtGui.QIcon("/home/pi/toggle_switch_icon_512_off.png"));
        else:
            print("enable")
            writeshint(CFG_ENABLE_ROBOT,1)
            self.window.label_enable.setText("Disable Robot")
            self.window.pushButton_disable.setIcon(QtGui.QIcon("/home/pi/toggle_switch_icon_512_on.png"));
            robotenabled = 1



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
    sys.exit(app.exec_())

